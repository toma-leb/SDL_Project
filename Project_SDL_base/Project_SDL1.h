// SDL_Test.h: Includedatei für Include-Standardsystemdateien

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>

// Defintions
constexpr double frame_rate = 60.0; // refresh rate
constexpr double frame_time = 1. / frame_rate;
constexpr unsigned frame_width = 1400; // Width of window in pixel
constexpr unsigned frame_height = 900; // Height of window in pixel
// Minimal distance of animals to the border
// of the screen
constexpr unsigned frame_boundary = 100;
constexpr unsigned hunger_count = 300;

// Helper function to initialize SDL
void init();
class moving_object
{
private:
    SDL_Surface *window_surface_ptr_;
    SDL_Surface *image_ptr_;

protected:
    size_t _width;
    size_t _height;
    int _speed;

public:
    bool alive = true;
    std::string _type;
    int _pos_x;
    int _pos_y;

    moving_object(const std::string &file_path,
                  SDL_Surface *window_surface_ptr);
    virtual ~moving_object(); // Use the destructor to release memory and
    virtual void draw();
    // virtual void interact(std::unique_ptr<moving_object> &obj);

    // virtual void move(){};
};
class animal : public moving_object
{
public:
    animal(const std::string &file_path, SDL_Surface *window_surface_ptr);

    virtual ~animal(){};

    virtual void move(){}; // Animals move around, but in a different

    virtual void interact(moving_object &obj){};
};
class sheep : public animal
{
    char _genre;
    int danger_distance = 40;
    int danger_x = 0;
    int danger_y = 0;
    int scare_speed = 5;
    std::vector<bool> wolfs_nearby;

public:
    sheep(SDL_Surface *window_surface_ptr);
    virtual ~sheep() override;
    virtual void move() override;
    virtual void interact(moving_object &obj) override;
};

class shepherd_dog : public animal
{
    // Ctor
    float spin_speed = 0.1; // 0 to 1
    int shepherd_x = 0;
    int shepherd_y = 0;
    int dis_fr_shepherd = 5;
    int angle_incre = 1;

public:
    shepherd_dog(SDL_Surface *window_surface_ptr);
    // Dtor
    virtual ~shepherd_dog() override;

    virtual void move() override;

    // void follow_shepherd_move(int shepherd_x, int shepherd_y);

    virtual void interact(moving_object &obj) override;
};

// class wolf, derived from animal
class wolf : public animal
{
    // Ctor
    unsigned int dead_time = 2000;
    int closest_sheep_dis = 2000;
    int eat_dis = 40;
    int sheep_x = 0;
    int sheep_y = 0;
    int _hunger_count = hunger_count;

    int danger_dis = 90;
    int dog_x = 0;
    int dog_y = 0;
    bool run_away = false;

public:
    wolf(SDL_Surface *window_surface_ptr);
    // Dtor
    virtual ~wolf() override;
    virtual void move() override;
    virtual void interact(moving_object &obj) override;
};

class shepherd : public moving_object
{
    char direction;

public:
    shepherd(SDL_Surface *); // Ctor
    ~shepherd(); // Dtor, again for clean up (if necessary)

    void move(char direction);
    void order(int pos_x, int pos_y, moving_object &obj);
};

// The "ground" on which all the animals live
class ground
{
private:
    // Attention, NON-OWNING ptr, again to the screen
    SDL_Surface *window_surface_ptr_;
    // Some attribute to store all the wolves and sheep
    unsigned _n_sheep;
    unsigned _n_wolf;
    std::vector<std::unique_ptr<animal>> animals;

public:
    std::unique_ptr<shepherd> _shepherd;

    // ground() = default;
    ground(SDL_Surface *); // Ctor

    ~ground(){}; // Dtor, again for clean up (if necessary)
    void draw();

    void add_animal(unsigned n_sheep, unsigned n_wolf);

    void update(); // "refresh the screen": Move animals and draw them

    // Possibly other methods, depends on your implementation
};

// The application class, which is in charge of generating the window
class application
{
private:
    // The following are OWNING ptrs
    SDL_Window *window_ptr_;
    SDL_Surface *window_surface_ptr_;
    SDL_Event window_event_;

    // Other attributes here, for example an instance of ground
    std::unique_ptr<ground> _ground; // TODO
    unsigned application_h = frame_height;
    unsigned application_w = frame_width;

public:
    application(unsigned n_sheep, unsigned n_wolf); // Ctor
    ~application(); // dtor

    int loop(unsigned period); // main loop of the application.
                               // this ensures that the screen is updated
                               // at the correct rate.
                               // See SDL_GetTicks() and SDL_Delay() to enforce
                               // a duration the application should terminate
                               // after 'period' seconds
};