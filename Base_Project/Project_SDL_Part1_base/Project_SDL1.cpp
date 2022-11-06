﻿// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Project_SDL1.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <random>
#include <string>

void init()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
        throw std::runtime_error("init():" + std::string(SDL_GetError()));

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
        throw std::runtime_error("init(): SDL_image could not initialize! "
                                 "SDL_image Error: "
                                 + std::string(IMG_GetError()));
}

namespace
{
    // Defining a namespace without a name -> Anonymous workspace
    // Its purpose is to indicate to the compiler that everything
    // inside of it is UNIQUELY used within this source file.

    SDL_Surface *load_surface_for(const std::string &path,
                                  SDL_Surface *window_surface_ptr)
    {
        // Helper function to load a png for a specific surface
        auto surf = IMG_Load(path.c_str());
        if (!surf)
            throw std::runtime_error("Could not load image");
        // See SDL_ConvertSurface

        auto op_surface_ptr =
            SDL_ConvertSurface(surf, window_surface_ptr->format, 0);
        if (op_surface_ptr == NULL)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(),
                   SDL_GetError());
        }
        SDL_FreeSurface(surf);
        return op_surface_ptr;
    }
    int get_rand_direction()
    {
        std::srand(std::time(nullptr));
        // use current time as seed for random generator
        int random_variable = std::rand() % 2 - 1;

        return random_variable;
    };
} // namespace

// animal
animal::animal(const std::string &file_path, SDL_Surface *window_surface_ptr)
{
    window_surface_ptr_ = window_surface_ptr;
    image_ptr_ = load_surface_for(file_path, window_surface_ptr_);
}
animal::~animal()
{
    SDL_FreeSurface(image_ptr_);
    image_ptr_ = NULL;
    std::cout << " An animal died" << std::endl;
}
void animal::draw()
{
    auto dst_rect = SDL_Rect{ 0, 0, (int)a_height, (int)a_width };
    if (SDL_BlitSurface(image_ptr_, NULL, window_surface_ptr_, &dst_rect))
        throw std::runtime_error("Could not apply texture.");
}
// sheep
sheep::sheep(SDL_Surface *window_surface_ptr)
    : animal("../../media/sheep.png", window_surface_ptr)
{
    this->_pos_x = 50;
    this->_pos_y = 50;
    this->_speed = 10;
}
sheep::~sheep()
{
    std::cout << " A sheep died" << std::endl;
}

void sheep::move()
{
    this->_pos_x += this->_speed * get_rand_direction();
    this->_pos_y += this->_speed * get_rand_direction();
}
// wolfs
//---------------------- ground
ground::ground(SDL_Surface *window_surface_ptr)
{
    window_surface_ptr_ = window_surface_ptr;
}
void ground::add_animal(unsigned n_sheep, unsigned n_wolf)
{
    this->_n_sheep = n_sheep;
    this->_n_wolf = n_wolf;
    // _sheeps = std::make_unique<sheep[]>(n_sheep);
    // _wolfs = std::make_unique<wolf[]>(n_wolf);
}
// ground::~ground()
// {
//     SDL_FreeSurface(window_surface_ptr_);
//     window_surface_ptr_ = NULL;
// }

void ground::draw(SDL_Surface *window_surface_ptr_)
{
    auto dst_rect = SDL_Rect{ 0, 0, (int)application_h, (int)application_w };

    auto surf = load_surface_for("../../media/farm.png", window_surface_ptr_);

    if (SDL_BlitSurface(surf, NULL, window_surface_ptr_, &dst_rect))
        throw std::runtime_error("Could not apply texture.");
}
void ground::update()
{}

// application
application::application(unsigned n_sheep, unsigned n_wolf)
// : _ground(window_surface_ptr_)
{
    window_ptr_ = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, application_w,
                                   application_h, SDL_WINDOW_SHOWN);

    if (!window_ptr_)
        throw std::runtime_error(std::string(SDL_GetError()));

    window_surface_ptr_ = SDL_GetWindowSurface(window_ptr_);

    if (!window_surface_ptr_)
        throw std::runtime_error(std::string(SDL_GetError()));
    _ground = std::make_unique<ground>(window_surface_ptr_);
    // this->_ground = ground(window_surface_ptr_);
    _ground->add_animal(n_sheep, n_wolf);
}

application::~application()
{
    SDL_FreeSurface(window_surface_ptr_);
    window_surface_ptr_ = NULL;

    SDL_DestroyWindow(window_ptr_);
    window_ptr_ = NULL;
}

int application::loop(unsigned period)
{
    auto lastUpdateTime = SDL_GetTicks();
    bool quit = false;

    // auto src_rect = SDL_Rect{ 0, 0, (int)application_h, (int)application_w };
    // auto dst_rect = SDL_Rect{ 0, 0, (int)application_h, (int)application_w };

    // auto surf = load_surface_for("../../media/farm.png",
    // window_surface_ptr_);

    // if (SDL_BlitSurface(surf, NULL, window_surface_ptr_, &dst_rect))
    //     throw std::runtime_error("Could not apply texture.");
    _ground->draw(window_surface_ptr_);

    SDL_UpdateWindowSurface(window_ptr_);

    while (!quit && (SDL_GetTicks() - lastUpdateTime < period * 1000))
        while (SDL_PollEvent(&window_event_))
        {
            if (window_event_.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
        }
    // SDL_FreeSurface(surf);
    return 0;
}
