#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#include "Runtime.h"
#include "Function.h"

namespace Library {
    
    template<class T> UInt64 Hash(T value) { return UInt64(value); }
    
    template<class C> void ForEach(const C &iterable, Library::Function<bool(decltype(*iterable.Start())&)> handler)
    {
        for (auto it = iterable.Start(); it != iterable.End(); it++) {
            if (!handler(*it))
                break;
        }
    }
    
    template<class ObjType> class Array
    {
    public:
        typedef ObjType* Iterator;
        
        Array()
        {
            _count = 0;
            _max = 8;
            _index = new char[sizeof(ObjType) * _max];
        }
        Array(const Array &other)
        {
            _count = other._count;
            _max = other._max;
            _index = new char[sizeof(ObjType) * _max];
            for (UInt32 i = 0; i < _count; i++)
                new (_index + (sizeof(ObjType) * i)) ObjType(reinterpret_cast<ObjType*>(other._index)[i]);
        }
        ~Array()
        {
            for (UInt32 i = 0; i < _count; i++)
                reinterpret_cast<ObjType*>(_index)[i].~ObjType();
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
        
        Iterator Start(void) const
        {
            return reinterpret_cast<ObjType*>(_index);
        }
        Iterator End(void) const
        {
            return reinterpret_cast<ObjType*>(_index) + _count;
        }
                      
        
        ObjType& operator[](UInt32 index)
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
        
        UInt32 Count(void)
        {
            return _count;
        }
        
    private:
        char *_index;
        UInt32 _count, _max;
        
        void CheckSize(void)
        {
            UInt32 newMax = _max;
            if (_count == _max)
                newMax *= 2;
            if (_count < (_max / 2))
                newMax /= 2;
            if (newMax < 8)
                newMax = 8;
            if (newMax != _max) {
                char *newIndex = new char[sizeof(ObjType) * newMax];
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
                _entries.Add(Entry(key, value));
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
            
            typename Array<KeyValuePair>::Iterator Start(void)
            {
                return _entries.Start();
            }

            typename Array<KeyValuePair>::Iterator End(void)
            {
                return _entries.End();
            }

        private:
            Array<KeyValuePair> _entries;
        };

        KeyValuePair& Get(KeyType key)
        {
            UInt32 index = Hash(key) % _slots.Count();
            return _slots[index].Find(key);
        }
        
        friend class KvpIterator;
        Array<HashEntry> _slots;
        
        // TODO: resize slots
        
    public:
        class KvpIterator
        {
        private:
            const Dictionary &_owner;
            typename Array<HashEntry>::Iterator _current;
            typename Array<KeyValuePair>::Iterator _active;
        public:
            KvpIterator(const Dictionary &owner, bool end)
            :_owner(owner), _current(end ? owner._slots.End() : owner._slots.Start())
            {
                if (!end)
                    _active = (*_current).Start();
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
                if (_active == (*_current).End()) {
                    while (_current != _owner._slots.End()) {
                        _current++;
                        if (_current == _owner._slots.End())
                            break;
                        _active = (*_current).Start();
                        if (_active != (*_current).End())
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
            KeyValuePair& operator*()
            {
                return *_active;
            }
            friend bool operator==(const KvpIterator &l, const KvpIterator &r)
            {
//                if (l._owner != r._owner)
//                    return false;
                if (l._current != r._current)
                    return false;
                if (l._current == l._owner._slots.End())
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
            ForEach(*this, [&result](KeyValuePair &kvp){
                result.Add(kvp.key);
                return true;
            });
            return result;
        }

        Array<ValueType> AllValues(void) const
        {
            Array<ValueType> result;
            ForEach(*this, [&result](KeyValuePair &kvp){
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
            
            operator ValueType const&()
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
}

#endif // __LIBRARY_H__
