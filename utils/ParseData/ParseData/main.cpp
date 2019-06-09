//
//  main.cpp
//  ParseData
//
//  Created by Colin David Munro on 9/06/2017.
//  Copyright © 2017 MICE Software. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef u_int64_t UInt64;
typedef u_int32_t UInt32;
#include "Interface.h"

static void PrintObject(FILE *outFile, FlatObject *object)
{
    if (object->type == FlatBlob::Type()) {
        FlatBlob *blob = (FlatBlob*)object;
        fprintf(outFile, ":");
        unsigned char *data = (unsigned char*)blob->Data();
        UInt32 count = blob->Length();
        for (UInt32 i = 0; i < count; i++)
            fprintf(outFile, "%.2x", data[i]);
    } else if (object->type == FlatString::Type()) {
        fprintf(outFile, "\"%s\"", ((FlatString*)object)->Value());
    } else if (object->type == FlatInteger::Type()) {
        fprintf(outFile, "%llu", ((FlatInteger*)object)->Value());
    } else if (object->type == FlatArray::Type()) {
        FlatArray *array = (FlatArray*)object;
        UInt32 count = array->Count();
        char c = '[';
        for (UInt32 i = 0; i < count; i++) {
            fprintf(outFile, "%c", c);
            PrintObject(outFile, array->ItemAt(i));
            c = ',';
        }
        fprintf(outFile, "]");
    } else if (object->type == FlatDictionary::Type()) {
        FlatDictionary *dictionary = (FlatDictionary*)object;
        UInt32 count = dictionary->Count();
        char c = '{';
        for (UInt32 i = 0; i < count; i++) {
            fprintf(outFile, "%c", c);
            PrintObject(outFile, dictionary->ItemAt(i));
            c = (c == ':') ? ',' : ':';
        }
        if (c == ':')
            fprintf(stderr, "Unmatched key:value count in dictionary\n");
        fprintf(outFile, "}");
    } else if (object->type == FlatDate::Type()) {
        FlatDate *date = (FlatDate*)object;
        fprintf(outFile, "%.2i:%.2i:%.2i.%.3i %i/%.2i/%.4i", date->Hour, date->Minute, date->Second, date->Millisecond, date->Day, date->Month, date->Year);
    } else {
        fprintf(outFile, "?%c%c%c%c(%i)", object->type >> 24, (object->type >> 16) & 0xFF, (object->type >> 8) & 0xFF, object->type & 0xFF, object->length);
    }
}

static bool IsWhitespace(char c)
{
    return (c == ' ') || (c == '\t') || (c == '\r') || (c == '\f') || (c == '\n');
}

static bool SkipWhitespace(const char *input, UInt64 &inPosition, UInt64 inputSize)
{
    while ((inPosition < inputSize) && IsWhitespace(input[inPosition]))
        inPosition++;
    return inPosition < inputSize;
}

static bool FromHex(char input, unsigned char &output)
{
    if ((input >= '0') && (input <= '9')) {
        output |= input - '0';
        return true;
    }
    if ((input >= 'a') && (input <= 'f')) {
        output |= input - 'a' + 10;
        return true;
    }
    if ((input >= 'A') && (input <= 'F')) {
        output |= input - 'A' + 10;
        return true;
    }
    return false;
}

static bool IsDigit(char c)
{
    return (c >= '0') && (c <= '9');
}

static bool Parse(const char *input, UInt64 &inPosition, UInt64 inputSize, FlatObject *currentOutput)
{
    if (!SkipWhitespace(input, inPosition, inputSize))
        return true;
    switch (input[inPosition]) {
        case '[':   // array
        {
            bool start = true;
            FlatArray *array = (FlatArray*)currentOutput;
            array->Initialise();
            while (true) {
                if (!SkipWhitespace(input, inPosition, inputSize)) {
                    fprintf(stderr, "End of file processing array\n");
                    return false;
                }
                if (input[inPosition] == ']') {
                    inPosition++;
                    break;
                } else if (!start && (input[inPosition] != ',')) {
                    fprintf(stderr, "Unexpected token '%c' in array\n", input[inPosition]);
                    return false;
                }
                start = false;
                inPosition++;
                if (!Parse(input, inPosition, inputSize, array->GetNextAddress()))
                    return false;
                array->CompleteNextItem();
            }
        }
            break;
        case '{':   // dictionary
        {
            FlatDictionary *dictionary = (FlatDictionary*)currentOutput;
            dictionary->Initialise();
            char expected = '{';
            while (true) {
                if (!SkipWhitespace(input, inPosition, inputSize)) {
                    fprintf(stderr, "End of file processing dictionary\n");
                    return false;
                }
                if (input[inPosition] == '}') {
                    inPosition++;
                    break;
                } else if (input[inPosition] != expected) {
                    fprintf(stderr, "Unexpected token '%c' in dictionary\n", input[inPosition]);
                    return false;
                }
                inPosition++;
                if (!Parse(input, inPosition, inputSize, dictionary->GetNextAddress()))
                    return false;
                dictionary->CompleteNextItem();
                expected =  (expected == ':') ? ',' : ':';
            }
            if (expected == ':') {
                fprintf(stderr, "Mismatched key/values in dictionary\n");
                return false;
            }
        }
            break;
        case '\"':  // string
        {
            FlatString *string = (FlatString*)currentOutput;
            char temp[1024];
            bool escape = false;
            int pos = 0;
            inPosition++;
            while (inPosition < inputSize) {
                if (input[inPosition] == '\"') {
                    if (escape) {
                        temp[pos++] = '\"';
                        escape = false;
                    } else
                        break;
                } else {
                    if (escape) {
                        temp[pos++] = '\"';
                        escape = false;
                    }
                    temp[pos++] = input[inPosition];
                }
                inPosition++;
            }
            if (inPosition >= inputSize) {
                fprintf(stderr, "Found EOF parsing string\n");
                return false;
            }
            temp[pos++] = '\0';
            string->Initialise(temp);
            inPosition++;
        }
            break;
        case ':':   // blob
        {
            FlatBlob *blob = (FlatBlob*)currentOutput;
            char temp[10240];
            int pos = 0;
            bool nibble = false;
            unsigned char c = 0;
            while (inPosition < inputSize) {
                inPosition++;
                if (!SkipWhitespace(input, inPosition, inputSize))
                    break;
                if (nibble) {
                    if (!FromHex(input[inPosition], c))
                        break;
                    temp[pos++] = c;
                    c = 0;
                } else {
                    if (!FromHex(input[inPosition], c))
                        break;
                    c <<= 4;
                }
                nibble = !nibble;
            }
            if (nibble) {
                fprintf(stderr, "Blob ended on half a byte\n");
                return false;
            }
            blob->Initialise(temp, pos);
        }
            break;
        case '0':   // number/date
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            FlatInteger *integer = (FlatInteger*)currentOutput;
            UInt32 value = 0;
            while ((inPosition < inputSize) && IsDigit(input[inPosition])) {
                value *= 10;
                value += input[inPosition++] - '0';
            }
            integer->Initialise(value);
        }
            break;
        default:
            fprintf(stderr, "Unknown object type '%c'\n", input[inPosition]);
            return false;
    }
    return true;
}

int main(int argc, const char * argv[])
{
    if (argc != 4) {
        fprintf(stdout, "Usage: %s <tobin/totext> <input> <output>\n", argv[0]);
        return -1;
    }
    bool tobin = strcmp(argv[1], "tobin") == 0;
    if (!tobin && (strcmp(argv[1], "totext") != 0)) {
        fprintf(stdout, "Invalid mode '%s'\n", argv[1]);
        return -2;
    }
    
    FILE *inFile = fopen(argv[2], "rb");
    if (!inFile) {
        fprintf(stdout, "Couldn't read '%s'\n", argv[2]);
        return -3;
    }
    fseek(inFile, 0, SEEK_END);
    unsigned long inSize = ftell(inFile);
    fseek(inFile, 0, SEEK_SET);
    char *input = new char[inSize];
    fread(input, inSize, 1, inFile);
    fclose(inFile);
    
    FILE *outFile = fopen(argv[3], "wb");
    
    if (tobin) {
        // [array item, array item]
        // "string"
        // {key: value, key:value}
        // int
        // :00FF00aa, !filename
        // hh:mm:ss.mmm dd/mm/yyyy
        UInt64 offset = 0;
        char temp[1024 * 1024];
        FlatObject *current = (FlatObject*)&temp;
        while (true) {
            Parse(input, offset, inSize, current);
            SkipWhitespace(input, offset, inSize);
            current = current->NextObject(true);
            if (offset == inSize)
                break;
        }
        ((FlatEnd*)current)->Initialise();
        fwrite(temp, UInt64(current) + sizeof(FlatEnd) - UInt64(temp), 1, outFile);
    } else {
        FlatObject *object = (FlatObject*)input;
        while (object->type != FlatEnd::Type()) {
            PrintObject(outFile, object);
            object = object->NextObject(true);
        }
        if ((UInt64(object) + sizeof(FlatEnd)) != (UInt64(input) + inSize))
            fprintf(stderr, "Extra bytes found: %llu\n", inSize - (UInt64(object) + sizeof(FlatEnd)));
    }
    
    fclose(outFile);
    
    return 0;
}
