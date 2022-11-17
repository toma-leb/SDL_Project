// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
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
    void wait_until_next_second()
    {
        time_t before = time(0);
        while (difftime(time(0), before) < 1)
            ;
    }

    int get_ran_pos_x(int pos_x, int speed, int a_width)
    {
        size_t window_width = 1200;
        int rand_dir = std::rand() % 2;
        switch (rand_dir)
        {
        case 0: // go right
            if (pos_x + a_width + speed < window_width)
                pos_x += speed;
            break;
        case 1: // go left
            if (pos_x - speed > 0)
                pos_x -= speed;
            break;
        default: // go nowhere
            break;
        }
        return pos_x;
    }

    int get_ran_pos_y(int pos_y, int speed, int a_height)
    {
        size_t window_length = 800;
        int rand_dir = std::rand() % 2;
        switch (rand_dir)
        {
        case 0: // go up
            if (pos_y - speed > 0)
                pos_y -= speed;
            break;
        case 1: // go down
            if (pos_y + speed + a_height < window_length)
                pos_y += speed;
            break;
        default: // go nowhere
            break;
        }
        return pos_y;
    }
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
    // std::cout << " An animal died" << std::endl;
}
void animal::draw()
{
    auto dst_rect = SDL_Rect{ _pos_x, _pos_y, (int)a_height, (int)a_width };

    if (SDL_BlitSurface(image_ptr_, NULL, window_surface_ptr_, &dst_rect))
        throw std::runtime_error("Could not apply texture.");
}
// sheep
sheep::sheep(SDL_Surface *window_surface_ptr)
    : animal("../../media/sheep.png", window_surface_ptr)
{
    this->a_height = 55;
    this->a_width = 55;

    this->_pos_x = std::rand() % 50 + 300; // right  || left
    this->_pos_y = std::rand() % 50 + 200; // up || down
    this->_speed = 5; // it's just the speed of the sheep
}
sheep::~sheep()
{
    std::cout << " A sheep died" << std::endl;
}

void sheep::move()
{
    _pos_x = get_ran_pos_x(_pos_x, _speed, a_width);
    _pos_y = get_ran_pos_y(_pos_y, _speed, a_height);

    // if (this->_pos_x + a_width > 1200)
    //     right = false;
    // else if (this->_pos_x < 0)
    //     right = true;
    // _pos_x += right ? _speed : _speed * -1;

    // if (this->_pos_y + a_height > 800)
    //     down = false;
    // else if (this->_pos_y < 0)
    //     down = true;
    // _pos_y += down ? _speed : _speed * -1;
    // std::cout << right << _pos_x << std::endl;
} // wolfs

wolf::wolf(SDL_Surface *window_surface_ptr)
    : animal("../../media/wolf.png", window_surface_ptr)
{
    this->a_height = 42;
    this->a_width = 62;

    this->_pos_x = std::rand() % 10 + 1000; // right  || left
    this->_pos_y = std::rand() % 10 + 400; // up || down
    this->_speed = 10; // it's just the speed of the sheep
}
wolf::~wolf()
{
    std::cout << " A wolf died" << std::endl;
}

void wolf::move()
{
    _pos_x = get_ran_pos_x(_pos_x, _speed, a_width);
    _pos_y = get_ran_pos_y(_pos_y, _speed, a_height);
    // if (this->_pos_x + a_width > 1200)
    //     right = false;
    // else if (this->_pos_x < 0)
    //     right = true;
    // _pos_x += right ? _speed : _speed * -1;

    // if (this->_pos_y + a_height > 800)
    //     down = false;
    // else if (this->_pos_y < 0)
    //     down = true;
    // _pos_y += down ? _speed : _speed * -1;
}

//---------------------- ground
ground::ground(SDL_Surface *window_surface_ptr)
{
    window_surface_ptr_ = window_surface_ptr;
}
void ground::add_animal(unsigned n_sheep, unsigned n_wolf)
{
    _n_sheep = n_sheep;
    _n_wolf = n_wolf;
    sheeps.resize(_n_sheep);
    for (unsigned i = 0; i < _n_sheep; i++)
        sheeps[i] = std::make_unique<sheep>(window_surface_ptr_);
    wolfs.resize(_n_wolf);
    for (unsigned i = 0; i < _n_wolf; i++)
        wolfs[i] = std::make_unique<wolf>(window_surface_ptr_);
}
void ground::draw()
{
    std::srand(std::time(nullptr));

    auto dst_rect = SDL_Rect{ 0, 0, (int)application_h, (int)application_w };

    auto surf = load_surface_for("../../media/farm.png", window_surface_ptr_);

    if (SDL_BlitSurface(surf, NULL, window_surface_ptr_, &dst_rect))
        throw std::runtime_error("Could not apply texture.");

    for (unsigned i = 0; i < _n_sheep; i++)
        sheeps[i]->draw();
    for (unsigned i = 0; i < _n_wolf; i++)
        wolfs[i]->draw();
}
void ground::update()
{
    // sheep_->move();
    for (unsigned i = 0; i < _n_sheep; i++)
        sheeps[i]->move();
    for (unsigned i = 0; i < _n_wolf; i++)
        wolfs[i]->move();
    draw();
}

// application
application::application(unsigned n_sheep, unsigned n_wolf)
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

    _ground->draw();

    while (!quit && (SDL_GetTicks() - lastUpdateTime < period * 1000))
    {
        // wait_until_next_second();
        while (SDL_PollEvent(&window_event_))
        {
            if (window_event_.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
        }
        _ground->update();
        SDL_UpdateWindowSurface(window_ptr_);
    }

    // SDL_FreeSurface(surf);
    return 0;
}
