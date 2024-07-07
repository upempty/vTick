
#ifndef MM_PAGING_H
#define MM_PAGING_H


#define PAGE_SIZE (4 * 1024u)
#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
typedef struct {
    unsigned int present:1;
    unsigned int rw:1;
    unsigned int user:1;
    unsigned int writeThrough:1;
    unsigned int cacheDisable:1;
    unsigned int accessed:1;
    unsigned int dirty:1;
    unsigned int _pageSize:1;
    unsigned int _ignore:4;
    unsigned int tableAddress:20;
} __attribute__((packed)) PageDirectoryEntry;

typedef struct {
    unsigned int present:1;
    unsigned int rw:1;
    unsigned int user:1;
    unsigned int writeThrough:1;
    unsigned int cacheDisable:1;
    unsigned int accessed:1;
    unsigned int dirty:1;
    unsigned int _pat:1;
    unsigned int _global:1;
    unsigned int _ignore:3;
    unsigned int pageAddress:20;
} __attribute__((packed)) PageTableEntry;

typedef struct {
    PageDirectoryEntry entries[1024];
} PageDirectory;

typedef struct {
    PageTableEntry entries[1024];
} PageTable;

PageDirectory* identityDirectory;

void mm_paging_init(void);


#endif