#pragma once
#include "UI.hpp"
#include "../SortList.hpp"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>


using ColorFn = std::function<UI::Color(const ElementCounter *, const Markings &)>;
struct WindowStyle
{
    UI::Color background = { .r = 0, .g = 0, .b = 0 };
    ColorFn foreground = color_default;
    inline static ColorFn color_default=[](const auto *e, const Markings &m){
        if(m.marks.contains(e)) return m.marks.at(e);
        switch (m.get_pointer(e)) {
            case Markings::Pointer::PRIMARY:
                return UI::Color{.r = 255, .g = 255, .b = 0};
            case Markings::Pointer::SECONDARY:
                return UI::Color{.r = 0, .g = 255, .b = 255};
            default:
                uint8_t channel = 255*(1.0-m.get_temp(e));
                return UI::Color{.r = 255, .g = channel, .b = channel};
            }
        },
        color_temp = [](const auto *e, const Markings &m){
            uint8_t channel = 255*(1.0-m.get_temp(e));
            return UI::Color{.r = 255, .g = channel, .b = channel};
        };
    inline static std::function<ColorFn(UI::Color)> const_color =
        [](UI::Color c){return [c=c](const auto *, const Markings &){return c;};};
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

    bool paused {false};

    std::mutex mtx;

    size_t skip_frames=0; // TODO factor out Render

    std::shared_ptr<const SortAlgorithm> current;
    std::list<std::shared_ptr<Window>> windows;

    void handle_inputs();
    void windows_map();
    void render_window_list(Window &w, const std::shared_ptr<List>& list);

protected:

    enum class State : uint8_t {
        NOT_CREATED,
        CREATED,
        DESTROYED
    };
    State state;

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
    auto active() -> bool {return current!=nullptr && state == State::CREATED;}


public:
    static GUI* instance;

    class ExitRequest : public std::exception{};


    auto start(UI::Config conf) -> bool;
    void stop();

    auto create_window(std::weak_ptr<List> l) -> std::weak_ptr<Window>;
    void wait_and_handle();

    auto set_speed(float ops) -> size_t;

        void set_algorithm(std::shared_ptr<const SortAlgorithm>);

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
