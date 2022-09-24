

#ifdef unix

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

#define RGB(r, g, b) (unsigned long)(r<<16 | g<<8 | b)

void Sleep_while_active(size_t ms)
{
    size_t iter = ms/SLEEP_RES;
    for(int i=0; current_state.conf->active && i<iter; i++){
        Sleep(SLEEP_RES);
    }
}

// IO functions

static struct Rect get_screen_dimensions()
{
    struct Rect rect = {0};
    XGetWindowAttributes(d,w,&attr);
   	rect.right=attr.width;
   	rect.bottom=attr.height;
    return rect;
}

static void Draw_begin()
{
    struct Rect rect = get_screen_dimensions();

}

static void Draw_Rect(struct Rect *rect, struct Color col)
{
	XGCValues v;
	v.foreground=RGB(col.r, col.g, col.b);
	GC color = XCreateGC(d,w,GCForeground,&v);
   	XFillRectangle(d,w,color,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top);
    XFreeGC(d,color);
}

static void Draw_Text(int x, int y, const char *text, size_t len, int size, struct Color col)
{
    XGCValues v;
    v.foreground=RGB(col.r, col.g, col.b);
    GC color = XCreateGC(d,w,GCForeground,&v);
    XDrawString(d, w, color, x, y, text, len);
    //printf("Text : %s | %d at %d,%d\n", text, len, x, y);
    XFreeGC(d,color);
}

static void Draw_end()
{
    // TODO: Implement double Buffering
}

static void* GUI_thread_proc(void *unused){

	d=XOpenDisplay(NULL);
	s=DefaultScreen(d);
	w=XCreateSimpleWindow(d, RootWindow(d,s), 10,10,100,100,1, BlackPixel(d,s), BlackPixel(d,s));
	XSelectInput(d,w, KeyPressMask | ExposureMask);

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
}

int GUI_impl_destroy()
{
    pthread_join(Thread, NULL);
}

void GUI_impl_update()
{
    //GUI_render();
    //XEvent e;
    //XSendEvent(d,w,0,ExposureMask,&e);
}

#endif
