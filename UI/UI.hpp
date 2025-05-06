#pragma once
#include "../SortList.hpp"
#include "OS.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <map>
#include <list>
#include <mutex>
#include <ranges>
#include <string>
#include <thread>
#include <vector>
#include <memory>

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

namespace UI {

    struct UI_conf
    {
        int ops_per_second=20;
        int mergedistance=10;
        float resolution_scale=1.0;
    };

    class GUI;

    struct WindowStyle
    {
        Color foreground = {.r = 255, .g = 255, .b = 255},
            background = { .r = 0, .g = 0, .b = 0 };
    };

    class Window
    {
        std::weak_ptr<List> l;
        Rect rect;
        friend GUI;
    public:

        std::string title;
        WindowStyle style;
    };

    class GUI : protected OS,
               protected Input,
               protected Draw
    {
        std::list<std::shared_ptr<Window>> windows;
        bool initilized {false};
        bool paused {false};

        std::mutex mtx;


        std::chrono::microseconds delay;
        size_t skip_frames=0;

        SortAlgorithm *current;

        auto active() -> bool {return current!=nullptr && initilized;}
        void handle_inputs()
        {
            #define DELAY_MIN std::chrono::microseconds(2)
            #define DELAY_MAX std::chrono::microseconds(3000000)
            switch(get_key())
            {
            using enum KeyCmd;

            case TOGGLE_PAUSE:
                paused=!paused; break;
            case SPEED_DOWN:
                if(skip_frames>0) skip_frames--;
                else {
                    delay=std::chrono::microseconds((int)(delay.count() *1.1f));
                    if(delay>DELAY_MAX) delay=DELAY_MAX;
                }
                break;
            case SPEED_UP:
                if(delay==DELAY_MIN) skip_frames++;
                else {
                    delay=std::chrono::microseconds((int)(delay.count() /1.2f));
                    if(delay<=DELAY_MIN) delay=DELAY_MIN;
                }
                break;
            default:
                break;
            }
        }
        auto windows_cleanup() -> bool
        {
            auto len = windows.size();
            std::ranges::remove_if(windows, [](auto &w){return w->l.expired();});
            return len!=windows.size();
        }
        void windows_map()
        {
            using namespace std::ranges::views;


            int count = windows.size();

            int cols = ceil(std::sqrt(count)),
                    y_size = screen_size.bottom/std::ceil((float)count/cols);

            int row = 0;
            auto it = windows.begin();
            while(it != windows.end()){
                int current_cols = std::min(count,cols);
                int x_size = screen_size.right/current_cols;
                for(int i = 0; i<current_cols; i++){
                    it++->get()->rect = {
                        .left = x_size*i, .top = y_size*row,
                        .right = x_size*(i+1), .bottom = y_size*(row+1)
                    };
                }
                count-=current_cols;
                row++;
            }
        }

        void render_window_list(Window &w, const std::shared_ptr<List>& list)
        {
            // clear Background
            draw_rect(w.rect, w.style.background);

            float step = conf.resolution_scale;

            int old_index = -1;

            for(float x=w.rect.left; x<=w.rect.right-step; x+=step){

                int index=(int)((x-w.rect.left)/(float)(w.rect.right-w.rect.left)*(list->size()));

                Rect line;

                line.left=x;
                line.top=w.rect.bottom-list->list[index].elem/(float)list->max*(w.rect.bottom-w.rect.top)/1.1;

                line.right=x+step;

                // Render seperation
                if(index!=old_index && list->size()*conf.mergedistance<w.rect.right){
                    old_index=index;
                    continue;
                }

                Color col;

                // set marked color
                if(list->marks.contains(index))
                    col=list->marks[index];
                else
                    col=w.style.foreground;

                draw_rect(line,col);
            }
        }

    protected:

        UI_conf conf;

        virtual auto create() -> bool = 0;
        virtual void destroy() = 0;
        virtual void update() = 0;

        Rect screen_size = {};

        void render()
        {
            mtx.lock();
            static Rect screen_size_last = {};

            screen_size = get_screen_dimensions();

            if(windows_cleanup() // remove dead windows
               || screen_size!=screen_size_last)
                windows_map();

            screen_size_last = screen_size;

            draw_begin();
            for(auto &win : windows){

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

                // Draw List
                std::shared_ptr<List> list = win->l.lock();
                if(!list) continue;

                {
                    std::lock_guard<std::mutex> g(list->mtx);
                    render_window_list(*win, list);
                }
            }

            int win_len = screen_size.right-screen_size.left;
            int win_hei = screen_size.bottom-screen_size.top;
            Rect text_rect = {
            .left = 0,
                .top = 0,
                .right = win_len/5,
                .bottom = (win_hei/10)
            };
            draw_text(text_rect, "win_count: " + std::to_string(windows.size()));
            mtx.unlock();
            draw_end();
        }

    public:

        static GUI* instance;

        auto start(UI_conf conf) -> bool
        {
            this->conf = conf;
            this->delay = std::chrono::milliseconds(1000)/conf.ops_per_second;
            return ( initilized = create() );
        }

        void stop()
        {
            initilized = false;
            destroy();
        }

        void set_algorithm(SortAlgorithm *algorithm)
        {
            std::lock_guard<std::mutex> g(mtx);
            current = algorithm;
        }

        auto create_window(std::weak_ptr<List> l) -> std::weak_ptr<Window>
        {
            std::lock_guard<std::mutex> g(mtx);
            auto win = std::make_shared<Window>();
            win->l = std::move(l);
            windows.emplace_back(win);

            windows_cleanup();
            windows_map();

            return win;
        }

        void wait_and_handle()
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

            long wait_ms = std::chrono::duration_cast<std::chrono::milliseconds>(e-s+delay).count();
            sleep_while_active(std::max(wait_ms, 0L));
            // if(e-s < delay){
            //     sleep_while_active(delay-(e-s));
            //     current->time += delay; // compensate for Rendering
            // } else {
            //     current->time += (e-s); // compensate for Rendering
            // }
        }
        virtual ~GUI() = default;
            void r(){render(); update();}
    };

}
