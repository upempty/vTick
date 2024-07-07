#ifndef TTY_H
#define TTY_H

typedef struct 
{
    char ch;
    unsigned char attr;
} __attribute__((packed)) vga_cell_t;

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

#define WHITE_ON_BLACK 0x0f 
#define RED_ON_WHITE 0xf4   
#define GREEN_ON_BLACK 0x02 

#define TTY_REG_CTRL 0x3d4
#define TTY_REG_DATA 0x3d5

void kclear_tty();
void kprint_at(char *text, int row, int col);
void kprint(char *text);
void kprint_backspace();

int task_one(void);
int task_two(void);

#endif