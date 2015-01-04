#include "Driver.h"
#include "runtime.h"

Driver::Driver(const char *name)
{
    _start = NULL;
    _end = NULL;
    _parent = NULL;
    _previous = NULL;
    _next = NULL;
}

Driver::~Driver()
{
    while (_start != NULL)
        DetachChild(_start);
}

void Driver::Ready(void)
{
    // TODO: Driver matching
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
