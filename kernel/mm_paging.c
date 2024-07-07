#include "../include/mm_heap.h"
#include "../include/mm_paging.h"
#include "../include/tty.h"
#include "../libc/strings.h"
//static  PageDirectory* identityDirectory;


// physical address to be used here for directory!!!!
void set_directory(PageDirectory* directory) {
    __asm__ __volatile__("mov %%eax, %%cr3"::"A"(directory):);
}

void setup_directory_entry(PageDirectoryEntry* entry, int present, unsigned int address) {
    entry->present = present;
    entry->tableAddress = address >> 12;
    entry->rw = 1;
    entry->user = 0;
    entry->writeThrough = 0;
    entry->cacheDisable = 0;
    entry->accessed = 0;
    entry->dirty = 0;
    entry->_pageSize = 0;
    entry->_ignore = 0;
}

void setup_table_entry(PageTableEntry* entry, int present, unsigned int address, int user, int rw) {
    entry->present = present;
    entry->pageAddress = address >> 12;
    entry->rw = rw;
    entry->user = user;
    entry->writeThrough = 0;
    entry->cacheDisable = 0;
    entry->accessed = 0;
    entry->dirty = 0;
    entry->_pat = 0;
    entry->_global = 0;
    entry->_ignore = 0;
}

// to do: virtual address per page to mapping, from vm page to ph page, for kernel used pages only.
void mm_paging_init(void) {

    identityDirectory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);

    for (size_t i = 0; i < 1024; i++) {

        PageTable* table = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE); 

        setup_directory_entry(&identityDirectory->entries[i], 1, (unsigned int) table);
        for (size_t j = 0; j < 1024; j++) {
            setup_table_entry(&table->entries[j], 1, (1024 * i + j) * PAGE_SIZE, 0, 1);
            // setup_table_entry(&table->entries[j], 1, (1024 * i + j) * PAGE_SIZE, 0, 1);
        }
    }

    set_directory(identityDirectory);

    unsigned int cr0;
    __asm__ __volatile__("mov %%cr0, %%eax":"=A"(cr0)::);
    cr0 |= 0x80000000;
    __asm__ __volatile__("mov %%eax, %%cr0"::"A"(cr0):);
}