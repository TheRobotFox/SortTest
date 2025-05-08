#pragma once
#include "UI.hpp"
#include "../SortList.hpp"
#include <chrono>
#include <cstddef>
#include <list>
#include <mutex>
#include <string>


struct WindowStyle
{
    UI::Color foreground = {.r = 255, .g = 255, .b = 255},
        background = { .r = 0, .g = 0, .b = 0 };
};

class GUI;

class Window
{
    std::weak_ptr<List> l;
    UI::Rect rect;
    friend GUI;
public:

    std::string title;
    WindowStyle style;
};

class GUI
{
    bool initilized {false};
    bool paused {false};

    std::mutex mtx;

    size_t skip_frames=0; // TODO factor out Render

    const SortAlgorithm *current;
    std::list<std::shared_ptr<Window>> windows;

    void handle_inputs();
    void windows_map();
    void render_window_list(Window &w, const std::shared_ptr<List>& list);

protected:

    UI::Config conf;
    UI::Rect screen_size = {};

    virtual auto create() -> bool = 0;
    virtual void destroy() = 0;
    virtual void update() = 0;

    virtual void sleep_while_active(size_t ms) = 0;
    virtual auto get_key() -> UI::KeyCmd = 0;

    virtual auto get_screen_dimensions() -> UI::Rect = 0;
    virtual void draw_begin() = 0;
        virtual void draw_rect(UI::Rect r, UI::Color c) = 0;
    virtual void draw_end() = 0;
    virtual void draw_text(UI::Rect pos, std::string text) = 0;

    void render();
    auto active() -> bool {return current!=nullptr && initilized;}


public:
    static GUI* instance;


    auto start(UI::Config conf) -> bool;
    void stop();

    auto create_window(std::weak_ptr<List> l) -> std::weak_ptr<Window>;
    void wait_and_handle();

    auto set_speed(float ops) -> size_t;

    void set_algorithm(const SortAlgorithm &algorithm);

    virtual ~GUI() = default;
};

class SlowDown
{
    GUI &ui;
    size_t restore;
public:
    SlowDown(GUI &ui, float ops)
        : ui(ui), restore(ui.set_speed(ops))
    {}
    ~SlowDown()
    {
        ui.set_speed(restore);
    }
};
