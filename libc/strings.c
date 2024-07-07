#include "strings.h"
#include "mem.h"

void int_to_ascii(int num, char *str){
    int i;
    i = 0;
    if(num < 0){
        num = (-num);
    }
    do{
        str[i++] = num%10 + '0';
    } while((num/=10) > 0);

    if(i < 0){
        str[i++] = '-';
    }
    str[i] = '\0';
    
    reverse(str);

}

void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


void append(char string[], char c){
    int len = strlen(string);
    string[len] = c;
    string[len+1] = '\0';
}

void backspace(char string[]){
    int len = strlen(string);
    string[len-1] = '\0';
}