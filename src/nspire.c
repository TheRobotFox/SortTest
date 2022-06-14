#include <os.h>
#include "nspire.h"
char k2c[]={
    'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
};
t_key charkeys[] = {
    KEY_NSPIRE_A,
    KEY_NSPIRE_B,
    KEY_NSPIRE_C,
    KEY_NSPIRE_D,
    KEY_NSPIRE_E,
    KEY_NSPIRE_F,
    KEY_NSPIRE_G,
    KEY_NSPIRE_H,
    KEY_NSPIRE_I,
    KEY_NSPIRE_J,
    KEY_NSPIRE_K,
    KEY_NSPIRE_L,
    KEY_NSPIRE_M,
    KEY_NSPIRE_N,
    KEY_NSPIRE_O,
    KEY_NSPIRE_P,
    KEY_NSPIRE_Q,
    KEY_NSPIRE_R,
    KEY_NSPIRE_S,
    KEY_NSPIRE_T,
    KEY_NSPIRE_U,
    KEY_NSPIRE_V,
    KEY_NSPIRE_W,
    KEY_NSPIRE_X,
    KEY_NSPIRE_Y,
    KEY_NSPIRE_Z
};

t_key visctrl[] = {
    KEY_NSPIRE_ESC,
    KEY_NSPIRE_MINUS,
    KEY_NSPIRE_PLUS,
    KEY_NSPIRE_P
};

t_key cmdctrl[] = {
    KEY_NSPIRE_ESC,
    KEY_NSPIRE_UP,
    KEY_NSPIRE_DOWN,
    KEY_NSPIRE_DEL,
    KEY_NSPIRE_ENTER,
    KEY_NSPIRE_RET
};

#define ARR_SIZE(arr) (sizeof(arr)/sizeof(*arr))
#define getkey(arr) _getkey(arr,ARR_SIZE(arr))
size_t static _getkey(t_key *map, size_t size)
{
    for(int i=0; i<size; i++)
    {
        if(isKeyPressed(map[i]))
            return i;
    }
    return -1;
}

// Test for all keys used in cmd context; block til found
int cmdkey()
{
    size_t i;

    wait_no_key_pressed();
    wait_key_pressed();
    if((i=getkey(charkeys))!=-1)
        return i+26*isKeyPressed(KEY_NSPIRE_SHIFT);

    if((i=getkey(cmdctrl))!=-1)
        return i+100;

}

char* CMDBUF[CMDBUFLEN];
short buffer_index=0, latest=0;
char overwrite=0;

int getcmd(){
    char* buf;
    size_t i,k;
    char c;
swap:
    buf = CMDBUF[buffer_index];
    if(overwrite){
        buf[sizeof(NAME)]=0;
        latest=buffer_index;
    }
    i=strlen(buf);
render:
    printf("\r%-20s",buf);
    fflush(stdout);
input:
        k=cmdkey(cmdctrl);
        switch(k)
        {
        case 100:
            return 1;
        case 101:
            buffer_index++;
            if(buffer_index==CMDBUFLEN){
                overwrite=1;
                buffer_index=0;
            }
            goto swap;
            break;
        case 102:
            buffer_index--;
            if(buffer_index<0){
                overwrite=0;
                buffer_index=CMDBUFLEN-1;
            }
            goto swap;
            break;
        case 103:
            if(i>=sizeof(NAME)){
                i--;
                buf[i]=0;
            }
            goto render;
            break;
        case 104:
        case 105:
            goto end;
        default:
            c = k2c[k];
            putchar(c);
            if(i<CMDLINELENGTH){
                buf[i]=c;
                i++;
            }
    }
    goto input;
    end:
    buf[i]=0;
    return  0;
}
