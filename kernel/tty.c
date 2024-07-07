#include "../include/ports.h"
#include "../include/tty.h"
#include "../libc/mem.h"


int get_cursor_offset(); //gets the current position of the cursor in vga memory
void set_cursor_offset(int offset);

int get_offset(int row, int col);
int get_offset_row(int offset); 
int get_offset_col(int offset);

int print_char(char c, int row, int col, char attr); //attr for the color

int scroll_handling(int offset);

void kprint_at(char *text, int row, int col){
    int offset;
    if(row >= 0 && col >=0){
        offset = get_offset(row,col);
    } else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
    
    int i = 0;
    while(text[i]){
        offset = print_char(text[i++],row,col,WHITE_ON_BLACK);
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

void kprint(char *text){
    kprint_at(text,-1,-1);
}

void kclear_tty(){
    int vidCellsSize = MAX_COLS * MAX_ROWS;
    int i;
    vga_cell_t *vga_mem = (vga_cell_t *)VIDEO_ADDRESS;
    for(i=0;i<vidCellsSize;i++){
        vga_mem += i;
        vga_mem->ch = ' ';
        vga_mem->attr = WHITE_ON_BLACK;
    }

    set_cursor_offset(get_offset(0,0));
}

void kprint_backspace(){
    int last_offset = get_cursor_offset() - 2;
    int row = get_offset_row(last_offset);
    int col = get_offset_col(last_offset);
    print_char(0x08, row, col, WHITE_ON_BLACK);
}


int print_char(char c, int row, int col, char attr){
    vga_cell_t *vga_mem = (vga_cell_t *)VIDEO_ADDRESS;
    if(!attr){
        attr = WHITE_ON_BLACK;
    }

    if(row >= MAX_ROWS || col >= MAX_COLS){
        // error for now
        vga_mem += MAX_COLS * MAX_ROWS - 1;
        vga_mem->ch = 'E';
        vga_mem->attr = RED_ON_WHITE;

        return get_offset(row,col); //maybe exit with 1?
    }

    int offset;
    if(row >= 0 && col >= 0){
        offset = get_offset(row,col);
    } else {
        offset = get_cursor_offset();
    }

    if(c == '\n'){
        row = get_offset_row(offset);
        offset = get_offset(row+1,0);
    } else if(c == 0x08){
        vga_mem += offset;
        vga_mem->ch = ' ';
        vga_mem->attr = attr;
    }else {
        vga_mem += offset;
        vga_mem->ch = c;
        vga_mem->attr = attr;
        offset += 1;


    }
    //offset is incremented before proceeding,  this is the offset for the next operation
    offset = scroll_handling(offset);

    set_cursor_offset(offset);
    return offset;
}

int scroll_handling(int offset){
    vga_cell_t *vga_mem = (vga_cell_t *)VIDEO_ADDRESS;
    if(offset >= MAX_COLS * MAX_ROWS){
        int i;
        for (i = 0; i < MAX_ROWS; i++){
            mem_copy((char *)(vga_mem + get_offset(i,0)), 
                (char *)(vga_mem + get_offset(i-1,0)), 2 * MAX_COLS);
        }
        //empty last line
        vga_cell_t * last = (vga_cell_t *)(vga_mem + get_offset(MAX_ROWS-1,0));
        for(i = 0;i < MAX_COLS;i++){
            last += i;
            last->ch = ' ';
        }
        
        offset -= MAX_COLS;
    }

    return offset;
}

int get_cursor_offset(){
    port_byte_out(TTY_REG_CTRL,14);
    int offset = port_byte_in(TTY_REG_DATA) << 8;
    port_byte_out(TTY_REG_CTRL,15);
    offset += port_byte_in(TTY_REG_DATA);
    return offset;
}

void set_cursor_offset(int offset){
    port_byte_out(TTY_REG_CTRL,14);
    port_byte_out(TTY_REG_DATA,(unsigned char)(offset >> 8)); 
    port_byte_out(TTY_REG_CTRL,15);
    port_byte_out(TTY_REG_DATA,(unsigned char)(offset & 0xff));
}

int get_offset(int row, int col){
    return ((row * MAX_COLS) + col);
}
int get_offset_row(int offset){
    return (offset/MAX_COLS);
}
int get_offset_col(int offset){
    return(offset - (get_offset_row(offset) * MAX_COLS));
}


int task_one(void)
{   int i = 0;
    while (1) {
        i++;
        if (i%10000==1) {
            kprint("1-task_one\n");
        }
    }
    return 0;
}

int task_two(void)
{   int i = 0;
    while (1) {
        i++;
        if (i%10000==2) {
            kprint("2-task_two\n");
        }
    }
    return 0;
}

