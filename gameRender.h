#ifndef gameRender_h
#define gameRender_h

#include <SDL.h>
#include <SDL_ttf.h>

const SDL_Color CYAN = {0, 255, 255};
const SDL_Color BLUE = {0, 0, 255};
const SDL_Color ORANGE = {255, 165, 0};
const SDL_Color YELLOW = {255, 255, 0};
const SDL_Color LIME = {0, 255, 0};
const SDL_Color PURPLE = {128, 0, 128};
const SDL_Color RED = {255, 0, 0};
const SDL_Color WHITE = {255, 255, 255};
const SDL_Color BLACK = {0, 0, 0};
const SDL_Color GREEN = {0, 128, 0};

const SDL_Color DEFAULT_Color = BLACK;


class gameRender{
private:
    int x;
    int y;
    int width;
    int height;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    TTF_Font *font;
public:
    gameRender(SDL_Renderer* renderer_);
    ~gameRender();
    int getWidth();
    int getHeight();
    void render(int x,int y,SDL_Rect* dstrect =nullptr,SDL_Rect* srcrect =nullptr);
    void loadImage(std::string path,SDL_Color* colorKey =nullptr);
    void loadFont(int size,SDL_Color color,int score);
    void loadString(int size,SDL_Color color,std::string s);
    static bool waitMouseDown(SDL_Event &event);
    void setTextureSize();
};

//addon
std::string intToString(int number);

#endif
