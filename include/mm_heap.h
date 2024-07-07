#ifndef MM_HEAP_H
#define MM_HEAP_H

#define NORMAL   0
#define EIO      1   // IO error
#define EINVARG  2   // invalid argument
#define ENOMEM   3   // no more memory
#define EBADPATH 4   // path error

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef          char  int8_t;
typedef          short int16_t;
typedef          int   int32_t;

typedef uint32_t size_t;
typedef uint8_t bool;
#define false 0
#define true  1

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST 0b01000000


#define HEAP_ADDRESS 0x02000000
#define HEAP_SIZE_BYTES 50331648 //48M(0x03000000)
#define HEAP_TABLE_ADDRESS 0x00100000
#define HEAP_BLOCK_SIZE 4096


typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table{
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t total;
};

struct heap{
    struct heap_table* table;
    void* saddr;
};

int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table* table);
void* heap_malloc(struct heap* heap, size_t size);
void heap_free(struct heap* heap, void* ptr);


void kheap_init();
void* kmalloc(size_t size);
void kfree(void* ptr);
void* kzalloc(size_t size);

#endif