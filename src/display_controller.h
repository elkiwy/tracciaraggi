#ifndef __DISPLAY_CONTROLLER_H_
#define __DISPLAY_CONTROLLER_H_



#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif




const int SCREEN_FPS = 30;
const int SCREEN_TICKS_PER_FRAME = 1000/SCREEN_FPS;


class Scene;


using namespace std;

class DisplayController{
    public:
        //Singleton constructor/getter
        static DisplayController& getInstance(){static DisplayController instance; return instance;}
        //Default method deletion to avoid accidental copies
        DisplayController(DisplayController const&) = delete;
        void operator=(DisplayController const&) = delete;

        //Game Events
        void init(int w, int h);
        void update();
        void close();

        //Features
        uint32_t* getPixelDataPtr();

        //Input events
        //void keyPressed(const KeyCode k);
        //void onKeyDown(const uint8_t* keyStates);
        //void onMousePress(const MouseButtonEvent& button);
        //void onScrollWheel(float offset);
        //point2 getMouseScreenPosition();

        //Rendering Events
        void draw();
        void drawImGui();
        void beginFrame();
        void endFrame();

    private:
        DisplayController(){}

        SDL_Window* _window = NULL;
        SDL_Renderer* _renderer = NULL;
        SDL_Texture* _renderTexture = NULL;
        uint32_t* _pixelData = NULL;

        int _res_width;
        int _res_height;
};



/*
* Display Events
 */

void DisplayController::init(int w, int h){
    //Init sdl and window
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    _res_width = w;
    _res_height = h;

    SDL_WindowFlags flags = (SDL_WindowFlags) (SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    _window = SDL_CreateWindow("TracciaRaggi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _res_width, _res_height, flags);
    if(_window == NULL) {printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() ); return;}


    _renderer = SDL_CreateRenderer(_window, 0, 0);
    _renderTexture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, _res_width, _res_height);

    _pixelData = new uint32_t[_res_width*_res_height];


    //Make the starting pixels white
    //memset(_pixelData, 255, RES_WIDTH * RES_HEIGHT * sizeof(uint32_t));
}

void DisplayController::update(){
}

void DisplayController::close(){
    delete[] _pixelData;
    SDL_DestroyTexture(_renderTexture);
    SDL_DestroyRenderer(_renderer);

    SDL_DestroyWindow(_window);
    _window = NULL;
    SDL_Quit();
}

uint32_t* DisplayController::getPixelDataPtr(){
    return _pixelData;
}


/*
* Rendering Events
 */

void DisplayController::draw(){
    SDL_UpdateTexture(_renderTexture, NULL, _pixelData, _res_width*sizeof(uint32_t));

    SDL_RenderClear(_renderer);
    SDL_RenderCopyEx(_renderer, _renderTexture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
    SDL_RenderPresent(_renderer);
}

void DisplayController::drawImGui(){
}







#endif // __DISPLAY_CONTROLLER_H_
