// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Project_SDL1.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <random>
#include <string>
#define PI 3.14159265

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
    int get_ran_pos_x(int pos_x, int speed, int a_width)
    {
        int rand_dir = std::rand() % 2;
        switch (rand_dir)
        {
        case 0: // go right
            if (pos_x + a_width + speed < frame_width)
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
        int rand_dir = std::rand() % 2;
        switch (rand_dir)
        {
        case 0: // go up
            if (pos_y - speed > 0)
                pos_y -= speed;
            break;
        case 1: // go down
            if (pos_y + speed + a_height < frame_height)
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
}
void animal::draw()
{
    auto dst_rect = SDL_Rect{ _pos_x, _pos_y, (int)a_height, (int)a_width };

    if (SDL_BlitSurface(image_ptr_, NULL, window_surface_ptr_, &dst_rect))
        throw std::runtime_error("Could not apply texture.");
}
// sheep
sheep::sheep(SDL_Surface *window_surface_ptr)
    : animal("../media/sheep.png", window_surface_ptr)
{
    this->a_height = 55;
    this->a_width = 55;

    this->_pos_x = std::rand() % 50 + 300; // right  || left
    this->_pos_y = std::rand() % 50 + 200; // up || down
    this->_speed = 5; // it's just the speed of the sheep
    this->_genre = std::rand() % 2 == 0 ? 'm' : 'f';
}
sheep::~sheep()
{
    std::cout << " A sheep died" << std::endl;
}

void sheep::move()
{
    if (_pos_x + a_width == 0)
        _pos_x -= _speed;
    else if (_pos_x + a_width == frame_width)
        _pos_x += _speed;
    else
        _pos_x = get_ran_pos_x(_pos_x, _speed, a_width);
    if (_pos_y == 0)
        _pos_y -= _speed;
    else if (_pos_y + a_height == frame_height)
        _pos_y += _speed;
    _pos_y = get_ran_pos_y(_pos_y, _speed, a_height);
} // wolfs
void sheep::interact(std::unique_ptr<animal> &animal)
{}
shepherd_dog::shepherd_dog(SDL_Surface *window_surface_ptr)
    : animal("../media/Shepherd_dog.png", window_surface_ptr)
{
    this->a_height = 46;
    this->a_width = 60;

    this->_pos_x = std::rand() % 10 + 300; // right  || left
    this->_pos_y = std::rand() % 10 + 250; // up || down
    this->_speed = 10; // it's just the speed of the sheep
}
shepherd_dog::~shepherd_dog()
{
    std::cout << " The shepherd_dog died" << std::endl;
}
void shepherd_dog::move()
{
    _pos_x = get_ran_pos_x(_pos_x, _speed, a_width);
    _pos_y = get_ran_pos_y(_pos_y, _speed, a_height);
}
void shepherd_dog::follow_shepherd_move(int shepherd_x, int shepherd_y)
{
    int xdiff = this->_pos_x - shepherd_x;
    int ydiff = this->_pos_y - shepherd_y;
    int distance = sqrt(xdiff * xdiff + ydiff * ydiff);
    if (distance > distance_max) // dog is far away from the master
    {
        cir_angle = 0;
        std::cout << distance << "\n";
        float angle = atan2(ydiff, xdiff) * (180 / PI);
        this->_pos_x += this->_speed * cos(angle);
        this->_pos_y += this->_speed * sin(angle);
    }
    else
    {
        cir_angle += (30) * (PI / 180); // Convert to radians
        std::cout << cir_angle << "\n";
        this->_pos_x =
            cos(cir_angle) * (xdiff)-sin(cir_angle) * (ydiff) + shepherd_x;
        this->_pos_y =
            sin(cir_angle) * (xdiff) + cos(cir_angle) * (ydiff) + shepherd_y;
    }
}
wolf::wolf(SDL_Surface *window_surface_ptr)
    : animal("../media/wolf.png", window_surface_ptr)
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
void wolf::interact(std::unique_ptr<animal> &animal)
{
    int xdiff = this->_pos_x - animal->_pos_x;
    int ydiff = this->_pos_y - animal->_pos_y;
    int distance = sqrt(xdiff * xdiff + ydiff * ydiff);

    if (typeid(animal) == typeid(shepherd_dog) && distance < min_distance)
    {
        avoid_x = animal->_pos_x;
        avoid_y = animal->_pos_y;
        run_away = true;
    }
    else if ((typeid(animal) == typeid(sheep)))
    {
        if (distance < eat_distance)
        {
            // animal = nullptr;
            hunt_distance != 2000;
            dead_time = 2000;
            std::cout << "eaten " << std::endl;
        }
        else if (distance < hunt_distance && hunt_distance != 2000)
        {
            target_x = animal->_pos_x;
            target_y = animal->_pos_y;
            hunt_distance = distance;
        }
        run_away = false;
    }
}
void wolf::move()
{
    if (avoid_x != 0 && avoid_y != 0 && run_away)
    {
        int xdiff = this->_pos_x - avoid_x;
        int ydiff = this->_pos_y - avoid_y;
        float angle = atan2(ydiff, xdiff) * (180 / PI);
        this->_pos_x -= this->_speed * cos(angle);
        this->_pos_y -= this->_speed * sin(angle);
    }
    else if (target_x != 0 && target_y != 0) // hunting for sheeps
    {
        int xdiff = this->_pos_x - target_x;
        int ydiff = this->_pos_y - target_y;
        float angle = atan2(ydiff, xdiff) * (180 / PI);
        this->_pos_x += this->_speed * cos(angle);
        this->_pos_y += this->_speed * sin(angle);
    }
    else
    {
        _pos_x = get_ran_pos_x(_pos_x, _speed, a_width);
        _pos_y = get_ran_pos_y(_pos_y, _speed, a_height);
    }
}
//---------------------- human
shepherd::shepherd(SDL_Surface *window_surface_ptr)
{
    window_surface_ptr_ = window_surface_ptr;
    this->_pos_x = std::rand() % 10 + 100; // right  || left
    this->_pos_y = std::rand() % 10 + 400; // up || down
    this->_speed = 10; // it's just the speed of the sheep
}
shepherd::~shepherd()
{
    std::cout << " player died !!!" << std::endl;
}
void shepherd::draw()
{
    std::srand(std::time(nullptr));
    auto dst_rect =
        SDL_Rect{ _pos_x, _pos_y, (int)shepherd_h, (int)shepherd_w };

    auto surf =
        load_surface_for("../media/Shepherd.png", window_surface_ptr_);

    if (SDL_BlitSurface(surf, NULL, window_surface_ptr_, &dst_rect))
        throw std::runtime_error("Could not apply texture.");
}
void shepherd::move(char direction) // TOTO: apply size
{
    switch (direction)
    {
    case 'u':
        if (_pos_y - _speed > 0)
            _pos_y -= _speed;
        break;
    case 'd':
        if (_pos_y + shepherd_h + _speed < frame_height)
            _pos_y += _speed;
        break;
    case 'l':
        if (_pos_x - _speed > 0)
            _pos_x -= _speed;
        break;
    case 'r':
        if (_pos_x + shepherd_w + _speed <= frame_width)
            _pos_x += _speed;
        break;
    }
}
//---------------------- ground
ground::ground(SDL_Surface *window_surface_ptr)
{
    window_surface_ptr_ = window_surface_ptr;
    _shepherd = std::make_unique<shepherd>(window_surface_ptr_);
}
void ground::add_animal(unsigned n_sheep, unsigned n_wolf)
{
    _n_sheep = n_sheep;
    _n_wolf = n_wolf;

    animals.resize(_n_wolf + 2 + _n_sheep);
    // add the dog
    animals[0] = std::make_unique<shepherd_dog>(window_surface_ptr_);
    // add sheeps
    for (unsigned i = 1; i <= _n_sheep; i++)
        animals[i] = std::make_unique<sheep>(window_surface_ptr_);
    // add wolfs
    for (unsigned i = _n_sheep + 1; i <= _n_wolf + 1 + _n_sheep; i++)
        animals[i] = std::make_unique<wolf>(window_surface_ptr_);
}
void ground::draw()
{
    std::srand(std::time(nullptr));
    auto dst_rect = SDL_Rect{ 0, 0, (int)frame_height, (int)frame_width };

    auto surf = load_surface_for("../media/farm.png", window_surface_ptr_);

    if (SDL_BlitSurface(surf, NULL, window_surface_ptr_, &dst_rect))
        throw std::runtime_error("Could not apply texture.");

    for (unsigned i = 0; i < animals.size(); i++)
        if (animals[i] != nullptr)
            animals[i]->draw();
    _shepherd->draw();
}
void ground::update()
{
    for (unsigned i = 0; i < animals.size(); i++)
        for (unsigned j = 0; j < animals.size(); j++)
            if (i != j && animals[i] != nullptr && animals[j] != nullptr)
                animals[i]->interact(animals[j]);

    for (unsigned i = 0; i < animals.size(); i++)
        if (animals[i] != nullptr)
            animals[i]->move();
    draw();
}

// application
application::application(unsigned n_sheep, unsigned n_wolf)
{
    window_ptr_ = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, frame_width,
                                   frame_height, SDL_WINDOW_SHOWN);

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
    Uint32 frameStart;
    char direction;
    bool quit = false;

    while (!quit && (SDL_GetTicks() - lastUpdateTime < period * 1000))
    {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&window_event_))
        {
            if (window_event_.type == SDL_QUIT)
            {
                quit = true;
                break;
            };
            if (window_event_.type == SDL_KEYDOWN)
            {
                switch (window_event_.key.keysym.sym)
                {
                case SDLK_z:
                    direction = 'u';
                    break;
                case SDLK_s:
                    direction = 'd';
                    break;
                case SDLK_q:
                    direction = 'l';
                    break;
                case SDLK_d:
                    direction = 'r';
                    break;
                }
            }
            if (window_event_.type == SDL_KEYUP)
            {
                direction = 0;
            }
        }
        // direction = 'u';
        _ground->_shepherd->move(direction);
        int frame_update = SDL_GetTicks() - frameStart;
        if (frame_time > frame_update)
            SDL_Delay(frame_time - frame_update);
        _ground->update();
        SDL_UpdateWindowSurface(window_ptr_);
    }

    // SDL_FreeSurface(surf);
    return 0;
}
