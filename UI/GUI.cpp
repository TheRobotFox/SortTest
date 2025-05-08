#include "GUI.hpp"
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <thread>
#include "../Sort.hpp"

using namespace UI;

auto GUI::start(Config conf) -> bool
{
    this->conf = conf;
    set_speed(conf.ops_per_second);

    return ( initilized = create() );
}
void GUI::stop()
{
    initilized = false;
    destroy();
}

auto GUI::set_speed(float ops) -> size_t
{
    size_t prev = conf.ops_per_second;
    conf.ops_per_second = ops;
    return prev;
}

void GUI::handle_inputs()
{
    #define OPS_MAX 100000000
    #define OPS_MIN 0.1
    switch(get_key())
    {
    using enum KeyCmd;

    case TOGGLE_PAUSE:
        paused=!paused; break;
    case SPEED_DOWN:

        if((conf.ops_per_second /=1.1) < OPS_MIN) conf.ops_per_second=OPS_MIN;
        break;
    case SPEED_UP:
        if((conf.ops_per_second *=1.1) > OPS_MAX) conf.ops_per_second=OPS_MAX;
        break;
    default:
        break;
    }
}

void GUI::windows_map()
{

    int count = windows.size();
    if(count == 0) return;

    int cols = floor(std::sqrt(count)),
        y_size = screen_size.bottom/std::ceil(count/(float)cols);

    int row = 0, current_cols = 0, x_size, i=0;
    for(auto& win : windows){
        if(current_cols==0){
            current_cols = std::min(count,cols);
            x_size = screen_size.right/current_cols;
            count-=current_cols;
            i=0;
            row++;
        }
        win->rect = {
        .left = x_size*i, .top = y_size*(row-1),
            .right = x_size*(i+1), .bottom = y_size*row
        };
        current_cols--;
        i++;
    }
}

void GUI::render_window_list(Window &w, const std::shared_ptr<List>& list)
{
    // clear Background
    draw_rect(w.rect, w.style.background);
    if(list->size()==0) return;

    float step = conf.resolution_scale;

    int old_index = -1;

    for(float x=w.rect.left; x<=w.rect.right-step; x+=step){

        int index=(int)((x-w.rect.left)/(float)(w.rect.right-w.rect.left)*(list->size()));
        const ElementCounter &e = list->list[index];

        Rect line;

        line.left=x;
        line.top=w.rect.bottom-e.elem/(float)list->max*(w.rect.bottom-w.rect.top)*0.9;

        line.right=x+step;

        // Render seperation
        if(index!=old_index && (int)list->size()*conf.mergedistance<w.rect.right){
            old_index=index;
            continue;
        }

        UI::Color col = list->m.get_color(&e);


        draw_rect(line,col);
    }
}
void GUI::render()
{
    static Rect screen_size_last = {};

    mtx.lock();
    screen_size = get_screen_dimensions();

    if((windows.remove_if([](auto &win){
        return win->l.expired();
    }) != 0U) || screen_size!=screen_size_last)
        windows_map();

    screen_size_last = screen_size;

    draw_begin();
    for(auto &win : windows){

        // Draw List
        if(auto list = win->l.lock())
        {
            std::lock_guard<std::mutex> g(list->mtx);
            render_window_list(*win, list);
        }

        //Draw Window Title
        int win_len = win->rect.right-win->rect.left;
        int win_hei = win->rect.bottom-win->rect.top;
        Rect text_rect = {
            .left = win->rect.right - (win_len/5),
            .top = win->rect.top,
            .right = win->rect.right,
            .bottom = win->rect.top + (win_hei/5)
        };
        draw_text(text_rect, win->title);
    }

    if(current != nullptr){
        Rect text_rect = {
            .left = 0,
            .top = 0,
            .right = screen_size_last.right/4,
            .bottom = screen_size_last.bottom/10
        };
        draw_text(text_rect, current->name);
    }
    mtx.unlock();
    draw_end();
}
void GUI::wait_and_handle()
{
    if(!active()) return;
    auto s = std::chrono::steady_clock::now().time_since_epoch();

    handle_inputs();
    while (paused){
        handle_inputs();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }


    // Render
    auto e = std::chrono::steady_clock::now().time_since_epoch();

    auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(1))/conf.ops_per_second;
    long wait_ms = std::chrono::duration_cast<std::chrono::milliseconds>(e-s+delay).count();
    sleep_while_active(std::max(wait_ms, 0L));
    // if(e-s < delay){
    //     sleep_while_active(delay-(e-s));
    //     current->time += delay; // compensate for Rendering
    // } else {
    //     current->time += (e-s); // compensate for Rendering
    // }
}

auto GUI::create_window(std::weak_ptr<List> l) -> std::weak_ptr<Window>
{
    auto win = std::make_shared<Window>();
    win->l = std::move(l);

    std::lock_guard<std::mutex> g(mtx);

    windows.remove_if([](auto &win){
        return win->l.expired();
    });
    windows.emplace_back(win);
    windows_map();

    return win;
}
void GUI::set_algorithm(const SortAlgorithm &algorithm)
{
    std::lock_guard<std::mutex> g(mtx);
    current = &algorithm;
}
