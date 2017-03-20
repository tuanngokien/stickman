#ifndef menuitem_h
#define menuitem_h

class menuItem{
private:
    int posX;
    int posY;
    int width;
    int height;
public:
    menuItem(int x,int y,int width,int height);
    bool checkArea(int x,int y);
};

#endif // menuitem_h
