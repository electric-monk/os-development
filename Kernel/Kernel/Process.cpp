#include "Process.h"

Process::Process(const char *name)
{
    pageDirectory.Init();
}

Process::~Process()
{
    pageDirectory.Release();
    // TODO: Should this be somewhere more sane?
    if (Process::Active == this)
        rootAddressSpace.Select();
}
