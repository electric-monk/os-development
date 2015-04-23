// This is to test the FlatObject stuff outside of the OS
#include <stdlib.h>
#include <stdio.h>
#include <Foundation/Foundation.h>

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
//typedef unsigned long UInt64;
#include "Interface.h"

static void RunTest(char *output, id input);

static void Encode(char *output, NSNumber *number)
{
    FlatInteger *numberOut = (FlatInteger*)output;
    numberOut->Initialise([number longLongValue]);
}

static void Encode(char *output, NSString *str)
{
    FlatString *stringOut = (FlatString*)output;
    stringOut->Initialise(str.UTF8String);
}

static void Encode(char *output, NSArray *array)
{
    FlatArray *arrayOut = (FlatArray*)output;
    arrayOut->Initialise();
    for (id object in array) {
        RunTest((char*)arrayOut->GetNextAddress(), object);
        arrayOut->CompleteNextItem();
    }
}

static void Encode(char *output, NSDictionary *dictionary)
{
    FlatDictionary *dictionaryOut = (FlatDictionary*)output;
    dictionaryOut->Initialise();
    for (id key in dictionary.allKeys) {
        id value = [dictionary objectForKey:key];
        RunTest((char*)dictionaryOut->GetNextAddress(), key);
        dictionaryOut->CompleteNextItem();
        RunTest((char*)dictionaryOut->GetNextAddress(), value);
        dictionaryOut->CompleteNextItem();
    }
}

static void Encode(char *output, NSData *data)
{
    FlatBlob *blob = (FlatBlob*)output;
    blob->Initialise(data.bytes, data.length);
}

static void RunTest(char *output, id input)
{
    if ([input isKindOfClass:[NSNumber class]])
        Encode(output, (NSNumber*)input);
    else if ([input isKindOfClass:[NSString class]])
        Encode(output, (NSString*)input);
    else if ([input isKindOfClass:[NSArray class]])
        Encode(output, (NSArray*)input);
    else if ([input isKindOfClass:[NSDictionary class]])
        Encode(output, (NSDictionary*)input);
    else if ([input isKindOfClass:[NSData class]])
        Encode(output, (NSData*)input);
    else
        NSLog(@"Unknown type %@", [input class]);
}

// Main
int main(int argc, char **argv)
{
    char test[4096];
    RunTest(test, @{@"Bob": @"Jim", @"Cat": @1000});
    return 0;
}
