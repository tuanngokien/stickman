#include <fstream>
#include "highscore.h"
#include <cstring>
#include <iostream>

 highscore::highscore(){
    name[0]='\0';
    score=0;
 }

 int highscore::getScore(){
    return score;
 }

 std::string highscore::getName(){
    std::string s=std::string(name);
    return s;
 }

void highscore::setInfo(std::string name_,int score){
    strcpy(name,name_.c_str());
    this->score=score;
}

void highscore::showInfo(){
    std::cout<<"Name: "<<name<<" -"<<score<<std::endl;
}
