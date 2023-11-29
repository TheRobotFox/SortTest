#pragma once
#include "GUI.h"

struct GUI_State
{
    struct GUI_conf *conf;
    List windows;
    char paused;
	char rendering;
    int skip_frames;
};
extern struct GUI_State current_state;


enum INPUT{
    NOTHING,
    TOGGLE_PAUSE,
    SPEED_UP,
    SPEED_DOWN
};

void handleInput(enum INPUT e);


// Port to System instructions:
// 1. implement following OS specific I/O functions
// 2. pass input to handleInput

// Wait as long as the gui is active
void Sleep_while_active(size_t ms);

// Following Functions are optionally replaced with Conscreen for in Terminal Display
struct Rect get_screen_dimensions();
void Draw_Rect(struct Rect *rect, struct Color col);
//void Draw_Text(int x, int y, const char *text, size_t len, int size, struct Color col);
void Draw_end();
void Draw_begin();
int GUI_impl_create();
int GUI_impl_destroy();
void GUI_impl_update();
