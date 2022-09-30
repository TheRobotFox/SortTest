#ifdef _WIN32
#include "GUI_impl.h"

static HANDLE Thread;
static HBITMAP bmp;
static HDC wndhdc, hdc;
static RECT screen_rect;
static HWND hwnd;
static HINSTANCE hinstance;

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
    RECT rc;
    GetClientRect(hwnd, &rc);
    return (struct Rect){0, 0, rc.right, rc.bottom};
}

void Draw_begin()
{
    GetClientRect(hwnd, &screen_rect);
    bmp = CreateCompatibleBitmap(wndhdc,screen_rect.right,screen_rect.bottom);
    SelectObject(hdc,bmp);

    // Clear Bitmap
    FillRect(hdc,&screen_rect,GetStockObject(BLACK_BRUSH));
}

void Draw_Rect(struct Rect *rect, struct Color col)
{
    RECT rc = {rect->right, rect->top, rect->left, rect->bottom};
    HBRUSH brush = CreateSolidBrush(RGB(col.r,col.g,col.b));
    FillRect(hdc,&rc,brush);
    DeleteObject(brush);
}

/*void Draw_Text(int x, int y, const char *text, size_t len, int size, struct Color col)
{
    HFONT font = CreateFontA(size,0,0,0,0,0,0,0,DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,0);
    SelectObject(hdc,font);

    SetTextColor(hdc, RGB(col.r, col.g, col.b));
    RECT rc = {x,y,screen_rect.right,screen_rect.bottom};
    DrawTextA(hdc, text, len, &rc,0);
    DeleteObject(font);
}*/

void Draw_end()
{
    BitBlt(wndhdc,0,0,screen_rect.right,screen_rect.bottom,hdc,0,0,SRCCOPY);
    DeleteObject(bmp);
    ValidateRect(hwnd,&screen_rect);
}

static LRESULT CALLBACK _GUI_tread_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){


    switch(msg)
    {
        case WM_CREATE:
        {
            //Init Objects
            wndhdc= GetDC(hwnd);
            hdc = CreateCompatibleDC(wndhdc);
            RECT rc;
            GetClientRect(hwnd,&rc);

            //Configure HDC
            SetBkColor(hdc,RGB(0,0,0));
            SetBkMode(hdc,TRANSPARENT);

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

static DWORD WINAPI GUI_thread_proc(LPVOID unused){

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
        hwnd = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, Name, Name, WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hinstance, NULL);

        if(hwnd){

            current_state.conf->active=1;
            MSG msg;

            while(GetMessage(&msg, NULL, 0, 0) > 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

        }
        UnregisterClassA(Name,hinstance);
    }
    current_state.conf->active=0;

    return 0;

}

int GUI_impl_create()
{

    SECURITY_ATTRIBUTES sa = {0};
    hinstance=GetModuleHandle(0);
    Thread = CreateThread(&sa,0,GUI_thread_proc, NULL,0,0);
    return 0;
}

int GUI_impl_destroy()
{
    PostMessageA(hwnd,WM_DESTROY,0,0);
    WaitForSingleObject(Thread,INFINITE);
    return 0;
}

void GUI_impl_update()
{
    //PostMessageA(hwnd,WM_PAINT,0,0);
}

#endif
