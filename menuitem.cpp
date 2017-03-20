#include "menuitem.h"

menuItem::menuItem(int x,int y,int width,int height){
    this->posX=x;
    this->posY=y;
    this->width=width;
    this->height=height;
}

bool menuItem::checkArea(int x, int y){
    if(x>=posX && x<=posX+width)
        if(y>=posY && y<=posY+height)
            return true;
    return false;
}
