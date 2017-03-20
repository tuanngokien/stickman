#include <SDL.h>
#include <iostream>
#include <SDL_image.h>
#include "gameRender.h"
#include <cstdlib>
#include <SDL_ttf.h>
#include <string.h>

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
    font=nullptr;
    width=0;
    height=0;
}
void gameRender::setPosition(int x,int y){
    this->x=x;
    this->y=y;
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

void gameRender::setColor(SDL_Color color_){
    this->color=color_;
}

void gameRender::setSize(int height,int width){
    this->height=height;
    this->width=width;
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
        font=TTF_OpenFont("NKT.ttf",size);
     if(font==nullptr)
        std::cout<<"Failed to load font"<<TTF_GetError()<<std::endl;
     else{
        surface=TTF_RenderText_Solid(font,showscore.c_str(),color);
        texture=SDL_CreateTextureFromSurface(renderer,surface);
     }
     SDL_FreeSurface(surface);
}

int mypow(int x,int n){
    if(n==0)
        return 1;
    else
        return x*mypow(x,n-1);
}
std::string intToString(int integer){
    char number[10];
    std::string s;
    int index=0,charCount=0,integerTemp=integer;
    if(integer!=0){
        while(integerTemp){
            integerTemp/=10;
            charCount++;
        }
        while(index<charCount){
            number[index]=integer/mypow(10,charCount-1-index)+48;
            integer%=mypow(10,charCount-1-index);
            index++;
        }
        number[index]='\0';
        s=std::string(number);
    }
    else
        s="0";
    return s;
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
        font=TTF_OpenFont("NKT.ttf",size);
     if(font==nullptr)
        std::cout<<"Failed to load font"<<TTF_GetError()<<std::endl;
     else{
        surface=TTF_RenderText_Solid(font,showstring.c_str(),color);
        texture=SDL_CreateTextureFromSurface(renderer,surface);
     }
     SDL_FreeSurface(surface);
}

