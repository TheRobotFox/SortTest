#include "GUI_impl.h"
#include "Font.font.h"

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

    const char *title;
    size_t title_len;

    struct Rect rect;
    float opacity;
    struct Color foreground;
    struct Color background;

    int do_render;
    int rendering;
};

struct GUI_State current_state = {0};
static char buff[256];


#define DELAY_MIN 0.000001
#define DELAY_MAX 100000

void handleInput(enum INPUT e)
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

static int f_Draw_Rect(Font_Rect *rect, void *col)
{
    struct Rect rc = {rect->x, rect->y, rect->x+rect->width, rect->y+rect->height};
    Draw_Rect(&rc, *(struct Color*)col);
    return 0;
}
static void Draw_Text(int x, int y, const char *text, size_t len, int size, struct Color col)
{
    Font_render_string_rect(&font, text, len, x, y, size, f_Draw_Rect, &col);
}

static void GUI_Window_render(struct GUI_Window *win)
{
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
            index=(int)((x-rect.left)/(float)(rect.right-1)*(list_size));

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

        if(win->title){
            unsigned char color = 255*(1-round((float)(win->background.r+win->background.g+win->background.b)/255.0f/3.0f));
            int size=(win->rect.bottom-win->rect.top)/15;
            Font_Rect title_size = Font_string_dimensions(&font, win->title, win->title_len, size);
            Draw_Text(win->rect.right-title_size.width-1, win->rect.top+1, win->title, win->title_len, size, (struct Color){color,color,color});
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
            int length=snprintf(buff,255,"%-20s - %.3fs",current_state.conf->Alg->name,current_state.conf->Alg->time/CLOCKS_PER_SEC);
            Draw_Text(1, (rect.bottom/10-rect.bottom/15)/2, buff, length, rect.bottom/15, (struct Color){255,255,255});

            // Print Pause
            if(current_state.paused){
                Draw_Text(rect.right/2, 0, "Paused", 6, rect.bottom/15, (struct Color){255,0,0});
            }
        }
        Draw_end();
        current_state.rendering = 0;
    }
}

int GUI_create(struct GUI_conf *conf)
{
    current_state.conf=conf;
    if(!conf->activate)
        return 0;

    current_state.windows = List_create(sizeof(struct GUI_Window));
    current_state.rendering=0;
    current_state.paused=0;

    GUI_impl_create();

    return 0;
}

void GUI_destroy()
{
    current_state.conf->do_render = 0;
    while(current_state.rendering) Sleep(5);

    for(struct GUI_Window *start = List_start(current_state.windows),
                          *end = List_end(current_state.windows);
                          start < end; start++)
        GUI_windows_remove(start->id);

    current_state.conf->active=0;

    List_free(current_state.windows);

    GUI_impl_destroy();
}

void GUI_update(int force){
    if(current_state.conf->active){
        static int skiped=0;
        if(skiped>=current_state.skip_frames || force){
            skiped=0;
            GUI_impl_update();
        }
        skiped++;
        current_state.skip_frames = 0.07/sqrt(current_state.conf->delay);
    }
}


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

union U_GUI_Window_id_wrapper
{
    GUI_Window_id id;
    void *ptr;
};

static void f_List_reserve_callback_stop_render(List l, enum E_CALLBACK_MSG msg, void *arg)
{
    union U_GUI_Window_id_wrapper wrapped_id = (union U_GUI_Window_id_wrapper)arg;
    GUI_Window_id id = wrapped_id.id;
    switch(msg)
    {
    case CM_PRE_REALLOC:
        GUI_Window_do_render(id, 0);
        break;

    case CM_POST_REALLOC:
        GUI_Window_do_render(id, 1);
        break;

    default:
        break;
    }
}

GUI_Window_id GUI_windows_append()
{
    if(current_state.conf->active){

        static GUI_Window_id current_id;

        if(!List_size(current_state.windows))
            current_id=0;

        current_state.conf->do_render = 0;
        while(current_state.rendering) Sleep(5);

        struct GUI_Window *win = List_append(current_state.windows, NULL);

        win->id=current_id++;
        win->marks = List_create(sizeof(struct GUI_Mark));

        //set callback for realloc
        union U_GUI_Window_id_wrapper wrapped_id = {win->id};
        List_reserve_callback(win->marks, f_List_reserve_callback_stop_render, wrapped_id.ptr);

        win->opacity=1.0f;
        win->do_render = 1;
        win->rendering = 0;
        win->title=NULL;
        win->foreground = (struct Color){255,255,255};
        win->background = (struct Color){0,0,0};

        current_state.conf->do_render = 1;

        // Remap all windows
        //GUI_windows_mapper();
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
            while(current_state.rendering) Sleep(5);
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
            //set callback for realloc
            union U_GUI_Window_id_wrapper wrapped_id = {id};
            List_reserve_callback(win->l, f_List_reserve_callback_stop_render, wrapped_id.ptr);
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
            if(!mark)
                mark = List_append(win->marks, NULL); // stops render in case of realloc

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

void GUI_Window_title_set(GUI_Window_id id, const char *title)
{
    if(current_state.conf->active){
        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win){
            win->title=title;
            if(title)
                win->title_len=strlen(title);
        }
    }
}

const char* GUI_Window_title_get(GUI_Window_id id)
{
    if(current_state.conf->active){
        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win)
            return win->title;
    }
    return NULL;
}

void GUI_Window_do_render(GUI_Window_id id, int do_render)
{
    if(current_state.conf->active){
        struct GUI_Window *win = List_find(current_state.windows, f_win_by_id, &id);
        if(win){
            win->do_render=do_render;
            if(!do_render)
                while(win->rendering) Sleep(5);
        }
    }
}

