#include <SDL.h>
#include "stickman.h"
#include "land.h"

stickman::stickman(): posX(10),posY(224){};

void stickman::setPosition(int x,int y){
    this->posX=x;
    this->posY=y;
}

void stickman::updateStickPosition(land Land){
    posX=Land.getWidth(0)-5;
}

int stickman::getX(){
    return posX;
}

int stickman::getY(){
    return posY;
}
