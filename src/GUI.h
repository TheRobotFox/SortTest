#pragma once

#ifdef NSPIRE
#include <os.h>
#include <nspireio/nspireio.h>
#define PIXELS (320*240)
#define Sleep(ms) msleep(ms)
nio_console console;
nio_console* ptr_console;
#define printf(str,...) nio_fprintf(ptr_console, str, ##__VA_ARGS__)
#endif
#if defined(_WIN32)
#include <windows.h>
#else
typedef struct{
    long top,bottom,right,left;
} RECT;
#endif
#ifdef __unix__
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysymdef.h>
#include <unistd.h>
#define Sleep(ms) usleep(ms*1000) 
#endif

#include "List.h"
#include "Sort.h"

#define SKIP 1
#define MERGEDIS 10

typedef struct {
    float delay;
    #ifdef _WIN32
    HWND hwnd;
    HINSTANCE hinstance;
    #endif
    LIST* List;
    SortAlg *Alg;
    int max;
    int comp1;
    int comp2;
    int skip;
    int mergedistance;
    char active;
    char activate;
    char pause;
	char rendering;
	char no_render;
} infoGUI;

#ifdef _WIN32
DWORD WINAPI create_gui(LPVOID info);
#else
void *create_gui(void* info);
#endif

void gui_updateList(LIST* List);

void gui_pointer1(int p);

void gui_pointer2(int p);

void gui_update();

void gui_wait();
