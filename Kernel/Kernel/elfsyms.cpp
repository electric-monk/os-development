#include "elfsyms.h"
#include "elf.h"
#include "debug.h"

ElfSymbols::ElfSymbols()
{
    _symbols = new KernelArray();
}

ElfSymbols::~ElfSymbols()
{
    _symbols->Release();
}

int ElfSymbols::Parse(ElfProvider *source)
{
    Elf32_Shdr *sh_table = (Elf32_Shdr*)source->GetAddress(source->Header());
    UInt64 max = source->Count();
    UInt64 shstrndx = source->Strndx();
    char *strings = NULL;
    bool done = false;
    bool overdone = false;
    int got = 0;
    for (UInt64 i = 0; i < max; i++) {
        if (i == shstrndx)
            continue;
        switch (sh_table[i].sh_type) {
            case SHT_STRTAB:
                if (strings != NULL)
                    return 0;
                if (sh_table[i].sh_addr == 0)
                    continue;
                strings = (char*)source->GetAddress(sh_table[i].sh_addr);
                break;
            case SHT_SYMTAB:
                if (!done) {
                    if (sh_table[i].sh_addr == 0)
                        continue;
                    Elf32_Sym *syms = (Elf32_Sym*)source->GetAddress(sh_table[i].sh_addr);
                    UInt64 sym_count = sh_table[i].sh_size / sizeof(*syms);
                    for (UInt64 j = 0; j < sym_count; j++) {
                        if ((ELF32_ST_TYPE(syms[j].st_info) == STT_FUNC) && syms[j].st_size && syms[j].st_name) {
                            Symbol *newSym = new Symbol(syms[j].st_value, syms[j].st_size, syms[j].st_name);
                            _symbols->Add(newSym);
                            newSym->Release();
                            got++;
                        }
                    }
                    done = true;
                } else {
                    overdone = true;
                }
                break;
        }
    }
    if (overdone) {
        for (UInt32 i = 0; i < _symbols->Count();) {
            Symbol *sym = (Symbol*)_symbols->ObjectAt(i);
            if (!sym->Name()) {
                _symbols->Remove(sym);
                got--;
            } else {
                i++;
            }
        }
    } else {
        _symbols->Enumerate([strings](KernelObject *symbol){
            ((Symbol*)symbol)->LoadName(strings);
            return (KernelObject*)NULL;
        });
    }
    return got;
}

ElfSymbols::Symbol* ElfSymbols::Find(void *address)
{
    UInt64 intAddress = UInt64(address);
    return (Symbol*)_symbols->Enumerate([intAddress](KernelObject *_symbol){
        Symbol *symbol = (Symbol*)_symbol;
        UInt64 base = UInt64(symbol->Base());
        if ((base <= intAddress) && (intAddress < (base + symbol->Length())))
            return symbol;
        return (Symbol*)NULL;
    });
}

ElfSymbols::Symbol::Symbol(UInt64 base, UInt64 len, UInt64 nameIndex)
:_base(base), _len(len), _nameIndex(nameIndex), _name(NULL)
{
}

void ElfSymbols::Symbol::LoadName(const char *strings)
{
    if (_name)
        return;
    _name = new KernelString(strings + _nameIndex);
}

ElfSymbols::Symbol::~Symbol()
{
    if (_name)
        _name->Release();
}

UInt64 ElfSymbols::Symbol::Offset(void *addr)
{
    UInt32 result = UInt32(addr) - UInt32(_base);
    if (result > _len)
        result = 0;
    return result;
}
