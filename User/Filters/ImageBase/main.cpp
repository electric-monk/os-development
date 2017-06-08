#include "IPC.h"
#include "Collections.h"
#include "EventLoop.h"
#include "Interface_Block.h"
#include "ImageBase.h"

extern "C" void sysmain(void)
{
    EventLoop loop;
    Kernel::IPC::Client *input = NULL;
    // Create our provider
    Kernel::IPC::Provider *provider = Kernel::IPC::Provider::Create();
    EventLoop::Token providerToken = loop.AddSource(provider, [=](Kernel::Blocking::Blockable *trigger, Kernel::Collections::Array *others){
        Kernel::IPC::Connection *connection;
        Kernel::IPC::Memory *message;
        Kernel::IPC::ProviderIO *providerConnection;
        Kernel::IPC::Provider::EventType type = provider->GetEvent(&connection, &message, &providerConnection);
        switch(type) {
            case Kernel::IPC::Provider::eventInputConnect:
                // Request entire file, easiest thing to do just now
            {
                Kernel::IPC::Memory *request = Kernel::IPC::Memory::Create(4096);
                Kernel::Handle *handle;
                BlockRequestRead *blockRequest = (BlockRequestRead*)request->GetPointer(&handle);
                blockRequest->packetClass = BlockRequest::Request;
                blockRequest->identifier = 0;
                blockRequest->type = BlockRequest::Read;
                blockRequest->offset = 0;
                blockRequest->length = 100000000; // Just a large number
                handle->Release();
                connection->Send(request);
                request->Release();
            }
                break;
            case Kernel::IPC::Provider::eventInputMessage:
                // Process file
            {
                bool success = true;
                Kernel::Handle *handle;
                BlockResponseRead *responseRead = (BlockResponseRead*)message->GetPointer(&handle);
                if (success)
                    success = responseRead->status == BlockResponseRead::Success;
                // Attempt to parse
                if (success) {
                    ImageDecoder *decoder = AttemptParseImage((const UInt8*)responseRead->data(), responseRead->readLength);
                    if (!decoder)
                        success = false;
                }
                // Finally
                handle->Release();
                if (success) {
                    // TODO: start output
                } else {
                    input->Disconnect();
                }
            }
                break;
        }
    });
    // Create our input
    loop.AddTask([&]{
        Kernel::Collections::String *inputName = Kernel::Collections::String::Create("input");
        input = provider->CreateClient(inputName);
        inputName->Release();
    });
    // Begin waiting for events
    loop.Run();
    // Tidy up
    loop.RemoveSource(providerToken);
    if (input)
        input->Release();
    provider->Release();
}
