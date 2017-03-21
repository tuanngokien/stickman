#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <ctime>
#include <fstream>
#include <vector>

#include "gameRender.h"
#include "land.h"
#include "stickman.h"
#include "menuitem.h"
#include "highscore.h"

using namespace std;

int initStartGame(SDL_Window* window,SDL_Renderer* renderer,bool& exitGame);
void initMainGame(SDL_Window* window,SDL_Renderer* renderer,bool& exitGame);
void initHighScore(SDL_Window* window,SDL_Renderer* renderer,bool& exitGame);
void initInstruction(SDL_Window* window,SDL_Renderer* renderer,bool& exitGame);

void renderBackground(gameRender& renderBG,int& BGoffset,int score);
bool checkGameOver(int stickLenght,land Land,stickman stickMan);
bool delay(int timeDelay);
int getScoreSize(int score);
SDL_Rect getRectManImage(int id);
SDL_Rect getRectBGImage(int score);
void renderStickMan(gameRender& renderMan,stickman& Man,bool& runningStatus,int& BGoffset);
bool playAgain();
bool pauseCheck(int x,int y);
string getInfo(SDL_Renderer* renderer,gameRender& renderHighScore);


int main(int argc,char** argv){
    srand(time(0));
    SDL_Window* window=nullptr;
    SDL_Renderer* renderer=nullptr;
    bool exit=false;
    window=SDL_CreateWindow("STICKMAN - NKT ",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,400,400,0);
    while(!exit){
        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        switch(initStartGame(window,renderer,exit)){
        case 1: renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                initMainGame(window,renderer,exit);
                break;
        case 2: renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                initHighScore(window,renderer,exit);
                break;
        case 3: exit=true;
                break;
        case 4:renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                initInstruction(window,renderer,exit);
                break;
        }
    }
    return 0;
}

bool checkGameOver(int stickLenght,land Land,stickman stickMan){
    SDL_Rect rect_temp=Land.getLandInfo(1);
    if(stickMan.getX()+stickLenght>rect_temp.x+1 && stickMan.getX()+stickLenght<=rect_temp.x+Land.getWidth(1))
        return 0;
    else
        return 1;
}

bool delay(int timeDelay){
    /* timeDelay*10 mlsecond*/
    static int delayCount=1;
    delayCount++;
    if(timeDelay==0){
        delayCount=1;
        return 1;
    }
    if((delayCount-1)%timeDelay==0){
        delayCount=1;
        return 1;
    }
    return 0;
}

void renderStickMan(gameRender& renderMan,stickman& Man,bool& runningStatus,int& BGoffset){
    SDL_Rect manInImage,manInRender;
    static int positionRunning;
    if(!runningStatus){
        manInImage=getRectManImage(2);
        manInRender={Man.getX()-25,Man.getY(),76,76};
        positionRunning=0;
    }
    else{
        if(positionRunning%15<=6)
            manInImage=getRectManImage(0);
        else
            manInImage=getRectManImage(1);
        manInRender={Man.getX()+positionRunning,Man.getY(),76,76};
        positionRunning++;
        BGoffset--;
    }
    renderMan.render(0,0,&manInRender,&manInImage);
}

int getScoreSize(int score){
    int index=0;
    if(score==0)
        return 20;
    while(score){
        index++;
        score/=10;
    }
    return 20*index;
}

string getInfo(SDL_Renderer* renderer,gameRender& renderHighScore){
    gameRender renderFont(renderer);
    string s;
    SDL_Rect name={80,197,0,20};
    SDL_Rect HSrect={0,0,400,400};
    SDL_Rect save={110,193,96,30};
    SDL_Event event;
    bool quit=false;
    while(!quit){
         renderHighScore.render(0,0,&HSrect);
         renderFont.loadString(20,BLACK,s);
         renderFont.render(0,0,&name);
         while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT)
                quit=true;
            else if(event.type==SDL_KEYDOWN){
                if(event.key.keysym.sym==SDLK_BACKSPACE && s.length()>0){
                    s.pop_back();
                    name.w-=20;
                }
                else if(event.key.keysym.sym==SDLK_v && SDL_GetModState() & KMOD_CTRL){
                    s=SDL_GetClipboardText();
                    name.w=+s.length()*20;
                }
                else if(event.key.keysym.sym==SDLK_c && SDL_GetModState() & KMOD_CTRL)
                    SDL_SetClipboardText(s.c_str());
                else if(event.key.keysym.sym==SDLK_RETURN){
                     renderHighScore.render(0,0,&HSrect);
                     renderFont.loadString(24,RED,"Saved");
                     renderFont.render(0,0,&save);
                     SDL_RenderPresent(renderer);
                     return s;
                }
            }
            else if(event.type==SDL_TEXTINPUT && s.length()<8)
                if( !((event.text.text[0] =='c' || event.text.text[0]=='C')&& (event.text.text[0]=='v'||event.text.text[0] =='V')&&SDL_GetModState()&KMOD_CTRL )){
                    s+=event.text.text;
                    name.w+=20;
                }
            }
        SDL_RenderPresent(renderer);
    }
}

SDL_Rect getRectBGImage(int score){
    //get Rect from Image for each score(day change) status
    SDL_Rect rectBG[3];
    rectBG[0]={0,0,785,400};
    rectBG[1]={0,400,785,400};
    rectBG[2]={0,800,785,400};
    if(score>500)
        score%=500;
    if(score<=150)
        return rectBG[0];
    else if(score<=300)
        return rectBG[1];
    else
        return rectBG[2];
}

void renderBackground(gameRender& renderBG,int& BGoffset,int score){
    if(BGoffset<-renderBG.getWidth())
            BGoffset=0;
    SDL_Rect srcRect=getRectBGImage(score);
    SDL_Rect dstRect1={BGoffset,0,renderBG.getWidth(),400},dstRect2{BGoffset+renderBG.getWidth(),0,renderBG.getWidth(),400};
    renderBG.render(0,0,&dstRect1,&srcRect);
    renderBG.render(0,0,&dstRect2,&srcRect);
}

SDL_Rect getRectManImage(int id){
    //get Rect from Image for each man status
    SDL_Rect Man[4];
    Man[0]={0,0,79,73};
    Man[1]={78,0,79,73};
    Man[2]={162,0,79,73};
    Man[3]={258,0,73,73};
    return Man[id];
}

bool playAgain(){
    SDL_Event event;
    menuItem buttonMenu(112,165,66,59);
    menuItem buttonAgain(200,166,66,59);
    while(true){
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT)
                return 0;
            if(event.type==SDL_MOUSEBUTTONDOWN){
                if(buttonAgain.checkArea(event.button.x,event.button.y))
                    return 1;
                else if(buttonMenu.checkArea(event.button.x,event.button.y))
                    return 0;
            }
        }
    }
}

bool pauseCheck(int x,int y){
    menuItem buttonResume={360,10,30,30};
    if(buttonResume.checkArea(x,y))
        return 1;
    return 0;
}

void initMainGame(SDL_Window* window, SDL_Renderer* renderer,bool& exitGame){
    //set icon window titlebar
    SDL_Surface* iconTitleBar=IMG_Load("image/icontitlebar.png");
    SDL_SetWindowIcon(window,iconTitleBar);
    //load background image-- BGoffset for scrolling BG
    gameRender renderBG(renderer);
    renderBG.loadImage("image/town.jpg");
    int BGoffset=0;
    //load land image, colorkey is white---- create land object to generate each land coordinate
    SDL_Color colorKey=WHITE;
    gameRender renderLand(renderer);
    renderLand.loadImage("image/land.png",&colorKey);
    land Land;
    Land.randomLand();
    SDL_Rect landTemp;
    //load stick image,colorkey is white(remove white from image)
    gameRender renderStick(renderer);
    renderStick.loadImage("image/stick.png");
    stickman stick;
    stick.setPosition(Land.getWidth(0)-5,224);
    int stickLength=0;
      //load imgage StickMan, colorkey is white(remove white from image)
    gameRender renderMan(renderer);
    renderMan.loadImage("image/prince.png",&colorKey);
    stickman Man;
    Man.setPosition(stick.getX()-30,stick.getY()-76);
    bool runningStatus=false,dead=false;
    //load Font and render score
    gameRender renderScore(renderer);
    int score=0;
    renderScore.loadFont(20,PURPLE,score);
    SDL_Rect scoreRect={185,10,20,30};
    //load gameOver menu
    gameRender renderGameOver(renderer);
    renderGameOver.loadImage("image/gameover.png",&colorKey);
    SDL_Rect gameOver={0,0,400,400};
    //load HighScore
    gameRender renderHighScore(renderer);
    renderHighScore.loadImage("image/GO_highscore.png",&colorKey);
//    SDL_Rect highScore={0,0,400,400};
    //load Pause button
    gameRender renderPause(renderer);
    renderPause.loadImage("image/pause.png",&colorKey);
    SDL_Rect pause={360,10,30,30};
    //load Resume button
    gameRender renderResume(renderer);
    renderResume.loadImage("image/resume.png",&colorKey);
    SDL_Rect resume={89,144,201,101};
    //load current highscore
    fstream scoreFile("hscore.dat",ios::app | ios::out | ios::in | ios::binary);
    highscore scoreTemp;
    while(!scoreFile.eof()){
        scoreFile.read((char*)&scoreTemp,sizeof(highscore));
    }
    int currHighScore=scoreTemp.getScore();
    string name;

    SDL_Event event;
    bool quit=false,MouseDown=false;

    while(!quit){
        renderBackground(renderBG,BGoffset,score);
        scoreRect.w=getScoreSize(score);
        renderScore.render(0,0,&scoreRect);
        renderPause.render(0,0,&pause);
        for(int i=0;i<10;i++){
            landTemp=Land.getLandInfo(i);
            renderLand.render(0,0,&landTemp);
        }
        if(!dead)
            renderStickMan(renderMan,Man,runningStatus,BGoffset);
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){
                quit=true;
                exitGame=true;
            }
            if(event.type==SDL_MOUSEBUTTONDOWN){
                if(pauseCheck(event.button.x,event.button.y)){
                    renderResume.render(0,0,&resume);
                    SDL_RenderPresent(renderer);
                        if(playAgain())
                            quit=true;
                }
            }
        }
        if(gameRender::waitMouseDown(event)&& !runningStatus && !dead){
            if(!(event.button.x> 360 && event.button.y<40) && stickLength<Land.getX(2)-Land.getWidth(0)-3 ){
                stickLength+=4;
                landTemp={stick.getX(),stick.getY()-stickLength,4,stickLength};
                renderStick.render(0,0,&landTemp);
                MouseDown=true;
            }
        }
        else if(MouseDown){
            if(!dead)
                landTemp={stick.getX(),stick.getY()-4,stickLength,4};
            renderStick.render(0,0,&landTemp);
            runningStatus=true;
            if(delay(stickLength)){
                if(checkGameOver(stickLength,Land,stick)){
                    landTemp={stick.getX()+5,stick.getY(),4,stickLength};
                    if(dead){
                        SDL_Rect ManInImage=getRectManImage(3),manInRender={Man.getX()+stickLength+10,300,73,73};
                        renderMan.render(0,0,&manInRender,&ManInImage);
                        renderStick.render(0,0,&landTemp);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(500);
                        if(score<=currHighScore)
                            renderGameOver.render(0,0,&gameOver);
                        else{
                            name=getInfo(renderer,renderHighScore);
                            scoreTemp.setInfo(name,score);
                            ofstream fileWrite("hscore.dat",ios::app | ios::binary);
                            fileWrite.write((char*)&scoreTemp,sizeof(highscore));
                            SDL_Delay(1000);
                            break;
                        }
                        SDL_RenderPresent(renderer);
                        if(playAgain()){
                                BGoffset=0;
                                stickLength=0;
                                score=0;
                                Land.resetLand();
                                MouseDown=false;
                                runningStatus=false;
                                stick.setPosition(Land.getWidth(0)-5,224);
                                Man.setPosition(stick.getX()-30,stick.getY()-76);
                                renderScore.loadFont(20,PURPLE,score);
                        }
                        else
                            quit=true;
                    }
                    if(!dead)
                        dead=true;
                    else
                        dead=false;
                }
                else{
                        //update
                        score+=10;
                        renderScore.loadFont(20,PURPLE,score);
                        stick.updateStickPosition(Land);
                        Man.setPosition(stick.getX()-30,stick.getY()-76);
                        Land.updateLandInfo();
                        stickLength=0;
                        MouseDown=false;
                        runningStatus=false;
                }
            }
        }
        SDL_Delay(10);
        if(!dead)
            SDL_RenderPresent(renderer);
    }
}

int initStartGame(SDL_Window* window,SDL_Renderer* renderer,bool& exitGame){
    gameRender renderStart(renderer);
    renderStart.loadImage("image/mainmenu.png");
    SDL_Rect BGrect={0,0,400,400};

    gameRender buttonStart(renderer);
    buttonStart.loadImage("image/start.png");
    SDL_Rect bStartRect={135,149,139,70};

    gameRender buttonScore(renderer);
    buttonScore.loadImage("image/highscore.png");
    SDL_Rect bScoreRect={137,232,139,70};

    gameRender buttonExit(renderer);
    buttonExit.loadImage("image/exit.png");
    SDL_Rect bExitRect={141,316,139,70};

    gameRender buttonInst(renderer);
    buttonInst.loadImage("image/instruction.png");
    SDL_Rect bInstRect={332,320,68,73};

    SDL_Rect defaultButton={0,0,139,70};
    menuItem start(137,151,139,70);
    menuItem highscore(137,232,139,70);
    menuItem exit(141,316,139,70);
    menuItem inst(332,320,68,73);

    SDL_Event event;
    while(true){
        while(SDL_PollEvent(&event)){
            renderStart.render(0,0,&BGrect);
            if(event.type==SDL_QUIT){
                return 3;
                exitGame=true;
            }
            if(start.checkArea(event.button.x,event.button.y)){
                buttonStart.render(0,0,&bStartRect,&defaultButton);
                if(event.type==SDL_MOUSEBUTTONDOWN)
                    return 1;
            }
            else if(highscore.checkArea(event.button.x,event.button.y)){
                buttonScore.render(0,0,&bScoreRect,&defaultButton);
                if(event.type==SDL_MOUSEBUTTONDOWN)
                    return 2;
            }
            else if(exit.checkArea(event.button.x,event.button.y)){
                buttonExit.render(0,0,&bExitRect,&defaultButton);
                if(event.type==SDL_MOUSEBUTTONDOWN){
                    return 3;
                    exitGame=true;
                }
            }
            else if(inst.checkArea(event.button.x,event.button.y)){
                buttonInst.render(0,0,&bInstRect);
                if(event.type==SDL_MOUSEBUTTONDOWN)
                    return 4;
            }
            SDL_Delay(20);
            SDL_RenderPresent(renderer);
        }
    }
}

void initHighScore(SDL_Window* window,SDL_Renderer* renderer,bool& exitGame){
    fstream scoreFile;
    menuItem buttonBack(0,345,52,52);
    highscore scoreTemp;
    string temp;
    SDL_Event event;
    gameRender scoreBoard(renderer);
    scoreBoard.loadImage("image/score.png");
    SDL_Rect scoreRect={0,0,400,400};
    scoreBoard.render(0,0,&scoreRect);
    SDL_Rect person[4];
    person[0]={131,115,170,30};
    person[1]={131,165,170,30};
    person[2]={131,220,170,30};
    person[3]={131,280,170,30};
    scoreFile.open("hscore.dat",ios::app | ios::out | ios::in | ios::binary);
    vector <highscore> listScore;
    while(!scoreFile.eof()){
        scoreFile.read((char*)&scoreTemp,sizeof(highscore));
        listScore.push_back(scoreTemp);
    }
    listScore.pop_back();
    vector <highscore>:: reverse_iterator read=listScore.rbegin();
//    while(read!=listScore.rend()){
//        (*read++).showInfo();
//    }
    for(int i=0;i<3;i++){
        temp=(*read).getName()+" - "+intToString((*read).getScore());
        scoreBoard.loadString(20,RED,temp);
        scoreBoard.render(0,0,&person[i]);
        read++;
    }
    SDL_RenderPresent(renderer);
    bool quit=false;
    while(!quit){
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){
                quit=true;
                exitGame=true;
            }
            else if(event.type==SDL_MOUSEBUTTONDOWN){
                if(buttonBack.checkArea(event.button.x,event.button.y))
                    quit=true;
            }
        }
    }
}

void initInstruction(SDL_Window* window,SDL_Renderer* renderer,bool& exitGame){
    gameRender gif1(renderer);
    gif1.loadImage("image/inst1.png");
    gameRender gif2(renderer);
    gif2.loadImage("image/inst2.png");
    gameRender gif3(renderer);
    gif3.loadImage("image/inst3.png");
    SDL_Rect defaultRect={0,0,400,400};
    int gifLoad=1;
    menuItem buttonBack(0,345,52,52);
    bool quit=false;
    SDL_Event event;
    while(!quit){
        if(gifLoad<=40)
            gif1.render(0,0,&defaultRect);
        else if(gifLoad<=80)
            gif2.render(0,0,&defaultRect);
        else if(gifLoad<=130)
            gif3.render(0,0,&defaultRect);
        else
            gifLoad=0;
        gifLoad++;
        SDL_RenderPresent(renderer);
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){
                quit=true;
                exitGame=true;
            }
            else if(event.type==SDL_MOUSEBUTTONDOWN){
                if(buttonBack.checkArea(event.button.x,event.button.y))
                    quit=true;
            }
        }
    }
}
