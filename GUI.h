#pragma once
#include "SortList.h"
#include "Sort.h"

#define DBGP //printf("DBG AT:%d\n", __LINE__); getchar(); getchar()

#if NSPIRE
#include <os.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <libndls.h>

#define Sleep_while_active Sleep

#elif defined(__unix__)

#include <unistd.h>

void Sleep(size_t ms);

#elif defined(_WIN32)
#include <windows.h>
#endif

void Sleep_while_active(size_t ms);

struct GUI_conf
{
    float delay;
    int mergedistance;
    int skip_pixels;
    short mapping;
    SortAlg *Alg;
    int activate;
    int active;
    int do_render;
};

struct Color
{
    unsigned char r,g,b;
};

struct Rect
{
    unsigned int left,top,right,bottom;
};

typedef long GUI_Window_id;


int GUI_create(struct GUI_conf *conf);

void GUI_destroy();

GUI_Window_id GUI_windows_append();

void GUI_windows_remove(GUI_Window_id id);

void GUI_Window_set_list(GUI_Window_id id, List l);

void GUI_Window_marks_add(GUI_Window_id id, size_t index, struct Color col);

void GUI_Window_marks_remove(GUI_Window_id id, size_t index);

void GUI_Window_marks_clear(GUI_Window_id id);

void GUI_Window_foreground_set(GUI_Window_id id, struct Color col);

void GUI_Window_background_set(GUI_Window_id id, struct Color col);

void GUI_Window_title_set(GUI_Window_id id, const char *title);

const char* GUI_Window_title_get(GUI_Window_id id);
void GUI_Window_do_render(GUI_Window_id id, int do_render);

void GUI_update(int force);

void GUI_render();

void GUI_wait();
