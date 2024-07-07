#include "../include/mm_heap.h"
#include "../include/tty.h"
#include "../include/task.h"
#include "../include/dtypes.h"
#include "../libc/mem.h"


struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init(){
    
    int total_table_entries = HEAP_SIZE_BYTES / HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*) (HEAP_TABLE_ADDRESS);
    kernel_heap_table.total = total_table_entries; 

    void* end = (void*)(HEAP_ADDRESS + HEAP_SIZE_BYTES);
    int response = heap_create(&kernel_heap, (void*)(HEAP_ADDRESS), end, &kernel_heap_table);
    if (response < 0) kprint("Failed to create heap!");


}

void* kmalloc(size_t size){
    return heap_malloc(&kernel_heap, size);
}

void* kzalloc(size_t size){
    void* ptr = kmalloc(size);
    if (!ptr)
        return 0;

    memset(ptr, 0x00, size);
    return ptr;
}

void kfree(void* ptr){
    heap_free(&kernel_heap, ptr);
}

static int heap_validate_table(void* ptr, void* end, struct heap_table* table){
    int res = 0;

    size_t table_size = (size_t)(end - ptr);
    size_t total_blocks = table_size / HEAP_BLOCK_SIZE;
    if (table->total != total_blocks){
        res = -EINVARG;
        goto out;
    }

out:
    return res;
}
static bool heap_validate_alignment(void* ptr){
    return ((unsigned int)ptr % HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table* table){
    int res = 0;

    if (!heap_validate_alignment(ptr) || !heap_validate_alignment(end)){
        res = -EINVARG;
        goto out;
    }

    memset((uint8_t*) heap, 0, sizeof(struct heap));
    heap->saddr = ptr; // set start address to provided pointer
    heap->table = table;

    res = heap_validate_table(ptr, end, table);
    if (res < 0){
        goto out;
    }

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total; // freeing heap space
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

out:
    return res;
}

static uint32_t heap_align_value_to_upper(uint32_t val){
    if ((val % HEAP_BLOCK_SIZE) == 0){
        return val;
    }
    return (val - (val % HEAP_BLOCK_SIZE)) + HEAP_BLOCK_SIZE;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry){
    return entry & 0x0f;
}

int heap_get_start_block(struct heap* heap, uint32_t total_blocks){
    struct heap_table* table = heap->table;
    unsigned int bc = 0;
    int bs = -1;

    for (size_t i = 0; i < table->total; i++){
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE){
            bc = 0;
            bs = -1;
            continue;
        }
        if (bs == -1){ // if this is the first block
            bs = i;
        }
        bc++;
        if (bc == total_blocks){
            break;
        }
    }
    if (bs == -1){
        return -ENOMEM;
    }
    return bs;
}

void* heap_block_to_address(struct heap* heap, int block){
    return heap->saddr + (block * HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_taken(struct heap* heap, int start_block, int total_blocks){
    int end_block = start_block + total_blocks - 1;

    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if (total_blocks > 1){
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    for (int i = start_block; i <= end_block; i++){
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        // if (i != end_block - 1){
        if (i != end_block){
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

void* heap_malloc_blocks(struct heap* heap, uint32_t total_blocks){
    void* address = 0;

    int start_block = heap_get_start_block(heap, total_blocks);
    if (start_block < 0){
        goto out;
    }

    address = heap_block_to_address(heap, start_block);

    // Mark the blocks as taken
    heap_mark_blocks_taken(heap, start_block, total_blocks);

out:
    return address;
}

void heap_mark_blocks_free(struct heap* heap, int starting_block){
    struct heap_table* table = heap->table;
    for (int i = starting_block; i < (int)table->total; i++){
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
        if (!(entry & HEAP_BLOCK_HAS_NEXT)){
            break;
        }
    }
}

int heap_address_to_block(struct heap* heap, void* address){
    return ((int)(address - heap->saddr)) / HEAP_BLOCK_SIZE;
}

void* heap_malloc(struct heap* heap, size_t size){
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / HEAP_BLOCK_SIZE; // calculate number of blocks
    return heap_malloc_blocks(heap, total_blocks);
}

void heap_free(struct heap* heap, void* ptr){
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}