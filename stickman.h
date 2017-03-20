#ifndef stickman_h
#define stickman_h

#include <SDL.h>
#include "land.h"

class stickman{
private:
    int posX;
    int posY;
public:
    stickman();
    void setPosition(int x,int y);
    void updateStickPosition(land Land);
    int getX();
    int getY();
};

#endif // stickman_h
