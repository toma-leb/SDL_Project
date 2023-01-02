// SDL_Test.h: Include file for include standard system files

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <any>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>

// Defintions
constexpr double frame_rate = 30.0; // refresh rate
constexpr double frame_time = 1000. / frame_rate;
constexpr unsigned frame_width = 1400; // Width of window in pixel
constexpr unsigned frame_height = 900; // Height of window in pixel
// Minimal distance of animals to the border
constexpr unsigned frame_boundary = 100;
// Animals default properties
// ---------- sheeps --------------
constexpr int sheep_danger_dis = 40;
constexpr int sheep_addition_speed = 5;
constexpr int sheep_normal_speed = 5;
constexpr int sheep_offspring_counter = 100;
constexpr int sheep_offspring_dis = 40; // collision detection for the offspring
// ---------- wolfs ---------------
constexpr unsigned wolf_hunger_count = 300;
constexpr int wolf_start_search_sheep_dis = 2000;
constexpr int wolf_danger_dis = 90;
constexpr int wolf_eat_dis = 40;
// ---------- shepherd dog --------
constexpr float dog_spin_speed = 0.1; // 0 to 1
constexpr int dog_dis_fr_shepherd = 150;
constexpr int dog_dis_fr_order = 10; // the distance between the dog and the position ordered

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
    int _pos_x;
    int _pos_y;
    std::string _type;
    std::map<std::string, std::any> _props;

    moving_object(const std::string &file_path,
                  SDL_Surface *window_surface_ptr);
    virtual ~moving_object(); // Use the destructor to release memory and
    virtual void draw();

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
    int wolf_x = 0;
    int wolf_y = 0;
    // list of the wolfs that is near the sheep
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
    int shepherd_x = 0;
    int shepherd_y = 0;
    bool clockwise = true; 
    //  the rotate's direction of dog

public:
    shepherd_dog(SDL_Surface *window_surface_ptr);
    // Dtor
    virtual ~shepherd_dog() override;

    virtual void move() override;

    virtual void interact(moving_object &obj) override;

    // void go_around_shepherd();
};

// class wolf, derived from animal
class wolf : public animal
{
    // Ctor
    int sheep_x = 0;
    int sheep_y = 0;
    int dog_x = 0;
    int dog_y = 0;

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
    void get_order(int pos_x, int pos_y);
};

// The "ground" on which all the animals live
class ground
{
private:
    SDL_Surface *window_surface_ptr_;
    unsigned _n_sheep;
    unsigned _n_wolf;
    std::vector<std::unique_ptr<animal>> animals;

public:
    std::unique_ptr<shepherd> _shepherd;
    ground(SDL_Surface *); // Ctor

    ~ground(){}; // Dtor, again for clean up (if necessary)
    void draw();

    void add_animal(unsigned n_sheep, unsigned n_wolf);

    void update(); // "refresh the screen": Move animals and draw them

    void offspring_check(unsigned index);
};

// The application class, which is in charge of generating the window
class application
{
private:
    SDL_Window *window_ptr_;
    SDL_Surface *window_surface_ptr_;
    SDL_Event window_event_;

    std::unique_ptr<ground> _ground;

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