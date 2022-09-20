#include "GUI.h"

struct GUI_State
{
    struct GUI_conf *conf;
    List windows;
    char paused;
	char rendering;
    int skip_frames;

    #ifdef _WIN32
    HWND hwnd;
    HINSTANCE hinstance;
    #endif
};

struct GUI_Mark
{
    size_t index;
    struct Color col;
};

struct GUI_Window
{
    GUI_Window_id id;
    List l;
    List marks;
    struct Rect rect;
    float opacity;
    struct Color foreground;
    struct Color background;

    int do_render;
    int rendering;
};

static struct GUI_State current_state = {0};
static char buff[256]="SortTest: ";

// Port to System instructions:
// 1. implement following OS specific I/O functions
// 2. pass input to handleInput
static struct Rect get_screen_dimensions();
static void Draw_Rect(struct Rect *rect, struct Color col);
static void Draw_Text(int x, int y, const char *text, size_t len, struct Color col);
static void Draw_end();
static void Draw_begin();
void Sleep_while_active(size_t ms);

#ifdef NSPIRE

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


static void Screen_clear(void* buff){
    memset(buff,0,SCREEN_X*SCREEN_Y*sizeof(short));
}

static void Screen_set(short* buff){
    memcpy(REAL_SCREEN_BASE_ADDRESS,buff,sizeof(short)*SCREEN_X*SCREEN_Y);
}


// IO functions
static struct Rect get_screen_dimensions()
{
    return (struct Rect){0, 0, SCREEN_X, SCREEN_Y};
}

static void Draw_begin()
{
    Screen_clear(screenbuff);
}

static void Draw_Rect(struct Rect *rect, struct Color col)
{
    for(int y=rect->top; y<rect->bottom; y++)
    {
        for(int x=rect->left; x<rect->right; x++)
        {
            screenbuff[(y*SCREEN_X)+x]=RGB(col.r, col.g, col.b);
        }
    }
}

static void Draw_Text(int x, int y, const char *text, size_t len, struct Color col)
{
    // implement
}

static void Draw_end()
{
    Screen_set(screenbuff);
}

#elif defined(_WIN32)

static HANDLE Thread;
static HBITMAP bmp;
static HDC wndhdc, hdc;
static RECT screen_rect;

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
    RECT rc;
    GetClientRect(current_state.hwnd, &rc);
    return (struct Rect){0, 0, rc.right, rc.bottom};
}

static void Draw_begin()
{
    GetClientRect(current_state.hwnd, &screen_rect);
    bmp = CreateCompatibleBitmap(wndhdc,screen_rect.right,screen_rect.bottom);
    SelectObject(hdc,bmp);

    // Clear Bitmap
    FillRect(hdc,&screen_rect,GetStockObject(BLACK_BRUSH));
}

static void Draw_Rect(struct Rect *rect, struct Color col)
{
    RECT rc = {rect->right, rect->top, rect->left, rect->bottom};
    HBRUSH brush = CreateSolidBrush(RGB(col.r,col.g,col.b));
    FillRect(hdc,&rc,brush);
    DeleteObject(brush);
}

static void Draw_Text(int x, int y, const char *text, size_t len, struct Color col)
{
    SetTextColor(hdc, RGB(col.r, col.g, col.b));
    RECT rc = {x,y,screen_rect.right,screen_rect.bottom};
    DrawTextA(hdc, text, len, &rc,0);
}

static void Draw_end()
{
    BitBlt(wndhdc,0,0,screen_rect.right,screen_rect.bottom,hdc,0,0,SRCCOPY);
    DeleteObject(bmp);
    ValidateRect(current_state.hwnd,&screen_rect);
}


#elif defined(__unix__)

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
    Draw_Rect(&rect, (struct Color){0,0,0});
}

static void Draw_Rect(struct Rect *rect, struct Color col)
{
	XGCValues v;
	v.foreground=RGB(col.r, col.g, col.b);
	GC color = XCreateGC(d,w,GCForeground,&v);
   	XFillRectangle(d,w,color,0,0,rect->right,rect->bottom);
    XFreeGC(d,color);
}

static void Draw_Text(int x, int y, const char *text, size_t len, struct Color col)
{
	XGCValues v;
	v.foreground=RGB(col.r, col.g, col.b);
	GC color = XCreateGC(d,w,GCForeground,&v);
    XDrawString(d, w, color, x, y, text, len);
    XFreeGC(d,color);
}

static void Draw_end()
{
    // TODO: Implement double Buffering
}

#endif


enum INPUT{
    NOTHING,
    TOGGLE_PAUSE,
    SPEED_UP,
    SPEED_DOWN
};


#define DELAY_MIN 0.000001
#define DELAY_MAX 100000

static void handleInput(enum INPUT e)
{
    switch(e)
    {
    case TOGGLE_PAUSE:
        current_state.paused=!current_state.paused; break;
    case SPEED_DOWN:
        current_state.conf->delay*=1.3;
        if(current_state.conf->delay>DELAY_MAX)
             current_state.conf->delay=DELAY_MAX;
        break;
    case SPEED_UP:
        current_state.conf->delay/=1.3;
        if(current_state.conf->delay<=DELAY_MIN)
            current_state.conf->delay=DELAY_MIN;
        break;

    default:
        break;
    }
}

static bool f_mark_by_index(void *_a, void *_b)
{
    struct GUI_Mark *a=_a;
    int *b = _b;
    return a->index==*b;
}

static void GUI_Window_render(struct GUI_Window *win){

    struct Rect rect = win->rect,
                line;
    struct Color col;

    Draw_Rect(&rect, win->background);
    //Draw List
    if(win->l){

        int skip = current_state.conf->skip_pixels+1,
            list_size = List_size(win->l);
        S_TYPE *data = List_start(win->l),
               max = get_max(win->l);
        int oldindex = 0,index;

        line.bottom=rect.bottom;

        for(float x=rect.left; x<=rect.right-skip; x+=skip){

            // Calc Rect dimentions
            index=(int)((x-rect.left)/(float)(rect.right-1)*(list_size-1));

            line.left=x;
            line.top=rect.bottom-data[index]/(float)max*(rect.bottom-rect.top)/1.1;

            line.right=x+skip;

            // Render seperation
            if(index!=oldindex && list_size*current_state.conf->mergedistance<rect.right){
                oldindex=index;
                continue;
            }

            // Create Rect and color pointers
            struct GUI_Mark *mark = List_find(win->marks, f_mark_by_index, &index);
            if(mark)
                col=mark->col;
            else
                col=win->foreground;

            Draw_Rect(&line,col);
        }
    }
}

static void GUI_windows_map_square(List l)
{
    struct Rect rect = get_screen_dimensions();

    // window manager
    unsigned int window_count = List_size(l),
        rows = ceil(sqrt(window_count)),
        // TODO: float precition offsets
        y_offset = 0,
        x_offset,
        coll;
    struct GUI_Window *data = List_start(l);

    for(int r=0; r < rows; r++)
    {
        coll = window_count/rows + window_count%rows;
        window_count-=coll;
        //printf("%d -> %d | %d\n", window_count, rows, coll);
        x_offset = 0;
        for(int w=0; w < coll; w++)
        {
            data->rect = (struct Rect){x_offset,y_offset, x_offset+rect.right/coll, y_offset+rect.bottom/rows};
            data++;
            x_offset+=rect.right/coll;
        }
        y_offset+=rect.bottom/rows;
    }
}

static void GUI_windows_mapper()
{
    switch(current_state.conf->mapping)
    {
    default:
        GUI_windows_map_square(current_state.windows);
        return;
    }
}

void GUI_render(){
    if(current_state.conf->do_render){

        current_state.rendering = 1;

        Draw_begin();

        // Map windows to rects
        GUI_windows_mapper();
        // Render windows
        for(struct GUI_Window *start = List_start(current_state.windows),
                              *end = List_end(current_state.windows);
                              start != end; start++)
        {
            if(start->do_render){
                start->rendering = 1;
                GUI_Window_render(start);
                start->rendering = 0;
            }
        }

        // Render Overlay
        if(current_state.conf->Alg){
            struct Rect rect = get_screen_dimensions();
            int length=snprintf(buff,255,"SortTest: %-20s - %.3fs",current_state.conf->Alg->name,current_state.conf->Alg->time/CLOCKS_PER_SEC);
            Draw_Text(0, 32, buff+10, length-10, (struct Color){255,255,255});

#ifdef _WIN32
            SetWindowTextA(current_state.hwnd,buff);
#endif
            // Print Pause
            if(current_state.paused){
                Draw_Text(rect.right/2, 0, "Paused", 6, (struct Color){255,0,0});
            }
        }
        Draw_end();
        current_state.rendering = 0;
    }
}

#if defined(NSPIRE)
static void* GUI_thread_proc(void*){

    lcd_init(SCR_320x240_16);

    current_state.conf->active=1;

	return NULL;
}
#elif  defined(__unix__)

static void* GUI_thread_proc(void* _){

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
#elif defined(_WIN32)
static LRESULT CALLBACK _GUI_tread_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

    static HFONT font;

    switch(msg)
    {
        case WM_CREATE:
        {
            //Init Objects
            wndhdc= GetDC(hwnd);
            hdc = CreateCompatibleDC(wndhdc);
            RECT rc;
            GetClientRect(hwnd,&rc);
            font = CreateFontA(rc.bottom/15,0,0,0,0,0,0,0,DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,0);

            //Configure HDC
            SetBkColor(hdc,RGB(0,0,0));
            SetBkMode(hdc,TRANSPARENT);
            SelectObject(hdc,font);

            // Set UpdateTimer
            SetTimer(hwnd,1,30,0);
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
        {
            //Free

            ReleaseDC(hwnd,hdc);
            ReleaseDC(hwnd,wndhdc);

            KillTimer(hwnd,1);

            PostQuitMessage(0);
        break;
        }
        case WM_ERASEBKGND:
        break;
        case WM_PAINT:
        {
            if(!current_state.rendering){
                // UpdateScreen
                GUI_render();
            }

        break;
        }
        case WM_TIMER:
        {
            InvalidateRgn(hwnd,0,0);
        break;
        }
        case WM_KEYDOWN:
        {
            enum INPUT event=NOTHING;
            switch(wParam)
            {
            case 'P':
                event=TOGGLE_PAUSE; break;

            case VK_OEM_MINUS:
            case VK_SUBTRACT:
                event=SPEED_UP; break;
            case VK_OEM_PLUS:
            case VK_ADD:
                event=SPEED_DOWN; break;
            }
            handleInput(event);

        break;
        }
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;

}

static DWORD WINAPI GUI_thread_proc(LPVOID _){

    // Create Windowclass
    const char* Name="SortTest";
    WNDCLASSEXA wc = {0};
    wc.cbSize=sizeof(wc);
    wc.lpfnWndProc=_GUI_tread_proc;
    wc.lpszClassName=Name;
    wc.lpszMenuName=Name;
    wc.hbrBackground = GetStockObject(0);

    if(RegisterClassExA(&wc)){
        // Create and maintain Window
        current_state.hwnd = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, Name, Name, WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, current_state.hinstance, NULL);

        if(current_state.hwnd){

            current_state.conf->active=1;
            MSG msg;

            while(GetMessage(&msg, NULL, 0, 0) > 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

        }
        UnregisterClassA(Name,current_state.hinstance);
    }
    current_state.conf->active=0;
    current_state.paused=0;

    return 0;

}
#endif

#ifdef NSPIRE

static void getKey(){
    static int pressed=0;

		if(isKeyPressed(KEY_NSPIRE_ESC)){
        current_state.conf->active=0;
        wait_no_key_pressed();
    }

    if(isKeyPressed(KEY_NSPIRE_P)){
        handleInput(TOGGLE_PAUSE);
        wait_no_key_pressed();
    }

    if(!any_key_pressed())
        pressed=0;
    if(pressed)
        return;
		if(isKeyPressed(KEY_NSPIRE_PLUS)){
				pressed=1;
        handleInput(SPEED_DOWN);
    }
		if(isKeyPressed(KEY_NSPIRE_MINUS)){
				pressed=1;
        handleInput(SPEED_UP);
    }

}

#endif


int GUI_create(struct GUI_conf *conf)
{
    current_state.conf=conf;
    if(!conf->activate)
        return 0;

    current_state.windows = List_create(sizeof(struct GUI_Window));
    current_state.rendering=0;

#ifdef _WIN32

    SECURITY_ATTRIBUTES sa = {0};
    current_state.hinstance=GetModuleHandle(0);
    Thread = CreateThread(&sa,0,GUI_thread_proc,NULL,0,0);

#elif defined(__unix__)

    pthread_create(&Thread, NULL, GUI_thread_proc, NULL);

#elif defined(NSPIRE)

    GUI_thread_proc(NULL);

#endif
    return 0;
}

void GUI_destroy()
{
    for(struct GUI_Window *start = List_start(current_state.windows),
                          *end = List_end(current_state.windows);
                          start != end; start++)
        GUI_windows_remove(start->id);

    List_free(current_state.windows);
    current_state.conf->active=0;

    #ifdef _WIN32_

    WaitForSingleObject(Thread,INFINITE);

    #elif defined(__unix__)

    pthread_join(Thread, NULL);

    #endif
}

void GUI_update(int force){
    if(current_state.conf->active){
        static int skiped=0;
        if(skiped>=current_state.skip_frames || force){
            skiped=0;
        #ifdef _WIN32
        //PostMessageA(current_state.hwnd,WM_PAINT,0,0);
		#elif defined(__unix__)
        //GUI_render();
		//XEvent e;
		//XSendEvent(d,w,0,ExposureMask,&e);
        #else
            GUI_render();
        #endif
        }
        skiped++;
        current_state.skip_frames = 0.07/sqrt(current_state.conf->delay);
    }
}



void GUI_wait(){
    if(current_state.conf->active){
        // Correct time for delay
        #ifdef NSPIRE
        getKey();
        #else
        current_state.conf->Alg->time+=clock()-current_state.conf->Alg->time_start;
        #endif
        if(current_state.paused){
            //current_state.conf->Alg->time+=clock()-INFO->Alg->time_start;
            #ifdef NSPIRE
            while(!isKeyPressed(KEY_NSPIRE_P) && !isKeyPressed(KEY_NSPIRE_ESC))
                ;
            #else
            while(current_state.paused)
                Sleep(5);
            #endif
            //current_state.conf->Alg->time_start=clock();
        }else{

            if(current_state.conf->delay){
                // sub 1ms wait with time smearing
                static float time=0;
                time+=current_state.conf->delay/10;
                if((int)time){
                    Sleep((int)time);
                    time-=(int)time;
                }
            }

        }
    #ifndef NSPIRE
        current_state.conf->Alg->time_start=clock();
    #endif
    }
}

static bool f_win_by_id(void *_a, void *_b)
{
    struct GUI_Window *a=_a;
    GUI_Window_id *b = _b;
    return a->id==*b;
}

GUI_Window_id GUI_windows_append()
{
    if(current_state.conf->active){

        static GUI_Window_id current_id;

        if(!List_size(current_state.windows))
            current_id=0;

        struct GUI_Window *win = List_append(current_state.windows, NULL);

        win->l = List_create(sizeof(S_TYPE));
        win->marks = List_create(sizeof(struct GUI_Mark));
        win->opacity=1.0f;
        win->id=current_id++;
        win->do_render = 1;
        win->rendering = 0;
        win->foreground = (struct Color){255,255,255};
        win->background = (struct Color){0,0,0};

        // Remap all windows
        GUI_windows_mapper();
        //for(struct GUI_Window *start = List_start(current_state.windows),
        //                          *end = List_end(current_state.windows);
        //                          start != end; start++)
        //    {
        //printf("%d -> %d | %d | %d | %d\n", start->id, start->rect.left, start->rect.top, start->rect.right, start->rect.bottom);
        //    }

        return win->id;
    }
    return -1;
}

void GUI_windows_remove(GUI_Window_id id)
{
    if(current_state.conf->active){
        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win){
            current_state.conf->do_render = 0;
            GUI_Window_do_render(id, 0);
            while(win->rendering) Sleep(5);
            List_free(win->marks);
            List_remove(current_state.windows, win-(struct GUI_Window*)List_start(current_state.windows));
            current_state.conf->do_render = 1;
        }
    }
}

unsigned int List_get_max(List l);

void GUI_Window_set_list(GUI_Window_id id, List l)
{
    if(current_state.conf->active){

        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win){

            int tmp = win->do_render;
            win->do_render = 0;
            while(win->rendering) Sleep(5);
            win->l = l;
            List_clear(win->marks);
            win->do_render = tmp;
        }
    }
}


void GUI_Window_marks_add(GUI_Window_id id, size_t index, struct Color col)
{
    if(current_state.conf->active){
        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win){

            struct GUI_Mark *mark = List_find(win->marks, f_mark_by_index, &index);
            if(!mark){
                mark = List_append(win->marks, NULL);
            }

            mark->index = index;
            mark->col = col;
        }
    }
}


void GUI_Window_marks_remove(GUI_Window_id id, size_t index)
{
    if(current_state.conf->active){
        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win){
            struct GUI_Mark *mark = List_find(win->marks, f_mark_by_index, &index);
            if(mark)
                List_remove(win->marks, mark-(struct GUI_Mark*)List_start(win->marks));
        }
    }
}

void GUI_Window_marks_clear(GUI_Window_id id)
{
    if(current_state.conf->active){
        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win)
            List_clear(win->marks);
    }
}

void GUI_Window_foreground_set(GUI_Window_id id, struct Color col)
{
    if(current_state.conf->active){
        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win)
            win->foreground=col;
    }
}

void GUI_Window_background_set(GUI_Window_id id, struct Color col)
{
    if(current_state.conf->active){
        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win)
            win->background=col;
    }
}

void GUI_Window_do_render(GUI_Window_id id, int do_render)
{
    if(current_state.conf->active){
        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win){
            win->do_render=do_render;
            while(win->rendering) Sleep(5);
        }
    }
}

