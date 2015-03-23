#include "Driver.h"
#include "runtime.h"
#include "Collections.h"

static KernelArray *s_factories = NULL;

void Driver::RegisterFactory(DriverFactory *factory)
{
    if (s_factories == NULL)
        s_factories = new KernelArray();
    else if (s_factories->Contains(factory))
        return;
    s_factories->Add(factory);
}

void Driver::UnregisterFactory(DriverFactory *factory)
{
    if (s_factories == NULL)
        return;
    s_factories->Remove(factory);
}

Driver::Driver(const char *name)
{
    _start = NULL;
    _end = NULL;
    _parent = NULL;
    _previous = NULL;
    _next = NULL;
    _properties = new KernelDictionary();
    // Do this manually, as we might not yet be able to autorelease
    KernelString *property = new KernelString("name");
    KernelString *value = new KernelString(name);
    SetProperty(property, value);
    value->Release();
    property->Release();
}

Driver::~Driver()
{
    while (_start != NULL)
        DetachChild(_start);
    _properties->Release();
}

Driver* Driver::Child(int index)
{
    Driver *children = _start;
    while ((children != NULL) && (index != 0)) {
        children = children->_next;
        index--;
    }
    return children;
}

static int SortFunc(KernelObject *a, KernelObject *b)
{
    DriverFactory::Match *matchA = (DriverFactory::Match*)a;
    DriverFactory::Match *matchB = (DriverFactory::Match*)b;
    return matchA->MatchValue() - matchB->MatchValue();
}

bool Driver::Start(Driver *parent)
{
    if (s_factories == NULL)
        return true;
    AutoreleasePool pool;
    // Work out what drivers might work
    KernelArray *matches = new KernelArray();
    for (UInt32 index = 0; index < s_factories->Count(); index++) {
        DriverFactory *factory = (DriverFactory*)s_factories->ObjectAt(index);
        KernelArray *items = factory->MatchForParent(this);
        if (items != NULL)
            matches->AddArray(items);
    }
    // Sort them in order of matchingness
    matches->Sort(SortFunc);
    // Attempt to pick one
    for (UInt32 index = 0; index < matches->Count(); index++) {
        DriverFactory::Match *match = (DriverFactory::Match*)matches->ObjectAt(index);
        Driver *potentialChild = match->Instantiate();
        ConnectChild(potentialChild);
        potentialChild->Release();  // We either own it or dump it now
        if (potentialChild->Start(this)) {
            if (!match->MatchMultiple())
                break;
        } else {
            DetachChild(potentialChild);
        }
    }
    return true;
}

void Driver::Stop(void)
{
    while (_start != NULL) {
        _start->Stop();
        DetachChild(_start);
    }
}

Interrupts* Driver::InterruptSource(void)
{
    if (_parent != NULL)
        return _parent->InterruptSource();
    else
        return NULL;
}

bool Driver::ConnectChild(Driver *child)
{
    if (child->_parent != NULL)
        return false;
    child->AddRef();
    child->_parent = this;
    if (_end == NULL) {
        _start = _end = child;
    } else {
        _end->_next = child;
        child->_previous = _end;
        _end = child;
    }
    return true;
}

void Driver::DetachChild(Driver *child)
{
    if (child->_parent != this)
        return;
    if (child->_previous != NULL)
        child->_previous->_next = child->_next;
    if (child->_next != NULL)
        child->_next->_previous = child->_previous;
    if (_start == child)
        _start = child->_next;
    if (_end == child)
        _end = child->_previous;
    child->_parent = NULL;
    child->_previous = NULL;
    child->_next = NULL;
    child->Release();
}

KernelArray* Driver::PropertyList(void)
{
    return _properties->AllKeys();
}

KernelObject* Driver::PropertyFor(KernelObject *property)
{
    return _properties->ObjectFor(property);
}

void Driver::SetProperty(KernelObject *property, KernelObject *value)
{
    _properties->Set(property, value);
}
