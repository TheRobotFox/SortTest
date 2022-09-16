#include "GUI.h"

infoGUI tmp = {0};
infoGUI *INFO = &tmp;

#ifdef _WIN32
HBRUSH Black, White, Green, Red, color;
HDC wndhdc,hdc;
char buff[256]="SortTest: ";
#elif defined(NSPIRE)
short Black=0, White=0xFFFF, Green=0x07E0, Red=0xF800, color;
short screenbuff[PIXELS];

void clear(void* buff){
    memset(buff,0,sizeof(short)*PIXELS);
}

void setScreen(short* buff){
    memcpy(REAL_SCREEN_BASE_ADDRESS,buff,sizeof(short)*PIXELS);
}
#elif defined(__unix__)
Display *d;
Window w;
XEvent e;
char buff[256]="SortTest: ";
int s;
GC Green, Red, White, Black, color;
XWindowAttributes attr;
#endif

enum INPUT{
    TOGGLE_PAUSE,
    SPEED_UP,
    SPEED_DOWN
};


#define DELAY_MIN 0.001
#define DELAY_MAX 1000

void handleInput(enum INPUT e)
{
    switch(e)
    {
    case TOGGLE_PAUSE:
        INFO->pause=!INFO->pause; break;
    case SPEED_DOWN:
        INFO->delay*=1.3;
        if(INFO->delay>DELAY_MAX)
             INFO->delay=DELAY_MAX;
        break;
    case SPEED_UP:
        INFO->delay/=1.3;
        if(INFO->delay<=DELAY_MIN)
            INFO->delay=DELAY_MIN;
        break;
    }
}

void render(){
    if(INFO->no_render)
        return;
    RECT line={0};
    RECT rc={0};
    #ifdef _WIN32
    HBITMAP bmp;
    GetClientRect(INFO->hwnd,&rc);
    bmp = CreateCompatibleBitmap(wndhdc,rc.right,rc.bottom);
    SelectObject(hdc,bmp);

    //Clear Screen
    FillRect(hdc,&rc,Black);
    #elif defined(__unix__)
    XGetWindowAttributes(d,w,&attr);
   	rc.right=attr.width;
   	rc.bottom=attr.height;
   	XFillRectangle(d,w,Black,0,0,rc.right,rc.bottom);
    #elif defined(NSPIRE)
    rc.right=320;
    rc.bottom=240;
    clear(screenbuff);
    #endif

    line.bottom=rc.bottom;

    //Draw List
    if(INFO->List){
        int oldindex = 0,index;
		INFO->rendering=1;
        for(int x=0; x<=rc.right-SKIP; x+=SKIP){

            // Calc Rect dimentions
            index=(int)(x/(float)(rc.right-1)*(INFO->List->size-1));

            line.left=x;
            line.top=rc.bottom-INFO->List->data[index]/(float)INFO->max*rc.bottom/1.1;

            line.right=x+SKIP;

            // Render seperation 
            if(index!=oldindex && INFO->List->size*MERGEDIS<rc.right){
                oldindex=index;
                continue;
            }

            color=White;
            // Create Rect and color pointers
            for(int i=0; i<INFO->r_pointers->filled; i++)
            {
                if(INFO->g_pointers->data[i]==index){
                    color=Red;
                    break;
                }
            }
            for(int i=0; i<INFO->g_pointers->filled; i++)
            {
                if(INFO->g_pointers->data[i]==index){
                    color=Green;
                    break;
                }
            }

        	#ifdef NSPIRE
            for(int i=line.top; i<rc.bottom; i++){
                screenbuff[(i*rc.right)+x]=color;
            }
            #elif defined(__unix__)
			XFillRectangle(d,w,color,line.left,line.top,line.right-line.left,line.bottom-line.top);

			#else
            FillRect(hdc,&line,color);
            #endif
        }
    }
	INFO->rendering=0;
    // Update Screen Info
    #ifdef NSPIRE
    setScreen(screenbuff);
	#else
    if(INFO->Alg){
		int length=snprintf(buff,255,"SortTest: %-20s - %.3fs",INFO->Alg->name,INFO->Alg->time/CLOCKS_PER_SEC);
		#if defined(__unix__)
		XDrawString(d,w,White,0,32,buff+10,length-10);
		}
    	#else
        DrawTextA(hdc,buff+10,-1,&rc,0);
        SetWindowTextA(INFO->hwnd,buff);
        // Print Pause
        if(INFO->pause){
            SetTextColor(hdc,RGB(255,0,0));
            DrawTextA(hdc,"Paused",-1,&rc, DT_LEFT | DT_CENTER);
            SetTextColor(hdc,RGB(255,255,255));
        }
    }

    BitBlt(wndhdc,0,0,rc.right,rc.bottom,hdc,0,0,SRCCOPY);
    DeleteObject(bmp);
    ValidateRect(INFO->hwnd,&rc);
    #endif
	#endif
}

#if defined(NSPIRE) || defined(__unix__)
void *create_gui(void* info){
    INFO = info;
    if(INFO->activate)
        INFO->active=1;
	else
		return NULL;
	#ifdef NSPIRE
    lcd_init(SCR_320x240_16);
	#elif defined(__unix__)
	d=XOpenDisplay(NULL);
	s=DefaultScreen(d);
	w=XCreateSimpleWindow(d, RootWindow(d,s), 10,10,100,100,1, BlackPixel(d,s), BlackPixel(d,s));
	XSelectInput(d,w, KeyPressMask | ExposureMask);
	XGCValues v;

	v.foreground=0xFF0000;
	Red=XCreateGC(d,w,GCForeground,&v);
	v.foreground=0x00FF00;
	Green=XCreateGC(d,w,GCForeground,&v);
	v.foreground=0xffffff;
	White=XCreateGC(d,w,GCForeground,&v);

	v.foreground=0x00000;
	Black=XCreateGC(d,w,GCForeground,&v);

	XMapWindow(d,w);
	while(INFO->active){
	//	while(XNextEvent(d, &e)){
		//	if(e.type == KeyPress){
		//		switch(XkbKeycodeToKeysym(d,e.xkey.keycode,0,0)){
		//		}

		//	}
		//if(e.type == Expose)
		//	render();
		//}
		render();
		Sleep(20);
	}
	#endif
	return NULL;
} 
#elif defined(_WIN32)
LRESULT CALLBACK SortGUI(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

    static HFONT font;
    static int drawing=0;

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
            SetTextColor(hdc,RGB(255,255,255));
            SetBkMode(hdc,TRANSPARENT);
            SelectObject(hdc,font);

            //Create Brushes
            Black = CreateSolidBrush(RGB(0,0,0));
            White = CreateSolidBrush(RGB(255,255,255));
            Green = CreateSolidBrush(RGB(0,255,0));
            Red = CreateSolidBrush(RGB(255,0,0));

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
            DeleteObject(Black);
            DeleteObject(White);
            DeleteObject(Green);
            DeleteObject(Red);

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
            if(!drawing){
                drawing=1;
                // UpdateScreen
                render();
                drawing=0;
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
            enum INPUT event;
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

DWORD WINAPI create_gui(LPVOID info){
    INFO = info;
    if(INFO->activate){

        // Create Windowclass
        const char* Name="SortTest";
        WNDCLASSEXA wc = {0};
        wc.cbSize=sizeof(wc);
        wc.lpfnWndProc=SortGUI;
        wc.lpszClassName=Name;
        wc.lpszMenuName=Name;
        wc.hbrBackground = GetStockObject(0);

        if(RegisterClassExA(&wc)){
            // Create and maintain Window
            INFO->hwnd = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, Name, Name, WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, INFO->hinstance, NULL);

            if(INFO->hwnd){

                INFO->active=1;
                MSG msg;

                while(GetMessage(&msg, NULL, 0, 0) > 0)
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

            }
            UnregisterClassA(Name,INFO->hinstance);
        }
        INFO->active=0;
        INFO->pause=0;
    }

    return 0;

}
#endif

#ifdef NSPIRE

void getKey(){
    static int pressed=0;

		if(isKeyPressed(KEY_NSPIRE_ESC)){
        INFO->active=0;
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

void gui_wait(){
    if(INFO->active){
        // Correct time for delay
        #ifdef NSPIRE
        getKey();
        #else
        INFO->Alg->time+=clock()-INFO->Alg->time_start;
        #endif
        if(INFO->pause){
            //INFO->Alg->time+=clock()-INFO->Alg->time_start;
            #ifdef NSPIRE
            while(!isKeyPressed(KEY_NSPIRE_P) && !isKeyPressed(KEY_NSPIRE_ESC))
                ;
            #else
            while(INFO->pause)
                Sleep(5);
            #endif
            //INFO->Alg->time_start=clock();
        }else{

            if(INFO->delay){
                // sub 1ms wait with time smearing
                static float time=0;
                time+=INFO->delay;
                if((int)time){
                    Sleep((int)time);
                    time-=(int)time;
                }
            }

        }
        #ifdef NSPIRE
        gui_update();
        #else
        INFO->Alg->time_start=clock();
        #endif
    }
}


void gui_update(){
    if(INFO->active){
        #ifdef _WIN32
        PostMessageA(INFO->hwnd,WM_PAINT,0,0);
		#elif defined(__unix__)
        //render();
		//XEvent e;
		//XSendEvent(d,w,0,ExposureMask,&e);
        #else
        render();
        #endif
    }
}

#undef realloc
void gui_updateList(LIST* List){
    if(INFO->active){
        INFO->no_render=1;
		while(INFO->rendering && INFO->active)
            Sleep(1);
        INFO->List=List;
		if(List)
        	INFO->max=get_max(List);
        if(!INFO->r_pointers){
            INFO->r_pointers = malloc(sizeof(LIST));
            *INFO->r_pointers = CreateList(List->size);
            INFO->g_pointers = malloc(sizeof(LIST));
            *INFO->g_pointers = CreateList(List->size);
        }
        INFO->r_pointers->data = realloc(INFO->r_pointers->data, List->size*sizeof(int));
        INFO->g_pointers->data = realloc(INFO->g_pointers->data, List->size*sizeof(int));
        INFO->r_pointers->size= List->size;
        INFO->g_pointers->size= List->size;

        INFO->no_render=0;
    }
}

void gui_p_g_clear(){
     if(INFO->active){
        if(INFO->g_pointers)
            if(INFO->g_pointers->size>INFO->g_pointers->filled)
                INFO->g_pointers->filled=0;
     }
}
void gui_p_r_clear(){
     if(INFO->active){
        if(INFO->r_pointers)
            if(INFO->g_pointers->size>INFO->g_pointers->filled)
                INFO->r_pointers->filled=0;
     }
}
void gui_pointer2(int p){
     if(INFO->active){
        if(INFO->r_pointers)
            INFO->r_pointers->data[INFO->r_pointers->filled++]=p;
     }
}

void gui_pointer1(int p){
     if(INFO->active){
        if(INFO->g_pointers)
            INFO->g_pointers->data[INFO->g_pointers->filled++]=p;
     }
}
