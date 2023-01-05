// SDL_Test.cpp: Include file for include standard system files
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
        // auto surf = IMG_Load(path.c_str());
        auto surf = SDL_LoadBMP(path.c_str());
        if (!surf)
            throw std::runtime_error("Could not load image");

        auto op_surface_ptr =
            SDL_ConvertSurface(surf, window_surface_ptr->format, 0);
        // make transparent
        SDL_SetColorKey(op_surface_ptr, SDL_TRUE,
                        SDL_MapRGB(op_surface_ptr->format, 0xFF, 0, 0xFF));
        if (op_surface_ptr == NULL)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(),
                   SDL_GetError());
        }
        SDL_FreeSurface(surf);
        return op_surface_ptr;
    }

    // generate a pos_x or pos_y, with speed = a_speed,
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
    // calculate the distance between two objs
    int get_dis_bet_2_objs(int pos_x_1, int pos_y_1, int pos_x_2, int pos_y_2)
    {
        int xdiff = pos_x_1 - pos_x_2;
        int ydiff = pos_y_1 - pos_y_2;
        return sqrt(xdiff * xdiff + ydiff * ydiff);
    }
    // calculate the angle between two objs in radians
    float get_angle_bet_2_objs_rad(int pos_x_1, int pos_y_1, int pos_x_2,
                                   int pos_y_2)
    {
        int xdiff = pos_x_1 - pos_x_2;
        int ydiff = pos_y_1 - pos_y_2;
        return atan2(ydiff, xdiff);
    }
    // calcul the next pos to get closer to or (far away from run_away = false)
    // the destination position
    void get_next_pos_to_des(int &pos_x, int &pos_y, int des_x, int des_y,
                             int speed, int a_height, int a_width,
                             bool run_away = false)
    {
        float angle = get_angle_bet_2_objs_rad(pos_x, pos_y, des_x, des_y);
        int direc = -1; // go
        if (run_away)
            direc = 1; // run away
        int next_x = pos_x + direc * speed * cos(angle);
        int next_y = pos_y + direc * speed * sin(angle);
        if (next_x > 0 && next_x + a_width < frame_width)
            pos_x = next_x;
        if (next_y > 0 && next_y + a_height < frame_height)
            pos_y = next_y;
    }
    // calcul the next pos to go around a center
    // clockwise is rotation's direction
    // speed is in rad (0 to 1)
    void get_next_pos_around_cent(int &pos_x, int &pos_y, int cent_x,
                                  int cent_y, double speed, int height,
                                  int width, bool &clockwise)
    {
        int xdiff = pos_x - cent_x;
        int ydiff = pos_y - cent_y;
        // clockwise rotation
        pos_x = cos(speed) * (xdiff)-sin(speed) * (ydiff) + cent_x;
        pos_y = sin(speed) * (xdiff) + cos(speed) * (ydiff) + cent_y;
        if (pos_x < 0 || pos_x + width > frame_width || pos_y < 0
            || pos_y + height > frame_height)
            clockwise = !clockwise;
        if (!clockwise)
        {
            // counter clockwise rotation
            pos_x = cos(speed) * (xdiff) + sin(speed) * (ydiff) + cent_x;
            pos_y = -sin(speed) * (xdiff) + cos(speed) * (ydiff) + cent_y;
        }
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
    : animal("../media/sheep.bmp", window_surface_ptr)
{
    this->_height = 55;
    this->_width = 55;
    this->_type = "sheep";
    this->_pos_x = std::rand() % 50 + 300; // right  || left
    this->_pos_y = std::rand() % 50 + 200; // up || down
    this->_speed = sheep_normal_speed; // it's just the speed of the sheep
    std::string _genre = std::rand() % 2 == 0 ? "m" : "f";
    _props.insert(std::make_pair("genre", _genre));
    _props["offspring_counter"] = sheep_offspring_counter;
    _props["offspring"] = false;
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
    int distance = get_dis_bet_2_objs(_pos_x, _pos_y, obj._pos_x, obj._pos_y);
    bool wolf_nearby = false;
    if (obj._type.compare("wolf") == 0)
    {
        if (distance < sheep_danger_dis + _width)
        {
            wolf_x = obj._pos_x;
            wolf_y = obj._pos_y;
            wolf_nearby = true;
        }
    }
    if (obj._type.compare("sheep") == 0 && distance < sheep_offspring_dis)
    {
        std::string genre = std::any_cast<std::string>(_props["genre"]);
        std::string obj_genre = std::any_cast<std::string>(obj._props["genre"]);
        if (genre.compare(obj_genre) != 0)
            if (obj_genre.compare("f") == 0)
            {
                if (std::any_cast<int>(obj._props["offspring_counter"]) == 0)
                    obj._props["offspring"] = true;
            }
            else
            {
                if (std::any_cast<int>(_props["offspring_counter"]) == 0)
                    _props["offspring"] = true;
            }
    }
    wolfs_nearby.push_back(wolf_nearby);
}

void sheep::move()
{
    // if there are a wolf nearby then run !!!
    if (std::binary_search(wolfs_nearby.begin(), wolfs_nearby.end(), true))
    {
        _speed += sheep_addition_speed;
        get_next_pos_to_des(_pos_x, _pos_y, wolf_x, wolf_y, _speed, _height,
                            _width, true);
        wolfs_nearby.clear();
    }
    else
    {
        _speed = sheep_normal_speed;
        _pos_y = get_ran_pos(_pos_y, _speed, _height, frame_height);
        _pos_x = get_ran_pos(_pos_x, _speed, _width, frame_width);
    }
    int offspr_count = std::any_cast<int>(_props["offspring_counter"]);
    _props["offspring_counter"] = offspr_count > 0 ? offspr_count - 1 : 0;
}

// -------------------------------- wolfs class-----------

wolf::wolf(SDL_Surface *window_surface_ptr)
    : animal("../media/wolf.bmp", window_surface_ptr)
{
    std::srand(std::time(nullptr));
    this->_height = 42;
    this->_width = 62;
    this->_type = "wolf";
    this->_pos_x = std::rand() % 50 + 1000; // right  || left
    this->_pos_y = std::rand() % 50 + 400; // up || down
    this->_speed = 10;
    _props["search_sheep_dis"] = wolf_start_search_sheep_dis;
    _props["hunger_count"] = wolf_hunger_count;
    _props["run_away"] = false;
}
wolf::~wolf()
{
    std::cout << " A wolf died" << std::endl;
}
void wolf::interact(moving_object &obj)
{
    int distance = get_dis_bet_2_objs(_pos_x, _pos_y, obj._pos_x, obj._pos_y);
    int search_dis = std::any_cast<int>(_props["search_sheep_dis"]);
    if (obj._type.compare("sheep") == 0)
    {
        if (distance < search_dis) // look for the closest sheep
        {
            sheep_x = obj._pos_x;
            sheep_y = obj._pos_y;
            _props["search_sheep_dis"] = distance;
        }
        if (distance < wolf_eat_dis) // eat sheep
        {
            obj.alive = false;
            // reset hunger count
            _props["hunger_count"] = wolf_hunger_count;
            // reset search distance
            _props["search_sheep_dis"] = wolf_start_search_sheep_dis;
            sheep_x = 0;
            sheep_y = 0;
        }
    }
    else if (obj._type.compare("shepherd_dog") == 0)
    {
        _props["run_away"] = false;
        if (distance < wolf_danger_dis)
        {
            dog_x = obj._pos_x;
            dog_y = obj._pos_y;
            _props["run_away"] = true;
        }
    }
}
void wolf::move()
{
    if (std::any_cast<bool>(_props["run_away"])) // avoiding dog
    {
        get_next_pos_to_des(_pos_x, _pos_y, dog_x, dog_y, _speed, _height,
                            _width, true);
    }
    else if (sheep_x != 0 && sheep_y != 0) // hunting for sheeps
    {
        get_next_pos_to_des(_pos_x, _pos_y, sheep_x, sheep_y, _speed, _height,
                            _width);
        _props["hunger_count"] =
            std::any_cast<unsigned>(_props["hunger_count"]) - 1;
        // reset distance
        _props["search_sheep_dis"] = wolf_start_search_sheep_dis;
    }
    else
    {
        _pos_x = get_ran_pos(_pos_x, _speed, _width, frame_width);
        _pos_y = get_ran_pos(_pos_y, _speed, _height, frame_height);
    }
    alive = std::any_cast<unsigned>(_props["hunger_count"]) != 0 ? true : false;
}
// -------------------------------- shepherd_dog class --------------

shepherd_dog::shepherd_dog(SDL_Surface *window_surface_ptr)
    : animal("../media/Shepherd_dog.bmp", window_surface_ptr)
{
    this->_height = 46;
    this->_width = 60;
    this->_type = "shepherd_dog";
    this->_pos_x = std::rand() % 10 + 200; // right  || left
    this->_pos_y = std::rand() % 10 + 350; // up || down
    this->_speed = 10;
    _props["order_x"] = 0;
    _props["order_y"] = 0;
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
        int order_x = std::any_cast<int>(obj._props["order_x"]);
        int order_y = std::any_cast<int>(obj._props["order_y"]);
        if (_pos_x == order_x && _pos_y == order_y)
        {
            obj._props["order_x"] = 0;
            obj._props["order_x"] = 0; // reset to default
        }
        _props["order_x"] = obj._props["order_x"];
        _props["order_y"] = obj._props["order_y"];
    }
}
void shepherd_dog::move()
{
    int order_x = std::any_cast<int>(_props["order_x"]);
    int order_y = std::any_cast<int>(_props["order_y"]);
    if (order_x != 0 && order_y != 0) // order
    {
        if (get_dis_bet_2_objs(_pos_x, _pos_y, order_x, order_y)
            < dog_dis_fr_order)
        {
            this->_pos_x = order_x;
            this->_pos_y = order_y;
        }
        else
            get_next_pos_to_des(_pos_x, _pos_y, order_x, order_y, _speed,
                                _height, _width);
    }
    else if (shepherd_x != 0 && shepherd_y != 0)
    {
        int distance =
            get_dis_bet_2_objs(_pos_x, _pos_y, shepherd_x, shepherd_y);
        if (distance > dog_dis_fr_shepherd)
            get_next_pos_to_des(_pos_x, _pos_y, shepherd_x, shepherd_y, _speed,
                                _height, _width);
        else
            get_next_pos_around_cent(this->_pos_x, _pos_y, shepherd_x,
                                     shepherd_y, dog_spin_speed, _height,
                                     _width, clockwise);
        // this->go_around_shepherd();
    }
}

//---------------------- shepherd class ------------------------------
playable::playable(const std::string &file_path,
                   SDL_Surface *window_surface_ptr)
    : moving_object(file_path, window_surface_ptr)
{
    std::cout << "Initialization playable character \n";
}

shepherd::shepherd(SDL_Surface *window_surface_ptr)
    : playable("../media/Shepherd.bmp", window_surface_ptr)
{
    this->_height = 89;
    this->_width = 60;
    this->_type = "shepherd";
    this->_pos_x = std::rand() % 10 + 100; // right  || left
    this->_pos_y = std::rand() % 10 + 400; // up || down
    this->_speed = 10;
    _props["order_x"] = 0;
    _props["order_y"] = 0;
    std::cout << "new player created \n";
}
shepherd::~shepherd()
{
    std::cout << " player died !!!" << std::endl;
}
void shepherd::get_order(int pos_x, int pos_y)
{
    _props["order_x"] = pos_x;
    _props["order_y"] = pos_y;
}
void shepherd::move(char direction)
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
    auto surf = load_surface_for("../media/farm.bmp", window_surface_ptr_);

    if (SDL_BlitSurface(surf, NULL, window_surface_ptr_, &dst_rect))
        throw std::runtime_error("Could not apply texture.");

    for (unsigned i = 0; i < animals.size(); i++)
        if (animals[i])
            animals[i]->draw();
    _shepherd->draw();
}
void ground::offspring_check(unsigned index)
{
    unsigned i = index;
    if (animals[i]->_props.find("offspring") != animals[i]->_props.end())
    {
        if (std::any_cast<bool>(animals[i]->_props["offspring"]))
        {
            // reset offsrping counters
            animals[i]->_props["offspring"] = false;
            animals[i]->_props["offspring_counter"] = sheep_offspring_counter;
            // add new baby
            animals.resize(animals.size() + 1);
            animals[animals.size() - 1] =
                std::make_unique<sheep>(window_surface_ptr_);
            animals[animals.size() - 1]->_pos_x = animals[i]->_pos_x;
            animals[animals.size() - 1]->_pos_y = animals[i]->_pos_y;
        }
    }
}

void ground::update()
{
    // interacting between animals and updating
    for (unsigned i = 0; i < animals.size(); i++)
    {
        if (animals[i])
        {
            if (!animals[i]->alive)
                animals[i] = nullptr;
            else
            {
                for (unsigned j = 0; j < animals.size(); j++)
                    if (i != j && animals[j])
                    {
                        animals[i]->interact(*animals[j]);
                        animals[i]->interact(*_shepherd);
                    }
                offspring_check(i);
                animals[i]->move();
            }
        }
    }
    draw();
}

// application
application::application(unsigned n_sheep, unsigned n_wolf)
{
    window_ptr_ = SDL_CreateWindow("Project SDL EPITA", SDL_WINDOWPOS_CENTERED,
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
                case SDLK_UP:
                    direction = 'u';
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    direction = 'd';
                    break;
                case SDLK_q:
                case SDLK_LEFT:
                    direction = 'l';
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    direction = 'r';
                    break;
                }
            }
            if (window_event_.type == SDL_MOUSEBUTTONUP)
                _ground->_shepherd->get_order(window_event_.button.x,
                                              window_event_.button.y);

            if (window_event_.type == SDL_KEYUP)
                direction = 0;
        }
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
