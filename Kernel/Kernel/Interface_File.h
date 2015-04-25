#ifndef __INTERFACE_FILE_H__
#define __INTERFACE_FILE_H__

#include "Interface.h"

/* REQUESTS - requests that can be sent to these services */

class NodeRequest : public Interface_Request
{
public:
    static const UInt64 RootNode = (UInt64)-1;
    
    static const UInt32 Search = Interface_Request::MAX + 0;        // DirectoryRequest -> DirectoryResponse
    static const UInt32 OpenFile = Interface_Request::MAX + 1;      // OpenRequest -> OpenResponse
    static const UInt32 CloseFile = Interface_Request::MAX + 2;     // FileRequest -> OpenResponse
    static const UInt32 ReadFile = Interface_Request::MAX + 3;      // ReadRequest -> ReadResponse
    static const UInt32 MAX = Interface_Request::MAX + 100;
    
    UInt64 rootNode;
};

class DirectoryRequest : public NodeRequest
{
public:
    UInt64 offset, length;  // In entries - allows requesting a portion of a directory listing
    FlatArray subpath;  // Optional, to allow searching subdirectories without having to walk down the directories manually
};

class OpenRequest : public NodeRequest
{
public:
    FlatArray subpath; // Each component of the path should be a single string entry, or integer node.
};

class FileRequest : public NodeRequest
{
public:
    UInt32 handle;
};

class ReadRequest : public FileRequest
{
public:
    UInt64 offset, length;  // In bytes
};

/* RESPONSES - responses that will be received from these services */

class NodeResponse : public Interface_Response
{
public:
    static const int NotFound = Interface_Response::MAX + 0;
    static const int NotDirectory = Interface_Response::MAX + 1;
    static const int NotFile = Interface_Response::MAX + 2;
    static const int IOError = Interface_Response::MAX + 3;
    static const int MAX = Interface_Response::MAX + 100;
};

#define Node_ID             "id"        /* FlatInteger */
#define Node_Name           "name"      /* FlatString */
#define Node_Type           "type"      /* FlatString as below */
#define Node_Size           "size"      /* FlatInteger */
#define Node_Date_Create    "dcreat"    /* FlatDate */
#define Node_Date_Modify    "dmod"      /* FlatDate */

// Node_Type values
#define NoteType_Directory  "x-system/directory"
#define NoteType_PlainFile  "x-system/file/unknown"

class DirectoryResponse /* : public NodeResponse */
{
public:
    // From NodeResponse
    UInt32 status;
    DirectoryRequest originalRequest;
    // Directory response
    FlatArray directoryEntries;
        // FlatDictionary: metadata
};

class OpenResponse /* : public NodeResponse */
{
public:
    // From NodeResponse
    UInt32 status;
    FileRequest originalRequest;
    // Open response
    UInt32 handle;
};

class ReadResponse /* : public NodeResponse */
{
public:
    // From NodeResponse
    UInt32 status;
    ReadRequest originalRequest;
    // Open response
    UInt64 actualOffset;
    UInt64 actualLength;
    char* rawData(void) { return ((char*)this) + sizeof(*this); }
    char* data(void) { return rawData() + (originalRequest.offset - actualOffset); }
};

#endif // __INTERFACE_FILE_H__
