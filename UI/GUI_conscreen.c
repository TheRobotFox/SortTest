#ifdef CONSCREEN
#include "Conscreen/Conscreen_console.h"
#include "Conscreen/Conscreen_screen.h"
#include "GUI_impl.h"
#include "Conscreen/Conscreen.h"
#include <stdint.h>

struct Rect get_screen_dimensions()
{
    Conscreen_point btc = Conscreen_screen_size();
    return (struct Rect){0,0, btc.x, btc.y};
}
void Draw_Rect(struct Rect *rect, struct Color col)
{
    for(uint16_t y=rect->top; y<rect->bottom; y++){
        for(uint16_t x=rect->left; x<rect->right; x++){

            char c = '#';
            if(col.r+col.g+col.b==0)
                c=0;

            Conscreen_pixel p = {
                .character=c,
                .style = CONSCREEN_ANSI_DEFAULT(col.r, col.g, col.b)
            };
            Conscreen_screen_set(x,y, p);
        }

    }
}
//void Draw_Text(int x, int y, const char *text, size_t len, int size, struct Color col);
void Draw_end(){
    Conscreen_screen_flush();
}
void Draw_begin(){
    Conscreen_screen_begin();
    /* Conscreen_point size = Conscreen_screen_size(); */
    /* printf("Screen size %d %d\n", size.x, size.y); */
}
int GUI_impl_create()
{
    Conscreen_init();
    current_state.conf->active=1;
    return 0;
}
int GUI_impl_destroy()
{
    Conscreen_deinit();
    current_state.conf->active=0;
    return 0;
}
void GUI_impl_update()
{
    GUI_render();

    // pass input to Sorttest
}
#endif
