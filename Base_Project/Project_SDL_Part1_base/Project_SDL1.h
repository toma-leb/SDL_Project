﻿// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.

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

// Helper function to initialize SDL
void init();
// class moving_object {

// }
class animal
{
private:
    SDL_Surface *window_surface_ptr_; // ptr to the surface on which we want the
                                      // animal to be drawn, also non-owning
    SDL_Surface *image_ptr_; // The texture of the sheep (the loaded image), use
                             // load_surface_for
    // todo: Attribute(s) to define its position

protected:
    size_t a_width;
    size_t a_height;
    int _speed;

public:
    int _pos_x;
    int _pos_y;
    animal(const std::string &file_path, SDL_Surface *window_surface_ptr);
    // todo: The constructor has to load the sdl_surface that corresponds to the
    // texture
    virtual ~animal(); // todo: Use the destructor to release memory and "clean
                       // up behind you"

    void draw();
    // todo: Draw the animal on the screen <-> window_surface_ptr.
    // Note that this function is not virtual, it does not depend
    // on the static type of the instance

    virtual void move(){}; // todo: Animals move around, but in a different
                           // fashion depending on which type of animal
    virtual void interact(std::unique_ptr<animal> &ani){};
    // todo: Animals interact around, but in a different
    // fashion depending on which type of animal
};

// Insert here:
// class sheep, derived from animal
class sheep : public animal
{
    // Ctor
    char _genre;

public:
    sheep(SDL_Surface *window_surface_ptr);
    // Dtor
    virtual ~sheep() override;
    // implement functions that are purely virtual in base class
    virtual void move() override;
    virtual void interact(std::unique_ptr<animal> &ani) override;
};

class shepherd_dog : public animal
{
    // Ctor
    float cir_angle = 0;

public:
    int distance_max = 25;
    shepherd_dog(SDL_Surface *window_surface_ptr);
    // Dtor
    virtual ~shepherd_dog() override;
    // implement functions that are purely virtual in base class
    virtual void move() override;
    void follow_shepherd_move(int shepherd_x, int shepherd_y);
};

// class wolf, derived from animal
class wolf : public animal
{
    // const std::string file_path = "../../media/wolf.png";
    // Ctor
    unsigned int dead_time = 2000;
    int hunt_distance = 2000;
    int eat_distance = 30;
    int target_x = 0;
    int target_y = 0;
    int min_distance = 100;
    int avoid_x = 0;
    int avoid_y = 0;
    bool run_away = false;

public:
    wolf(SDL_Surface *window_surface_ptr);
    // Dtor
    virtual ~wolf() override;
    // implement functions that are purely virtual in base class
    virtual void move() override;
    virtual void interact(std::unique_ptr<animal> &ani) override;
};
class shepherd
{
private:
    // Attention, NON-OWNING ptr, again to the screen
    SDL_Surface *window_surface_ptr_;
    unsigned shepherd_h = 89;
    unsigned shepherd_w = 60;

public:
    int _pos_x;
    int _pos_y, _speed;
    // ground() = default;
    shepherd(SDL_Surface *); // Ctor
    ~shepherd(); // Dtor, again for clean up (if necessary)
    void draw();
    void move(char direction);
};
// The "ground" on which all the animals live (like the std::vector
// in the zoo example).
class ground
{
private:
    // Attention, NON-OWNING ptr, again to the screen
    SDL_Surface *window_surface_ptr_;
    // const std::string file_path = "../../media/farm.png";

    // Some attribute to store all the wolves and sheep
    unsigned _n_sheep;
    unsigned _n_wolf;
    std::vector<std::unique_ptr<animal>> animals;
    // here

public:
    std::unique_ptr<shepherd> _shepherd; // TODO

    // ground() = default;
    ground(SDL_Surface *); // Ctor
    ~ground(){}; // Dtor, again for clean up (if necessary)
    void draw();
    void add_animal(unsigned n_sheep, unsigned n_wolf); // todo: Add an animal
    void update(); // todo: "refresh the screen": Move animals and draw them
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