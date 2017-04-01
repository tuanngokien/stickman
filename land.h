#ifndef land_h
#define land_h

#include<vector>

class land{
private:
    int landX;
    int landY;
    std::vector <SDL_Rect> landInfo;
    std::vector <SDL_Rect> pfPos; //perfect position +20 points
    int currentLand;
public:
    land();
    ~land();
    void randomLand();
    void updateLandInfo();
    SDL_Rect getLandInfo(int index);
    SDL_Rect getPFinfo(int index);
    int getX(int index);
    int getWidth(int index);
    void resetLand();
};

#endif

