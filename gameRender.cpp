#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include <cstdlib>
#include <SDL_ttf.h>
#include <sstream>

#include "gameRender.h"

gameRender::gameRender(SDL_Renderer* renderer_){
    renderer=renderer_;
    texture=nullptr;
    font=nullptr;
    width=785;
    height=1200;
    x=0;
    y=0;
}
gameRender::~gameRender(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void gameRender::render(int x,int y,SDL_Rect* dstrect,SDL_Rect* srcrect){
    if(dstrect==nullptr && srcrect==nullptr){
        SDL_Rect temp={x,y,width,height};
        SDL_RenderCopy(renderer,texture,nullptr,&temp);
    }
    else if(srcrect==nullptr)
        SDL_RenderCopy(renderer,texture,nullptr,dstrect);
    else
        SDL_RenderCopy(renderer,texture,srcrect,dstrect);
}

void gameRender::loadImage(std::string path,SDL_Color* colorKey ){
    SDL_Surface* surface=nullptr;
    bool colorKeyuse=false;
    if(colorKey!=nullptr)
        colorKeyuse=true;
    surface=IMG_Load(path.c_str());
    if(surface==nullptr)
        std::cout<<"LOADED FAILED"<<" "<<IMG_GetError()<<std::endl;
    else{
        if(colorKeyuse){
            SDL_SetColorKey(surface,SDL_TRUE,SDL_MapRGB(surface->format,colorKey->r,colorKey->g,colorKey->b));
            texture=SDL_CreateTextureFromSurface(renderer,surface);
        }
        else
            texture=SDL_CreateTextureFromSurface(renderer,surface);
    }
    SDL_FreeSurface(surface);
}

int gameRender::getWidth(){
    return width;
}

int gameRender::getHeight(){
    return height;
}

void gameRender::loadFont(int size,SDL_Color color,int score){
     std::string showscore=intToString(score);
     SDL_Surface* surface=nullptr;
     TTF_Init();
     if(font==nullptr)
        font=TTF_OpenFont("carbon.ttf",size);
     if(font==nullptr)
        std::cout<<"Failed to load font"<<TTF_GetError()<<std::endl;
     else{
        surface=TTF_RenderText_Solid(font,showscore.c_str(),color);
        texture=SDL_CreateTextureFromSurface(renderer,surface);
     }
     SDL_FreeSurface(surface);
}

std::string gameRender::intToString(int number)
{
    std::stringstream ss;
    ss << number;
    std::string str = ss.str();
    return str;
}

bool gameRender::waitMouseDown(SDL_Event &event){
      if(event.type==SDL_MOUSEBUTTONDOWN)
        return true;
      return false;
}

void gameRender::loadString(int size,SDL_Color color,std::string s){
     std::string  showstring=s;
     SDL_Surface* surface=nullptr;
     TTF_Init();
     if(font==nullptr)
        font=TTF_OpenFont("carbon.ttf",size);
     if(font==nullptr)
        std::cout<<"Failed to load font"<<TTF_GetError()<<std::endl;
     else{
        surface=TTF_RenderText_Solid(font,showstring.c_str(),color);
        texture=SDL_CreateTextureFromSurface(renderer,surface);
     }
     SDL_FreeSurface(surface);
}

void  gameRender::setTextureSize(int& widthx,int& heightx){
    SDL_QueryTexture(texture,NULL,NULL,&widthx,&heightx);
}
