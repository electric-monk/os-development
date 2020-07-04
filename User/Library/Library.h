#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#include "Runtime.h"
#include "Function.h"

namespace Library {
    template<class T> T Max(const T& a, const T& b)
    {
        return (a > b) ? a : b;
    }
    template<class T> T Min(const T& a, const T& b)
    {
        return (a < b) ? a : b;
    }

    template<class T> void Swap(T &a, T &b)
    {
        UInt8 temp[sizeof(T)];
        CopyMemory(temp, &a, sizeof(T));
        CopyMemory(&a, &b, sizeof(T));
        CopyMemory(&b, temp, sizeof(T));
    }
    inline void Swap(UInt32 a, UInt32 b)
    {
        UInt32 temp = a;
        a = b;
        b = temp;
    }
    inline void Swap(double a, double b)
    {
        double temp = a;
        a = b;
        b = temp;
    }
    
    template<class Iterator, typename Sorter> UInt64 _QuickSortPartition(Iterator start, UInt64 lo, UInt64 hi, Sorter comparator)
    {
        decltype(*start) pivot = *(start + lo);
        UInt64 i = lo - 1;
        UInt64 j = hi + 1;
        while (true) {
            do {
                i++;
            } while(comparator(*(start + i), pivot));
            do {
                j--;
            } while(comparator(pivot, *(start + j)));
            if (i >= j)
                return j;
            Swap(*(start + i), *(start + j));
        }
    }
    
    template<class Iterator, typename Sorter> void _QuickSort(Iterator start, UInt64 lo, UInt64 hi, Sorter comparator)
    {
        if (lo >= hi)
            return;
        UInt64 p = _QuickSortPartition(start, lo, hi, comparator);
        _QuickSort(start, lo, p, comparator);
        _QuickSort(start, p + 1, hi, comparator);
    }
    
    template<class Iterator, typename Sorter> void Sort(Iterator start, Iterator end, Sorter comparator)
    {
        // TODO: Measure sensibly?
        UInt64 length = end - start;
        if (length == 0)
            return;
        // Quicksort
        _QuickSort(start, 0, length - 1, comparator);
    }
    
    template<class T> UInt64 Hash(T value) { return UInt64(value); }
    
    template<class C> void ForEach(const C &iterable, Library::Function<bool(decltype(*iterable.Start())&)> handler)
    {
        for (auto it = iterable.Start(); it != iterable.End(); it++) {
            if (!handler(*it))
                break;
        }
    }
    
    template<class Iterator, class T> Iterator Find(Iterator start, Iterator end, const T& find)
    {
        auto it = start;
        for (; it != end; it++) {
            if (find == *it)
                break;
        }
        return it;
    }
    
    template<class ObjType> class Array
    {
    public:
        typedef ObjType* Iterator;
        typedef const ObjType* ConstIterator;
        
        Array()
        {
            _count = 0;
            _max = 4;
            _index = new char[sizeof(ObjType) * _max];
        }
        Array(const Array &other)
        {
            _count = other._count;
            _max = other._max;
            _index = new char[sizeof(ObjType) * _max];
            ObjType *to = reinterpret_cast<ObjType*>(_index);
            ObjType *from = reinterpret_cast<ObjType*>(other._index);
            for (UInt32 i = _count; i; i--, to++, from++)
                new (to) ObjType(*from);
        }
        ~Array()
        {
            Clear();
            delete[] _index;
        }
        Array& operator=(const Array &other)
        {
            // Destruct old objects
            for (UInt32 i = 0; i < _count; i++)
                reinterpret_cast<ObjType*>(_index)[i].~ObjType();
            // Resize, if necessary
            if (_max < other._count) {
                delete[] _index;
                _max = other._max;
                _index = new char[sizeof(ObjType) * _max];
            }
            _count = other._count;
            // Install new objects
            for (UInt32 i = 0; i < _count; i++)
                new (_index + (sizeof(ObjType) * i)) ObjType(reinterpret_cast<ObjType*>(other._index)[i]);
            return *this;
        }
        
        Iterator begin(void)
        {
            return reinterpret_cast<ObjType*>(_index);
        }
        Iterator end(void)
        {
            return reinterpret_cast<ObjType*>(_index) + _count;
        }
        ConstIterator begin(void) const
        {
            return reinterpret_cast<ObjType*>(_index);
        }
        ConstIterator end(void) const
        {
            return reinterpret_cast<ObjType*>(_index) + _count;
        }
        
        void Clear(void)
        {
            ObjType *objs = reinterpret_cast<ObjType*>(_index);
            for (UInt32 i = _count; i; i--, objs++)
                objs->~ObjType();
            _count = 0;
        }
        
        Array Reverse(void) const
        {
            if (_count < 2)
                return *this;
            Array result(*this);
            for (UInt32 i = 0, j = _count - 1; i < j; i++, j--)
                Swap(reinterpret_cast<ObjType*>(result._index)[i], reinterpret_cast<ObjType*>(result._index)[j]);
            return result;
        }
        
        ObjType& operator[](UInt32 index)
        {
            return reinterpret_cast<ObjType*>(_index)[index];
        }
        const ObjType& operator[](UInt32 index) const
        {
            return reinterpret_cast<ObjType*>(_index)[index];
        }
        void Insert(UInt32 location, const ObjType &object)
        {
            CheckSize();
            CopyMemory(_index + (sizeof(ObjType) * (location + 1)), _index + (sizeof(ObjType) * location), sizeof(ObjType) * (_count - location));
            _count++;
            new (_index + (sizeof(ObjType) * location)) ObjType(object);
        }
        void Add(const ObjType &object)
        {
            CheckSize();
            new ((ObjType*)(_index + (sizeof(ObjType) * (_count++)))) ObjType(object);
        }
        void Remove(UInt32 index)
        {
            reinterpret_cast<ObjType*>(_index)[index].~ObjType();
            CopyMemory(_index + (sizeof(ObjType) * index), _index + (sizeof(ObjType) * (index + 1)), sizeof(ObjType) * (_count - (index + 1)));
            _count--;
            CheckSize();
        }
        void Remove(const ObjType &object)
        {
            for (UInt32 i = 0; i < _count; i++) {
                if (reinterpret_cast<ObjType*>(_index)[i] == object) {
                    Remove(i);
                    break;
                }
            }
            CheckSize();
        }
        
        UInt32 Count(void) const
        {
            return _count;
        }
        
        void PrepareFor(UInt32 maxAdditions)
        {
            CheckSize(maxAdditions);
        }
        
    private:
        char *_index;
        UInt32 _count, _max;
        
        void CheckSize(UInt32 adding = 1)
        {
            UInt32 newMax = _max;
            if ((_count + adding) >= _max)
                newMax += Max(UInt32(512), adding);
            if (newMax != _max) {
                char *newIndex = new char[sizeof(ObjType) * newMax];
                // I don't think this is safe
                CopyMemory(newIndex, _index, sizeof(ObjType) * _count);
                delete[] _index;
                _index = newIndex;
                _max = newMax;
            }
        }
    };
    
    template<class KeyType, class ValueType> class Dictionary
    {
    public:
        class KeyValuePair
        {
        public:
            KeyValuePair(KeyType rkey, ValueType rvalue)
            :key(rkey), value(rvalue)
            {
            }
            
            KeyType key;
            ValueType value;
        };
    private:
        class HashEntry
        {
        public:
            KeyValuePair& Find(const KeyType &key)
            {
                for (UInt32 i = 0; i < _entries.Count(); i++)
                    if (_entries[i].key == key)
                        return _entries[i];
                // TODO: good idea? Add a new empty entry
                _entries.Add(KeyValuePair(key, ValueType()));
                return _entries[_entries.Count() - 1];
            }
            void Set(const KeyType &key, ValueType &value)
            {
                for (UInt32 i = 0; i < _entries.Count(); i++) {
                    if (_entries[i].key == key) {
                        _entries[i].value = value;
                        return;
                    }
                }
                _entries.Add(KeyValuePair(key, value));
            }
            void Remove(const KeyType &key)
            {
                for (UInt32 i = 0; i < _entries.Count(); i++) {
                    if (_entries[i].key == key) {
                        _entries.Remove(i);
                        break;
                    }
                }
            }
            
            void Clear(void)
            {
                while (_entries.Count() != 0)
                    _entries.Remove(0);
            }
            
            typename Array<KeyValuePair>::Iterator begin(void)
            {
                return _entries.begin();
            }

            typename Array<KeyValuePair>::Iterator end(void)
            {
                return _entries.end();
            }

            typename Array<KeyValuePair>::ConstIterator begin(void) const
            {
                return _entries.begin();
            }
            
            typename Array<KeyValuePair>::ConstIterator end(void) const
            {
                return _entries.end();
            }

        private:
            Array<KeyValuePair> _entries;
        };

        KeyValuePair& Get(KeyType key)
        {
            UInt32 index = Hash(key) % _slots.Count();
            return _slots[index].Find(key);
        }
        
//        friend class KvpIterator;
    public://for gcc
        Array<HashEntry> _slots;
        
        // TODO: resize slots
        
    public:
        class KvpIterator
        {
        private:
            const Dictionary &_owner;
            typename Array<HashEntry>::ConstIterator _current;
            typename Array<KeyValuePair>::ConstIterator _active;
        public:
            KvpIterator(const Dictionary &owner, bool end)
            :_owner(owner), _current(end ? owner._slots.end() : owner._slots.begin())
            {
                if (!end) {
                    _active = (*_current).begin();
                    while ((_active == (*_current).end()) && (_current != _owner._slots.end())) {
                        _current++;
                        _active = (*_current).begin();
                    }
                }
            }
            KvpIterator(const KvpIterator &other)
            :_owner(other._owner)
            {
                _current = other._current;
                _active = other._active;
            }
            KvpIterator& operator=(const KvpIterator &other)
            {
                _owner = other._owner;
                _current = other._current;
                _active = other._active;
                return *this;
            }
            KvpIterator& operator++(void)
            {
                _active++;
                if (_active == (*_current).end()) {
                    while (_current != _owner._slots.end()) {
                        _current++;
                        if (_current == _owner._slots.end())
                            break;
                        _active = (*_current).begin();
                        if (_active != (*_current).end())
                            break;
                    }
                }
                return *this;
            }
            KvpIterator operator++(int)
            {
                KvpIterator ret = *this;
                this->operator++();
                return ret;
            }
            const KeyValuePair& operator*() const
            {
                return *_active;
            }
            friend bool operator==(const KvpIterator &l, const KvpIterator &r)
            {
//                if (l._owner != r._owner)
//                    return false;
                if (l._current != r._current)
                    return false;
                if (l._current == l._owner._slots.end())
                    return true;
                return l._active == r._active;
            }
            friend bool operator!=(const KvpIterator &l, const KvpIterator &r)
            {
                return !(l == r);
            }
        };
        
        KvpIterator Start(void) const
        {
            return KvpIterator(*this, false);
        }
        
        KvpIterator End(void) const
        {
            return KvpIterator(*this, true);
        }
        
        Array<KeyType> AllKeys(void) const
        {
            Array<KeyType> result;
            ForEach(*this, [&result](const KeyValuePair &kvp){
                result.Add(kvp.key);
                return true;
            });
            return result;
        }

        Array<ValueType> AllValues(void) const
        {
            Array<ValueType> result;
            ForEach(*this, [&result](const KeyValuePair &kvp){
                result.Add(kvp.value);
                return true;
            });
            return result;
        }
        
        Dictionary()
        {
            for (int i = 0; i < 10; i++)
                _slots.Add(HashEntry());
        }

        Dictionary& operator=(const Dictionary &other)
        {
            Clear();
            ForEach(other, [this](KeyValuePair &pair){
                Set(pair.key, pair.value);
                return true;
            });
            return *this;
        }

        void Set(KeyType key, ValueType value)
        {
            UInt32 index = Hash(key) % _slots.Count();
            _slots[index].Set(key, value);
        }
        void Reset(KeyType key)
        {
            UInt32 index = Hash(key) % _slots.Count();
            _slots[index].Remove(key);
        }
        void Clear(void)
        {
            ForEach(_slots, [](HashEntry &entry){
                entry.Clear();
                return true;
            });
        }

        class Updater
        {
        private:
            Dictionary<KeyType, ValueType> &_owner;
            KeyValuePair &_entry;
        public:
            Updater(Dictionary<KeyType, ValueType> &owner, KeyValuePair &entry)
            :_owner(owner), _entry(entry)
            {
            }
            
            Updater& operator=(ValueType const& rhs)
            {
                _entry.value = rhs;
                return *this;
            }
            
            operator ValueType &()
            {
                return _entry.value;
            }
        };
        
        ValueType const& operator[](KeyType key) const
        {
            return Get(key).value;
        }
        Updater operator[](KeyType key)
        {
            return Updater(*this, Get(key));
        }
    };
    
    template<typename C>
    class SharedPointer
    {
    private:
        class Reference
        {
        private:
            UInt32 _refCount;
            C *_pointer;
        public:
            Reference(C *pointer)
            :_pointer(pointer), _refCount(0)
            {
            }
            void AddRef(void)
            {
                _refCount++;
            }
            void Release(void)
            {
                _refCount--;
                if (!_refCount)
                    delete _pointer;
            }
            C* Pointer(void) const
            {
                return _pointer;
            }
        private:
            ~Reference() {}
        };
        Reference *_reference;
    public:
        SharedPointer(C *newPointer)
        :_reference(new Reference(newPointer))
        {
            _reference->AddRef();
        }
        
        SharedPointer(const SharedPointer<C>& other)
        :_reference(other._reference)
        {
            _reference->AddRef();
        }
        
        ~SharedPointer()
        {
            _reference->Release();
        }
        
        C& operator*()
        {
            return *_reference->Pointer();
        }
        C* operator->()
        {
            return _reference->Pointer();
        }
        const C& operator*() const
        {
            return *_reference->Pointer();
        }
        const C* operator->() const
        {
            return _reference->Pointer();
        }

        SharedPointer<C>& operator=(const SharedPointer<C>& other)
        {
            if (this != &other) {
                Reference *old = _reference;
                _reference = other._reference;
                _reference->AddRef();
                old->Release();
            }
            return *this;
        }
    };
}

#endif // __LIBRARY_H__
