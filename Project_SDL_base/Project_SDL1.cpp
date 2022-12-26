// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Project_SDL1.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
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
        // auto surf = SDL_LoadBMP(path.c_str());
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

    // genarate a pos_x or pos_y, with speed = a_speed,
    // limit = frame_width or frame_height,
    // var = a_width or a_height
    int get_ran_pos(int pos, int speed, int var, int limit)
    {
        int rand_dir = std::rand() % 2;
        switch (rand_dir)
        {
        case 0: // go right
            if (pos + var + speed < limit)
                pos += speed;
            break;
        case 1: // go left
            if (pos - speed > 0)
                pos -= speed;
            break;
        default: // go nowhere
            break;
        }
        return pos;
    }
    int get_distance_between_2_point(int pos_x_1, int pos_y_1, int pos_x_2,
                                     int pos_y_2)
    {
        int xdiff = pos_x_1 - pos_x_2;
        int ydiff = pos_y_1 - pos_y_2;
        return sqrt(xdiff * xdiff + ydiff * ydiff);
    }

    float get_angle_between_2_point(int pos_x_1, int pos_y_1, int pos_x_2,
                                    int pos_y_2)
    {
        int xdiff = pos_x_1 - pos_x_2;
        int ydiff = pos_y_1 - pos_y_2;
        return atan2(ydiff, xdiff) * (180 / PI);
    }

} // namespace
// -------------- moving object class -----------------
moving_object::moving_object(const std::string &file_path,
                             SDL_Surface *window_surface_ptr)
{
    window_surface_ptr_ = window_surface_ptr;
    image_ptr_ = load_surface_for(file_path, window_surface_ptr_);
}
moving_object::~moving_object()
{
    SDL_FreeSurface(image_ptr_);
    image_ptr_ = NULL;
}

void moving_object::draw()
{
    auto dst_rect = SDL_Rect{ _pos_x, _pos_y, (int)_height, (int)_width };

    if (SDL_BlitSurface(image_ptr_, NULL, window_surface_ptr_, &dst_rect))
        throw std::runtime_error("Could not apply texture.");
}

// -------------------------------- animal class -------------

animal::animal(const std::string &file_path, SDL_Surface *window_surface_ptr)
    : moving_object(file_path, window_surface_ptr)
{
    std::cout << "new animal created \n";
}

// -------------------------------- sheeps class ----------------

sheep::sheep(SDL_Surface *window_surface_ptr)
    : animal("../media/sheep.png", window_surface_ptr)
{
    this->_height = 55;
    this->_width = 55;
    this->_type = "sheep";
    this->_pos_x = std::rand() % 50 + 300; // right  || left
    this->_pos_y = std::rand() % 50 + 200; // up || down
    this->_speed = 5; // it's just the speed of the sheep
    this->_genre = std::rand() % 2 == 0 ? 'm' : 'f';
}
sheep::~sheep()
{
    std::cout << " A sheep died" << std::endl;
}

// Sheep move randomly around the map except when getting too close to a wolf.
// In this case, they get a temporary speed boost in the opposite direction of
// the wolf.
void sheep::interact(moving_object &obj)
{
    bool wolf_nearby = false;
    if (obj._type.compare("wolf") == 0)
    {
        int distance = get_distance_between_2_point(this->_pos_x, this->_pos_y,
                                                    obj._pos_x, obj._pos_y);
        if (distance < danger_distance + _width)
        {
            danger_x = obj._pos_x;
            danger_y = obj._pos_y;
            wolf_nearby = true;
        }
    }
    if (obj._type.compare("sheep") == 0)
    {
    }
    wolfs_nearby.push_back(wolf_nearby);
}

void sheep::move()
{
    // if there are a wolf nearby then run !!!
    if (std::binary_search(wolfs_nearby.begin(), wolfs_nearby.end(), true))
    {
        std::cout << " wolf nearly \n";
        _speed += scare_speed;
        float angle =
            get_angle_between_2_point(_pos_x, _pos_y, danger_x, danger_y);
        if (_pos_x - _speed > 0)
            _pos_x -= _speed * cos(angle);
        if (_pos_y - _speed > 0)
            _pos_y -= _speed * sin(angle);
        wolfs_nearby.clear();
    }
    else
    {
        _speed = 5;
        _pos_y = get_ran_pos(_pos_y, _speed, _height, frame_height);
        _pos_x = get_ran_pos(_pos_x, _speed, _width, frame_width);
    }
}

// -------------------------------- wolfs class-----------

wolf::wolf(SDL_Surface *window_surface_ptr)
    : animal("../media/wolf.png", window_surface_ptr)
{
    this->_height = 42;
    this->_width = 62;
    this->_type = "wolf";
    this->_pos_x = std::rand() % 10 + 1000; // right  || left
    this->_pos_y = std::rand() % 10 + 400; // up || down
    this->_speed = 10;
}
wolf::~wolf()
{
    std::cout << " A wolf died" << std::endl;
}
void wolf::interact(moving_object &obj)
{
    int distance =
        get_distance_between_2_point(_pos_x, _pos_y, obj._pos_x, obj._pos_y);
    if (obj._type.compare("sheep") == 0) // look for the closest sheep
    {
        if (distance < closest_sheep_dis)
        {
            sheep_x = obj._pos_x;
            sheep_y = obj._pos_y;
            closest_sheep_dis = distance;
        }
        if (distance < eat_dis) // eat sheep
        {
            obj.alive = false;
            _hunger_count = hunger_count;
            closest_sheep_dis = 2000;
            sheep_x = 0;
            sheep_y = 0;
        }
        // std::cout << "interact sheep" << closest_sheep_dis << "\n";
    }
    else if (obj._type.compare("shepherd_dog") == 0)
    {
        // std::cout << "interact dog\n";
        run_away = false;
        if (distance < danger_dis)
        {
            dog_x = obj._pos_x;
            dog_y = obj._pos_y;
            run_away = true;
        }
    }
}
void wolf::move()
{
    if (run_away) // avoiding dog
    {
        std::cout << " run away dog \n";
        float angle = get_angle_between_2_point(_pos_x, _pos_y, dog_x, dog_y);

        this->_pos_x -= this->_speed * cos(angle);
        this->_pos_y -= this->_speed * sin(angle);
    }
    else if (sheep_x != 0 && sheep_y != 0) // hunting for sheeps
    {
        // std::cout << "hunting sheeps \n";
        float angle =
            get_angle_between_2_point(_pos_x, _pos_y, sheep_x, sheep_y);

        this->_pos_x += this->_speed * cos(angle);
        this->_pos_y += this->_speed * sin(angle);
        _hunger_count -= 1;
        closest_sheep_dis = 2000; // reset distance to closest
    }
    else
    {
        _pos_x = get_ran_pos(_pos_x, _speed, _width, frame_width);
        _pos_y = get_ran_pos(_pos_y, _speed, _height, frame_height);
    }
    alive = _hunger_count != 0 ? true : false;
}
// -------------------------------- shepherd_dog class --------------

shepherd_dog::shepherd_dog(SDL_Surface *window_surface_ptr)
    : animal("../media/Shepherd_dog.png", window_surface_ptr)
{
    this->_height = 46;
    this->_width = 60;
    this->_type = "shepherd_dog";
    this->_pos_x = std::rand() % 10 + 200; // right  || left
    this->_pos_y = std::rand() % 10 + 350; // up || down
    this->_speed = 10;
}
shepherd_dog::~shepherd_dog()
{
    std::cout << " The shepherd_dog died" << std::endl;
}

void shepherd_dog::interact(moving_object &obj)
{
    if (obj._type.compare("shepherd") == 0)
    {
        shepherd_x = obj._pos_x;
        shepherd_y = obj._pos_y;
    }
}
void shepherd_dog::move()
{
    if (shepherd_x != 0 && shepherd_y != 0)
    {
        int xdiff = this->_pos_x - shepherd_x;
        int ydiff = this->_pos_y - shepherd_y;
        int distance = sqrt(xdiff * xdiff + ydiff * ydiff);
        if (distance < dis_fr_shepherd)
        {
            float angle = atan2(ydiff, xdiff) * (180 / PI);
            std::cout << distance << "   " << angle << "\n";

            this->_pos_x += this->_speed * cos(angle);
            this->_pos_y += this->_speed * sin(angle);
            std::cout << _pos_x << "  " << _pos_y << "\n";
        }
        else
        {
            this->_pos_x = cos(spin_speed) * (xdiff)-sin(spin_speed) * (ydiff)
                + shepherd_x;
            this->_pos_y = sin(spin_speed) * (xdiff) + cos(spin_speed) * (ydiff)
                + shepherd_y;
        }
    }
}

//---------------------- shepherd class ------------------------------
shepherd::shepherd(SDL_Surface *window_surface_ptr)
    : moving_object("../media/Shepherd.png", window_surface_ptr)
{
    this->_height = 89;
    this->_width = 60;
    this->_type = "shepherd";
    this->_pos_x = std::rand() % 10 + 100; // right  || left
    this->_pos_y = std::rand() % 10 + 400; // up || down
    this->_speed = 10;
    std::cout << "new player created \n";
}
shepherd::~shepherd()
{
    std::cout << " player died !!!" << std::endl;
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
        if (_pos_y + _height + _speed < frame_height)
            _pos_y += _speed;
        break;
    case 'l':
        if (_pos_x - _speed > 0)
            _pos_x -= _speed;
        break;
    case 'r':
        if (_pos_x + _width + _speed <= frame_width)
            _pos_x += _speed;
        break;
    }
}
//---------------------- ground -------------------------------
ground::ground(SDL_Surface *window_surface_ptr)
{
    window_surface_ptr_ = window_surface_ptr;
    _shepherd = std::make_unique<shepherd>(window_surface_ptr_);
}
void ground::add_animal(unsigned n_sheep, unsigned n_wolf)
{
    _n_sheep = n_sheep;
    _n_wolf = n_wolf;
    animals.resize(_n_wolf + 1 + _n_sheep);
    // add the dog
    animals[0] = std::make_unique<shepherd_dog>(window_surface_ptr_);
    // add sheeps
    for (unsigned i = 1; i <= _n_sheep; i++)
        animals[i] = std::make_unique<sheep>(window_surface_ptr_);
    // add wolfs
    for (unsigned i = _n_sheep + 1; i < _n_wolf + 1 + _n_sheep; i++)
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
        if (animals[i])
            animals[i]->draw();
    _shepherd->draw();
}
void ground::update()
{
    // interacting between animals
    for (unsigned i = 0; i < animals.size(); i++)
        for (unsigned j = 0; j < animals.size(); j++)
            if (i != j && animals[i] && animals[j])
            {
                animals[i]->interact(*animals[j]);
            }

    animals[0]->interact(*_shepherd);

    // updating the position and existance of the animal
    for (unsigned i = 0; i < animals.size(); i++)
    {
        if (animals[i])
        {
            if (!animals[i]->alive)
                animals[i] = nullptr;
            else
                animals[i]->move();
        }
    }

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
                case SDL_MOUSEBUTTONUP:
                    // get position
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
