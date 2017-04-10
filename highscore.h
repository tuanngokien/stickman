#ifndef highscore_h
#define highscore_h

#include <fstream>

class highscore {
private:
    char name[10];
    int score;
public:
    highscore();
    int getScore();
    std::string getName();
    void setInfo(std::string name,int score);
};

#endif // highscore_h
