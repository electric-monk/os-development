#ifndef __FS_ISO9660_H__
#define __FS_ISO9660_H__

#include "Provider.h"

// For any FS:
// 1) register for service appearance
// 2) on relevant service appear, instantiate
// 3) instance should probe
// 4) if bad, quit. If good, start up, register new services

namespace ISO9660Driver {
    class GenericEntry;
    class DirectoryEntry;
};
class Interface_Request;
class Interface_Response;

class FileSystem_ISO9660 : public GenericProvider
{
public:
    CLASSNAME(GenericProvider, FileSystem_ISO9660);
    
    FileSystem_ISO9660();

protected:
    ~FileSystem_ISO9660();

    InputConnection* InputConnectionStart(KernelString *name, IpcEndpoint *connection);
    void InputConnectionReceived(InputConnection *connection, KernelBufferMemory *message);
    void InputConnectionEnd(InputConnection *connection);
    OutputConnection* OutputConnectionStart(Service *source, IpcEndpoint *connection);
    void OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message);
    void OutputConnectionEnd(OutputConnection *oldConnection);
    
private:
    ISO9660Driver::DirectoryEntry *_rootDirectory;
    UInt64 _nodeCounter;
    InterfaceHelper *_tasks;
    
    void PerformTask(IpcEndpoint *destination, bicycle::function<int(Interface_Request*)> generate, bicycle::function<int(Interface_Response*)> response);
    
    void ReadGVD(int offset);
    
    InputConnection* Input(void);
    
    void EnsureEntryLoaded(ISO9660Driver::GenericEntry *entry, bicycle::function<void(int)> onLoaded);
    void ParsePath(ISO9660Driver::GenericEntry *current, FlatArray *path, UInt32 pathIndex, bicycle::function<void(ISO9660Driver::GenericEntry*)> onFound, bicycle::function<void(UInt32 failedIndex, UInt32 reason)> onNotFound);
    void ParsePath(UInt64 startNode, FlatArray *path, UInt32 pathIndex, bicycle::function<void(ISO9660Driver::GenericEntry*)> onFound, bicycle::function<void(UInt32 failedIndex, UInt32 reason)> onNotFound);
};

#endif // __FS_ISO9660_H__
