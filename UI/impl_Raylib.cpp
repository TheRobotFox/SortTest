#include "OS.hpp"
#include "UI.hpp"
#include <deque>
#include <memory>
#include <mutex>
#include <raylib.h>
#include <thread>

using UI::Rect;
using UI::KeyCmd;
using UColor = UI::Color;
using UI::GUI;


auto color(UI::Color c) -> Color
{
    return {.r = c.r, .g = c.g, .b=c.b, .a=255};
}

class Raylib : public GUI {

        std::thread *render_thread;

protected:

    auto get_screen_dimensions() -> Rect override
    {
        return {
            .left = 0, .top = 0,
            .right = GetScreenWidth(),
            .bottom = GetScreenHeight()
        };
    }

    void draw_begin() override
    {
        BeginDrawing();
    }

    void draw_rect(Rect r, UColor c) override {
        DrawRectangle(r.left, r.top, r.right - r.left, r.bottom - r.top, color(c));
    }

    void draw_end() override
    {
        EndDrawing();
    }
    void draw_text(Rect pos, std::string text) override
    {
        draw_rect(pos, {});
      DrawText(text.c_str(), pos.left,
               pos.top, pos.bottom - pos.top,
               Color{.r=255, .g=255, .b=255, .a = 255});
    }
    auto create() -> bool override
    {
        render_thread = new std::thread([this] {
            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
            InitWindow(800, 450, "Sort Test");
            SetTargetFPS(30);
            while(true){
                render();
                update();
            }
        });
        render_thread->detach();
        return true;
    }

    std::mutex cmd_queue_mtx;
    std::deque<KeyCmd> cmd_queue;

    void update() override
    {
        if(WindowShouldClose()) return stop();
        char key;
        while((key = GetCharPressed()) > 0){
            KeyCmd cmd = KeyCmd::NONE;
            if(key=='p') cmd = KeyCmd::TOGGLE_PAUSE;
            if(key=='+') cmd = KeyCmd::SPEED_UP;
            if(key=='-') cmd = KeyCmd::SPEED_DOWN;
            if(cmd!= KeyCmd::NONE){
                std::lock_guard<std::mutex> g(cmd_queue_mtx);
                cmd_queue.push_back(cmd);
            }
        }
    }

    auto get_key() -> KeyCmd override
    {
        KeyCmd res = KeyCmd::NONE;
        if(!cmd_queue.empty()) {

            res = cmd_queue.front();

            std::lock_guard<std::mutex> g(cmd_queue_mtx);
            cmd_queue.pop_front();

        }
        return res;
    }

    void destroy() override
    {
        if(render_thread == nullptr) return;
        render_thread->join();
        delete render_thread;
    }

    void sleep_while_active(size_t ms) override
    {
        WaitTime(ms/1000.0);
    }
    public:
        Raylib() = default;
};

GUI *GUI::instance = new Raylib();
