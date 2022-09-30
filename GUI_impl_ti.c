#ifdef NSPIRE

#include "GUI_impl.h"

#define SCREEN_X 320
#define SCREEN_Y 240

#define RGB(r, g, b) (short)( (r/8)<<11 | (g/4)<<5 | (b/8) )
static short screenbuff[SCREEN_X*SCREEN_Y];

void Sleep(size_t ms)
{
                // see http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0271d/CHDFDDCF.html
                volatile unsigned *load = (unsigned*)0x900D0000;
                volatile unsigned *control = (unsigned*)0x900D0008;
                volatile unsigned *int_clear = (unsigned*)0x900D000C;
                volatile unsigned *int_status = (unsigned*)0x900D0010;
                unsigned orig_control = *control;
                unsigned orig_load = *load;
                *control = 0; // disable timer
                *int_clear = 1; // clear interrupt status
                *load = 32 * ms;
                *control = 0b01100011; // disabled, TimerMode N/A, int, no prescale, 32-bit, One Shot -> 32khz
                *control = 0b11100011; // enable timer

                // Can't use idle() here as it acks the timer interrupt
                volatile unsigned *intmask = IO(0xDC000008, 0xDC000010);
                unsigned orig_mask = intmask[0];
                intmask[1] = ~(1 << 19); // Disable all IRQs except timer

                while ((*int_status & 1) == 0 && current_state.conf->active && !(isKeyPressed(KEY_NSPIRE_ESC)))
                        __asm volatile("mcr p15, 0, %0, c7, c0, 4" : : "r"(0) ); // Wait for an interrupt to occur

                intmask[1] = 0xFFFFFFFF; // Disable all IRQs
                intmask[0] = orig_mask; // renable IRQs

                *control = 0; // disable timer
                *int_clear = 1; // clear interrupt status
                *control = orig_control & 0b01111111; // timer still disabled
                *load = orig_load;
                *control = orig_control; // enable timer
                if(isKeyPressed(KEY_NSPIRE_ESC))
                    current_state.conf->active=0;
}


void Screen_clear(void* buff){
    memset(buff,0,SCREEN_X*SCREEN_Y*sizeof(short));
}

void Screen_set(short* buff){
    memcpy(REAL_SCREEN_BASE_ADDRESS,buff,sizeof(short)*SCREEN_X*SCREEN_Y);
}


// IO functions
struct Rect get_screen_dimensions()
{
    return (struct Rect){0, 0, SCREEN_X, SCREEN_Y};
}

void Draw_begin()
{
    Screen_clear(screenbuff);
}

void Draw_Rect(struct Rect *rect, struct Color col)
{
    for(int y=rect->top; y<rect->bottom; y++)
    {
        for(int x=rect->left; x<rect->right; x++)
        {
            screenbuff[(y*SCREEN_X)+x]=RGB(col.r, col.g, col.b);
        }
    }
}

void Draw_end()
{
    Screen_set(screenbuff);
}

int GUI_impl_create()
{
    lcd_init(SCR_320x240_16);

    current_state.conf->active=1;

	return 0;
}

int GUI_impl_destroy(){}
void GUI_impl_update()
{
    GUI_render();
}

#endif
