// This is to test the Basic Heap functionality outside of the OS
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Pull in the actual class to test
#define kprintf printf
#define TESTING
#include "BasicHeap.cpp"

// Test class
class TestHarness
{
private:
    class Blob
    {
    public:
        Blob(Blob **start, Blob **end, void *offset, BasicHeap::h_size length)
        {
            _offset = (char*)offset;
            _length = length;
            _start = start;
            _end = end;
            _next = NULL;
            _previous = *_end;
            if (_previous == NULL)
                *_start = this;
            else
                _previous->_next = this;
            *_end = this;
        }
        
        ~Blob()
        {
            if (_previous != NULL)
                _previous->_next = _next;
            if (_next != NULL)
                _next->_previous = _previous;
            if (*_start == this)
                *_start = _next;
            if (*_end == this)
                *_end = _previous;
        }

        char *_offset;
        BasicHeap::h_size _length;

        Blob **_start, **_end, *_previous, *_next;
    };
    
    Blob *_startBlob, *_endBlob;
    
    bool IsWithinBlob(void *offset, BasicHeap::h_size length)
    {
        char *cOffset = (char*)offset;
        if (_startBlob == NULL)
            return false;
        for (Blob *blob = _startBlob; blob != NULL; blob = blob->_next) {
            if ((blob->_offset <= cOffset) && ((blob->_offset + blob->_length) >= (cOffset + length)))
                return true;
        }
        /* Reporting failure state */
        printf("Allocation not within blob: %p -> %i\n", offset, length);
        for (Blob *blob = _startBlob; blob != NULL; blob = blob->_next)
            printf("- %p -> %i\n", blob->_offset, blob->_length);
        return false;
    }
    
    Blob *_startAllocation, *_endAllocation;
    
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
    bool IsWithinAllocation(void *offset, BasicHeap::h_size length)
    {
        char *cOffset = (char*)offset;
        if (_startAllocation == NULL)
            return false;
        for (Blob *blob = _startAllocation; blob != NULL; blob = blob->_next) {
            char *x1 = cOffset;
            char *x2 = x1 + length;
            char *y1 = blob->_offset;
            char *y2 = y1 + blob->_length;
            if (MAX(x1, y1) <= MIN(x2, y2)) {
                /* Reporting failure state */
                printf("Allocation collided: %p -> %i\n", offset, length);
                printf("-> collission: %p -> %i\n", blob->_offset, blob->_length);
                return true;
            }
        }
        return false;
    }
    Blob* FindAllocation(void *offset)
    {
        if (_startAllocation == NULL)
            return NULL;
        for (Blob *blob = _startAllocation; blob != NULL; blob = blob->_next) {
            if (blob->_offset == offset)
                return blob;
        }
        return NULL;
    }
    
    int TotalInput(void)
    {
        int total = 0;
        for (Blob *blob = _startBlob; blob != NULL; blob = blob->_next)
            total += blob->_length;
        return total;
    }
    int TotalMemory(void)
    {
        int total = 0;
        for (Blob *blob = _startAllocation; blob != NULL; blob = blob->_next)
            total += blob->_length;
        return total;
    }
    
public:
    TestHarness(BasicHeap::h_size granularity)
    :_testHeap(granularity)
    {
        printf("<TestHarness:%p> created: granularity %i\n", this, granularity);
        _startBlob = _endBlob = NULL;
        _startAllocation = _endAllocation = NULL;
    }
    
    ~TestHarness()
    {
        while (_startBlob != NULL)
            delete _startBlob;
        while (_startAllocation != NULL)
            delete _startAllocation;
        printf("<TestHarness:%p> destroyed\n", this);
    }
    
    void AddBlock(void *offset, BasicHeap::h_size length)
    {
        printf("<TestHarness:%p> adding block %p length %i bytes\n", this, offset, length);
        _testHeap.AddBlock(offset, length);
        new Blob(&_startBlob, &_endBlob, offset, length);
    }
    
    void* Alloc(BasicHeap::h_size amount)
    {
        printf("<TestHarness:%p> allocating %i bytes\n", this, amount);
        void *data = _testHeap.Alloc(amount);
        if (data == NULL) {
            printf("<TestHarness:%p> failed\n", this);
            _testHeap.Test();
            printf("%i allocated of %i heap\n", TotalMemory(), TotalInput());
//            throw "Check 0: Allocation failure";
            return NULL;
        }
        // Consistency check 1: it's within expected blocks
        if (!IsWithinBlob(data, amount))
            throw "Check 1: Output is not within input";
        // Consistency check 2: it's not overlapping
        if (IsWithinAllocation(data, amount))
            throw "Check 2: Output overlaps another allocation";
        // All okay so far? Plop it in the list
        new Blob(&_startAllocation, &_endAllocation, data, amount);
        // Done
        printf("<TestHarness:%p> returning %p\n", this, data);
        return data;
    }
    
    void Free(void *data)
    {
        printf("<TestHarness:%p> freeing %p\n", this, data);
        Blob *allocation = FindAllocation(data);
        if (allocation == NULL)
            throw "TestHarness error: Could't find allocation";
        delete allocation;
        _testHeap.Release(data);
    }

    void CheckAllocations(void)
    {
        if (_startAllocation == NULL)
            return;
        for (Blob *blob = _startAllocation; blob != NULL; blob = blob->_next)
            _testHeap.Check(blob->_offset);
    }
    
    int Blocks(void)
    {
        int result = 0;
        for (Blob *blob = _startBlob; blob != NULL; blob = blob->_next)
            result++;
        return result;
    }
    void* BlockAt(int index, BasicHeap::h_size *sizeOut)
    {
        Blob *blob;
        for (blob = _startBlob; (blob != NULL) && (index != 0); blob = blob->_next)
            index--;
        if (blob == NULL)
            return NULL;
        if (sizeOut != NULL)
            *sizeOut = blob->_length;
        return blob->_offset;
    }

    int Allocations(void)
    {
        int result = 0;
        for (Blob *blob = _startAllocation; blob != NULL; blob = blob->_next)
            result++;
        return result;
    }
    void* AllocationAt(int index, BasicHeap::h_size *sizeOut)
    {
        Blob *blob;
        for (blob = _startAllocation; (blob != NULL) && (index != 0); blob = blob->_next)
            index--;
        if (blob == NULL)
            return NULL;
        if (sizeOut != NULL)
            *sizeOut = blob->_length;
        return blob->_offset;
    }
    
private:
    BasicHeap _testHeap;
};

static void SeedRandom(int seed)
{
    srand(seed);
}
static int Random(int min, int max)
{
    unsigned long long r = rand();
    unsigned long long mn = min;
    unsigned long long mx = max;
    return mn + ((r * (mx - mn)) / RAND_MAX);
}
static int SelectGranularity(void)
{
    int power = Random(6, 12);
    int scale = 1;
    while (power) {
        scale *= 2;
        power--;
    }
    return scale;
}

static void FillWithNonsense(unsigned char *c, BasicHeap::h_size length)
{
    for (BasicHeap::h_size i = 0; i < length; i++)
        c[i] = i & 0xFF;
}

static bool CheckNonsense(unsigned char *c, BasicHeap::h_size length)
{
    for (BasicHeap::h_size i = 0; i < length; i++) {
        if (c[i] != (i & 0xFF)) {
            printf("Checking buffer failed: index %i of %i: expected %.2x, found %.2x\n",
                   i, length,
                   i & 0xFF, c[i]);
            return false;
        }
    }
    return true;
}

static void DoTest()
{
    // Initialise
    int gran = SelectGranularity();
    TestHarness harness(gran);
    // Allocate some buffers
    int blockCount = Random(1, 4);
    for (int i = 0; i < blockCount; i++) {
        int len = Random(16384, 1024 * 1024);
        len &= ~0x3FF;
        harness.AddBlock(new char[len], len);
    }
    // Do some memory things
    int things = Random(1, 100000);
    for (int i = 0; i < things; i++) {
        int bob = Random(-1000, 3000);
        int total = harness.Allocations();
        if ((total == 0) && (bob < 0))
            bob = -bob;
        int removing = -1;
        if (bob < 0) {
            removing = (-bob) % total;
        } else {
            void *data = harness.Alloc(bob);
            if (data == NULL)
                removing = bob % total;
            else
                FillWithNonsense((unsigned char*)data, bob);
        }
        if (removing != -1) {
            printf("Removing %i from %i allocations\n", removing, total);
            BasicHeap::h_size length;
            void *data = harness.AllocationAt(removing, &length);
            if (!CheckNonsense((unsigned char*)data, length))
                throw "Nonsense changed";
            harness.Free(data);
        }
        // Each loop around, confirm integrity
        harness.CheckAllocations();
    }
    // Release everything
    while (true) {
        BasicHeap::h_size length;
        void *data = harness.AllocationAt(0, &length);
        if (data == NULL)
            break;
        if (!CheckNonsense((unsigned char*)data, length))
            throw "Nonsense changed";
        harness.Free(data);
    }
    // Tidy up
    int x = 0;
    while (true) {
        char *block = (char*)harness.BlockAt(x, NULL);
        if (block == NULL)
            break;
        delete[] block;
        x++;
    }
    // Done
    printf("Test completed OK\n");
}

// 1426407430

// Main
int main(int argc, char **argv)
{
    int seed;
    if (argc == 2)
        seed = atoi(argv[1]);
    else
        seed = time(NULL);
    try {
        printf("*** SEED %i%s ***\n", seed, (argc == 2) ? " (via command line)" : "");
        srand(seed);
    
        for (int i = 0; i < 1000; i++) {
            printf("*** BEGINNING TEST %i ***\n", i);
            DoTest();
        }
    }
    catch (const char *error) {
        printf("FAILURE: %s\n", error);
    }
}
