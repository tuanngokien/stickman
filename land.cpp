#include <SDL.h>
#include <SDL_image.h>
#include <cstdlib>
#include "land.h"

#define landHeight 176

land::land(){
    landX=0;//default size of the land image is 104*176
    landY=400-landHeight;// default size of the window is 400*400
    currentLand=0;
}

land::~land(){
    landX=0;
    landY=0;
    std::vector <SDL_Rect>().swap(landInfo);
    std::vector <SDL_Rect>().swap(pfPos);
    currentLand=0;
}

void land::randomLand(){
    //get random the width of each land and the distance to the next
    int landWidth[50], landDistance[50],pfPosX[50];
    for(int i=currentLand;i<50;i++){
        if(i>0){
            landWidth[i]=rand()%60+15;
            pfPosX[i]=rand()%(landWidth[i]-10)+2;
        }
        else
            landWidth[i]=100;
    }
    for(int i=currentLand;i<50;i++)
        landDistance[i]=rand()%130+25;
    SDL_Rect temp_rect;
    SDL_Rect temp_pf;
    for(int i=currentLand;i<50;i++){
        temp_rect={landX,landY,landWidth[i],landHeight};
        landInfo.push_back(temp_rect);
        temp_pf={landX+pfPosX[i],landY,5,4};
        pfPos.push_back(temp_pf);
        landX+=landWidth[i]+landDistance[i];
    }
}
void land::updateLandInfo(){
    int landUpdate=landInfo[1].x;
    for(int i=1;i<50;i++){
            landInfo[i].x-=landUpdate;
            pfPos[i].x-=landUpdate;
    }
    landInfo.erase(landInfo.begin());
    pfPos.erase(pfPos.begin());
    currentLand++;
    if(currentLand>=40){
        currentLand=0;
        land::randomLand(); //random next 50 land
    }
}

SDL_Rect land::getLandInfo(int index){
    return landInfo[index];
}

SDL_Rect land::getPFinfo(int index){
    return pfPos[index];
}

int land::getX(int index){
    return landInfo[index].x;
}

int land::getWidth(int index){
    return landInfo[index].w;
}

void land::resetLand(){
    currentLand=0;
    landX=0;
    landY=400-landHeight;
    std::vector <SDL_Rect>().swap(landInfo);
    std::vector <SDL_Rect>().swap(pfPos);
    land::randomLand();
}

