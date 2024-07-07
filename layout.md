
```   

disk.img: 512k max
   0x500|----------------------------------|---------------------------------|-------------------|
        | Sector 1       |     sector2-8   |   Sector 9-508 (LBA 8-507)      | xxxx not used xxx |
   0x400|----------------------------------|---------------------------------|------------------ |
        | MBR 0.5k       | bootloader 3.5k |kernel  image  250k              | data  258k        |
   ===============================================================================================
``` 

``` 
memory:

kernel_stack = 2 Pages = 4096 x 2 by allocate_pages(2).
kernel_stack_top = kernel_stack+ KERNEL_STACK_SIZE (STACK_PAGES x PGSIZE)
after paging, copy stack top to kernel stack top.
so that after that it uses non-fixed address location.
page table also allocated.

            |----------------------------------| =========kernel vm space later plan
            |----------------------------------| 
            |----------------------------------| 
            |----------------------------------| 
            |----------------------------------| 
            |----------------------------------|  
            |----------------------------------| 
            |----------------------------------| 
            |----------------------------------| 
  0xC1000000|----------------------------------| 
            |----------------------------------| 
            |----------------------------------| 
            |----------------------------------| 
  0xC0000000|----------------------------------| =========kernel vm space later plan
            |                                  |
            |                                  |
            |                                  |
            |                                  |==========memory_end for allocation
            |                                  |
  0x01e00000|----------------------------------| 30M
            |                                  |
            |  14M?                            | ---------{page table, directory, stack, heap}
            |                                  |
  0x0103ea00|----------------------------------| 16.512 M ==memory_begin, kernel _end.
            |                                  | 
            |                                  |  
            | kernel (512 K max)               |----------kernel code 16M above
  0x01000000|----------------------------------| 16M
            |                                  |----------pm mode esp 16M below
            |                                  |
            |                                  |
            | free                             |
  0x00100000|----------------------------------| 1M hardware
            | BIOS (256 kB)                    |
    0xC0000 |----------------------------------| 786K
            | video memory (128 kB)            |
    0xA0000|-----------------------------------|
            | extended BIOS data area (639 kB) |
    0x9fc00|-----------------------------------|
            | free ( kB)                       |
    0x010000|----------------------------------| 64k 16bits real mode range
            | E820 other params ( e820_num)    |
    0x009800|----------------------------------|
            | E820 128 entries : 3k            |
    0x008c00|----------------------------------|
            | 7 sectors stage2 (2-8) = 3.5 kB  |---------Boot stage2
    0x007e00|----------------------------------|---------For boot vbr 
            | loaded boot sector (512 bytes)   |---------Boot stage1
    0x007c00|----------------------------------|---------For boot mbr, used as sp 
            |                                  |
    0x000500|----------------------------------|
            | BIOS data area (256 bytes)       |
    0x000400|----------------------------------|
            | interrupt vector table (1 kB)    |
         0x0====================================


for heap structures:
       16M-32M reserved as kernel code
       0x01000000(16M)-0x02000000(32M)        -- 16M reserved for kernel code
       0x02000000(32M)-0x05000000(80M)        -- 48M heap variable ranges.
       0x00100000(01M)-at least 12K (48M/4k)  -- 1M assumed for heap table entry variable ranges. heap table entries: 
       heap table, heap in data seg inside of kernel code bss.
```

