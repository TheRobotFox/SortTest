#ifdef unix
#include "GUI_impl.h"

#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysymdef.h>

static pthread_t Thread;
static Display *d;
static Window w;
static XEvent e;
static int s;
static XWindowAttributes attr;
static Pixmap buffer;
static GC gc;
static struct Rect rect;

#define RGB(r, g, b) (unsigned long)(r<<16 | g<<8 | b)

void Sleep_while_active(size_t ms)
{
    size_t iter = ms/SLEEP_RES;
    for(int i=0; current_state.conf->active && i<iter; i++){
        Sleep(SLEEP_RES);
    }
}

// IO functions

struct Rect get_screen_dimensions()
{
    return rect;
}

void Draw_begin()
{
    if(current_state.conf->active){
      // get current window size
      XGetWindowAttributes(d,w,&attr);
      rect.right=attr.width;
      rect.bottom=attr.height;
      buffer = XCreatePixmap(d,w,rect.right,rect.bottom, 24);

      Draw_Rect(&rect, (struct Color){0,0,0});
      XSetForeground(d, gc, 0);
    }
}

void Draw_Rect(struct Rect *rect, struct Color col)
{
    if(buffer){
    XGCValues v;
    v.foreground=RGB(col.r, col.g, col.b);
    GC color = XCreateGC(d,buffer,GCForeground,&v);
    XFillRectangle(d,buffer,color,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top);
    XFreeGC(d,color);
    }
}

/*void Draw_Text(int x, int y, const char *text, size_t len, int size, struct Color col)
{
    XGCValues v;
    v.foreground=RGB(col.r, col.g, col.b);
    GC color = XCreateGC(d,buffer,GCForeground,&v);
    XDrawString(d, buffer, color, x, y, text, len);
    //printf("Text : %s | %d at %d,%d\n", text, len, x, y);
    XFreeGC(d,color);
}*/

void Draw_end()
{
    XCopyArea(d, buffer, w, gc, 0,0, rect.right, rect.bottom, 0, 0);
    XFreePixmap(d, buffer);
    buffer=0;

}

static void* GUI_thread_proc(void *unused){

	d=XOpenDisplay(NULL);
	s=DefaultScreen(d);
	w=XCreateSimpleWindow(d, RootWindow(d,s), 10,10,100,100,1, BlackPixel(d,s), BlackPixel(d,s));
	XSelectInput(d,w, KeyPressMask | ExposureMask);
    gc = XCreateGC(d,w,0, NULL);

	XMapWindow(d,w);

  current_state.conf->active=1;

	while(current_state.conf->active){
	//	while(XNextEvent(d, &e)){
		//	if(e.type == KeyPress){
		//		switch(XkbKeycodeToKeysym(d,e.xkey.keycode,0,0)){
		//		}

		//	}
		//if(e.type == Expose)
		//	render();
		//}
		GUI_render();
		Sleep(30);
	}
	return NULL;
}

int GUI_impl_create()
{
    pthread_create(&Thread, NULL, GUI_thread_proc, NULL);
    return 0;
}

int GUI_impl_destroy()
{
    pthread_join(Thread, NULL);
    XDestroyWindow(d, w);
    XFreeGC(d, gc);
    XCloseDisplay(d);

    return 0;
}

void GUI_impl_update()
{
    //GUI_render();
    //XEvent e;
    //XSendEvent(d,w,0,ExposureMask,&e);
}

#endif
