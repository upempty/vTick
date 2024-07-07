
#define SECT_SHIFT		9
#define SECT_SIZE		(1 << SECT_SHIFT)

/* bootloader contains 4KB in disk */
#define KERNEL_START_SECT	8

/* 16M */
#define KERNEL_START	0x01000000
#define KERNEL_SECTS	508

static inline unsigned char inb(unsigned short port)
{
  unsigned char  data;
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
  return data;
}

static inline void outb(unsigned short port, unsigned char data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void insl(int port, void *addr, int cnt)
{
  asm volatile("cld; rep insl" :
               "=D" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "memory", "cc");
}

static void disk_wait(void)
{
	/* wait for disk ready */
    while ((inb(0x1F7) & 0xC0) != 0x40) continue;
}

static void disk_readsect(void *dst, int lba)
{
	/* wait for disk to be ready */
	disk_wait();
	/* 28 bit LBA PIO mode read on the Primary bus */
	outb(0x1F2, 1);		/* read sector count = 1 */
	outb(0x1F3, lba & 0xff);
	outb(0x1F4, (lba >> 8) & 0xff);
	outb(0x1F5, (lba >> 16) & 0xff);
	outb(0x1F6, ((lba >> 24) & 0xf) | 0xe0);	/* 0xe0 for master */
	outb(0x1F7, 0x20); // Set ready register

	/* wait for disk to be ready */
	disk_wait();
	/* read sector to memory */
    insl (0x1F0, dst, SECT_SIZE / sizeof(int));

}

static void load_kernel(void)
{
	void *kern_addr = (void *)KERNEL_START;
	int i = KERNEL_START_SECT;
	int total = KERNEL_SECTS;

	while (i < KERNEL_START_SECT + total) {
		disk_readsect(kern_addr, i);
		kern_addr += SECT_SIZE;
		i++;
	}
}

void boot_main(void)
{
	/* load kernel from disk to memory */
	load_kernel();

	/* jmp to kernel */
	((void (*)(void))KERNEL_START)();
}
