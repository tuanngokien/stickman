#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <ctime>
#include <fstream>
#include <vector>

#include "gameRender.h"
#include "land.h"
#include "stickman.h"
#include "menuitem.h"
#include "highscore.h"

using namespace std;

bool exitGame=false;
bool soundOn=true;

int initStartGame(SDL_Window* window,SDL_Renderer* renderer);
void initMainGame(SDL_Window* window,SDL_Renderer* renderer);
void initHighScore(SDL_Window* window,SDL_Renderer* renderer);
void initInstruction(SDL_Window* window,SDL_Renderer* renderer);

SDL_Rect getRectBGImage(int score);
void renderBackground(gameRender& renderBG,int& BGoffset,int score);
bool checkGameOver(int stickLength,land Land,stickman stick);
bool checkPerfect(int stickLength,land Land,stickman stick);
SDL_Rect getRectManImage(int id);
void renderStickMan(gameRender& renderMan,Mix_Chunk * horseEffect,stickman& Man,bool& runningStatus,int& BGoffset,land Land,int scrollLand,bool gameOver);
bool playAgain();
bool pauseCheck(int x,int y);
string getInfo(SDL_Renderer* renderer,gameRender& renderHighScore);


int main(int argc,char** argv){
    srand(time(0));
    SDL_Window* window=nullptr;
    SDL_Renderer* renderer=nullptr;
    window=SDL_CreateWindow("STICKMAN - NKT ",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,400,400,0);
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );
    while(!exitGame){
        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        switch(initStartGame(window,renderer)){
        case 1: renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                initMainGame(window,renderer);
                break;
        case 2: renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                initHighScore(window,renderer);
                break;
        case 3: exitGame=true;
                break;
        case 4:renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                initInstruction(window,renderer);
                break;
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

bool checkGameOver(int stickLength,land Land,stickman stick){
    SDL_Rect rect_temp=Land.getLandInfo(1);
    if(stick.getX()+stickLength>rect_temp.x+2 && stick.getX()+stickLength<=rect_temp.x+Land.getWidth(1))
        return 0;
    else
        return 1;
}

bool checkPerfect(int stickLength,land Land,stickman stick){
    SDL_Rect rect_temp=Land.getPFinfo(1);
    if(stick.getX()+stickLength>rect_temp.x+2 && stick.getX()+stickLength<=rect_temp.x+7)
        return 1;
    else
        return 0;
}


void renderStickMan(gameRender& renderMan,Mix_Chunk* horseEffect,stickman& Man,bool& runningStatus,int& BGoffset,land Land,int scrollLand,bool gameOver){
    SDL_Rect manInImage,manInRender;
    static int positionRunning=0,fixed=1;
    if(!runningStatus){
        manInImage=getRectManImage(2);
        manInRender={Man.getX()-25,Man.getY(),76,76};
        fixed=0;
        positionRunning=0;
        Mix_HaltChannel(0);
    }
    else{
        if(positionRunning%15<=6)
            manInImage=getRectManImage(0);
        else
            manInImage=getRectManImage(1);
        if(scrollLand+Land.getX(1)+Land.getWidth(1)-Land.getWidth(0)-25>0)
            manInRender={Man.getX(),Man.getY(),76,76};
        else{
            if(!gameOver){
                manInRender={Man.getX()-fixed,Man.getY(),76,76};
                manInImage=getRectManImage(2);
                fixed++;
                Mix_HaltChannel(0);
            }
            else
                manInRender={Man.getX(),Man.getY(),76,76};
        }
        if(positionRunning==0 && soundOn)
            Mix_PlayChannel(0,horseEffect,0);
        positionRunning++;
        BGoffset--;
    }
    renderMan.render(0,0,&manInRender,&manInImage);
}


string getInfo(SDL_Renderer* renderer,gameRender& renderHighScore){
    gameRender renderFont(renderer);
    string s;
    SDL_Rect name={90,195,0,20};
    SDL_Rect HSrect={0,0,400,400};
    SDL_Rect save={130,193,96,30};
    SDL_Event event;
    bool quit=false;
    while(!quit){
         renderHighScore.render(0,0,&HSrect);
         renderFont.loadString(20,BLACK,s);
         renderFont.setTextureSize();
         name.h=renderFont.getHeight();
         name.w=renderFont.getWidth();
         renderFont.render(0,0,&name);
         while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT)
                quit=true;
            else if(event.type==SDL_KEYDOWN){
                if(event.key.keysym.sym==SDLK_BACKSPACE && s.length()>0){
                    s.pop_back();
                }
                else if(event.key.keysym.sym==SDLK_v && SDL_GetModState() & KMOD_CTRL){
                    name.w=+s.length()*20;
                    s=SDL_GetClipboardText();
                }
                else if(event.key.keysym.sym==SDLK_c && SDL_GetModState() & KMOD_CTRL){
                    SDL_SetClipboardText(s.c_str());
                }
                else if(event.key.keysym.sym==SDLK_RETURN){
                     renderHighScore.render(0,0,&HSrect);
                     renderFont.loadString(24,RED,"Saved");
                     renderFont.setTextureSize();
                     save.h=renderFont.getHeight();
                     save.w=renderFont.getWidth();
                     renderFont.render(0,0,&save);
                     SDL_RenderPresent(renderer);
                     return s;
                }
            }
            else if(event.type==SDL_TEXTINPUT && s.length()<11)
                if( !((event.text.text[0] =='c' || event.text.text[0]=='C')&& (event.text.text[0]=='v'||event.text.text[0] =='V')&&SDL_GetModState()&KMOD_CTRL )){
                    s+=event.text.text;
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

void initMainGame(SDL_Window* window, SDL_Renderer* renderer){
    //load music game effects
    Mix_HaltMusic();
    Mix_Music* gameMusic=Mix_LoadMUS("sound/maingame.wav");
    if(soundOn)
        Mix_PlayMusic(gameMusic,-1);
    Mix_Chunk* butt_press=Mix_LoadWAV("sound/button_pressed.wav");
    Mix_Chunk* horse=Mix_LoadWAV("sound/horse.wav");
    Mix_Chunk* perfect=Mix_LoadWAV("sound/perfect.wav");
    Mix_Chunk* over=Mix_LoadWAV("sound/gameover.wav");
    //load background image-- BGoffset for scrolling BG
    gameRender renderBG(renderer);
    renderBG.loadImage("image/town.jpg");
    int BGoffset=0,scrollLand=0;
    //load land image, colorkey is white---- create land object to generate each land coordinate
    SDL_Color colorKey=WHITE;
    gameRender renderLand(renderer);
    renderLand.loadImage("image/land.png",&colorKey);
    land Land;
    Land.randomLand();
    SDL_Rect landTemp;
    // load perfect point on the land
    gameRender renderPF(renderer);
    renderPF.loadImage("image/perfect.png");
    SDL_Rect pfTemp;
    gameRender PFnoti(renderer);
    SDL_Rect pfNoti={140,50,140,30};
    PFnoti.loadString(20,RED,"PERFECT!");
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
    SDL_Rect scoreRect={200,10,20,30};
    //load gameOver menu
    gameRender renderGameOver(renderer);
    renderGameOver.loadImage("image/gameover.png",&colorKey);
    SDL_Rect gameOverRect={0,0,400,400};
    int frame=0;
    //load HighScore
    gameRender renderHighScore(renderer);
    renderHighScore.loadImage("image/GO_highscore.png",&colorKey);
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
    bool quit=false,MouseDown=false,gameOver=false;

    while(!quit){
        renderBackground(renderBG,BGoffset,score);

        renderScore.setTextureSize();
        scoreRect.h=renderScore.getHeight();
        scoreRect.w=renderScore.getWidth();
        scoreRect.x=200-scoreRect.w/2;
        renderScore.render(0,0,&scoreRect);

        renderPause.render(0,0,&pause);

        for(int i=0;i<10;i++){
            landTemp=Land.getLandInfo(i);
            pfTemp=Land.getPFinfo(i);
            if(runningStatus || (gameOver && !runningStatus)){
                landTemp.x+=scrollLand;
                pfTemp.x+=scrollLand;
            }
            renderLand.render(0,0,&landTemp);
            renderPF.render(0,0,&pfTemp);
        }

        if(!dead)
            renderStickMan(renderMan,horse,Man,runningStatus,BGoffset,Land,scrollLand,gameOver);
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){
                quit=true;
                exitGame=true;
            }
            if(event.type==SDL_MOUSEBUTTONDOWN){
                if(pauseCheck(event.button.x,event.button.y)){
                    Mix_HaltChannel(0);
                    if(soundOn)
                        Mix_PlayChannel(-1,butt_press,0);
                    renderResume.render(0,0,&resume);
                    SDL_RenderPresent(renderer);
                        if(playAgain())
                            quit=true;
                        else{
                            if(soundOn)
                                Mix_PlayChannel(-1,horse,0);
                        }
                        if(soundOn)
                            Mix_PlayChannel(-1,butt_press,0);
                }
            }
        }
        if(gameRender::waitMouseDown(event)&& !runningStatus && !dead){
            if(!(event.button.x> 360 && event.button.y<40) && stickLength<Land.getX(2)-Land.getWidth(0)-3 ){
                stickLength+=4;
                MouseDown=true;
            }
            landTemp={stick.getX(),stick.getY()-stickLength,4,stickLength};
            renderStick.render(0,0,&landTemp);
        }
        else if(MouseDown){
            if(!dead){
                runningStatus=true;
                gameOver=checkGameOver(stickLength,Land,stick);
                if(!gameOver){
                    if(stick.getX()+scrollLand>=0)
                        landTemp={stick.getX()+scrollLand-2,stick.getY()-4,stickLength,4};
                    else if(Land.getX(1)+scrollLand>0)
                        landTemp={0,stick.getY()-4,stickLength+stick.getX()+scrollLand-2,4};
                    else
                        runningStatus=false;
                }
                else {
                    if(scrollLand+stickLength>=0)
                        landTemp={stick.getX()+scrollLand-2,stick.getY()-4,stickLength-2,4};
                    else
                        runningStatus=false;
                }
                    scrollLand--;
                    renderStick.render(0,0,&landTemp);
            }
            if(!runningStatus){
                if(gameOver){
                    if(dead){
                        if(frame==0){
                            Mix_HaltChannel(0);
                            if(soundOn)
                                Mix_PlayChannel(1,over,0);
                            SDL_Delay(500);
                        }
                        landTemp={stick.getX()+scrollLand-2,stick.getY()-4,stickLength,4};
                        renderStick.render(0,0,&landTemp);
                        SDL_Rect ManInImage=getRectManImage(3),manInRender={Man.getX()+25,225+frame*4,73,73};
                        frame++;
                        renderMan.render(0,0,&manInRender,&ManInImage);
                        SDL_RenderPresent(renderer);
                        if(225+frame*4>=400){
                            SDL_Delay(500);
                            Mix_HaltChannel(1);
                            if(score<=currHighScore)
                                renderGameOver.render(0,0,&gameOverRect);
                            else{
                                name=getInfo(renderer,renderHighScore);
                                scoreTemp.setInfo(name,score);
                                if(soundOn)
                                    Mix_PlayChannel(-1,butt_press,0);
                                ofstream fileWrite("hscore.dat",ios::app | ios::binary);
                                fileWrite.write((char*)&scoreTemp,sizeof(highscore));
                                SDL_Delay(1000);
                                break;
                            }
                            SDL_RenderPresent(renderer);
                            if(playAgain()){
                                    if(soundOn){
                                        Mix_PlayChannel(-1,butt_press,0);
                                        SDL_Delay(100);
                                    }
                                    BGoffset=0;
                                    stickLength=0;
                                    score=0;
                                    Land.resetLand();
                                    MouseDown=false;
                                    runningStatus=false;
                                    gameOver=false;
                                    frame=0;
                                    scrollLand=0;
                                    stick.setPosition(Land.getWidth(0)-5,224);
                                    Man.setPosition(stick.getX()-30,stick.getY()-76);
                                    renderScore.loadFont(20,PURPLE,score);
                            }
                            else
                                quit=true;
                            Mix_HaltChannel(0);
                        }
                    }
                    if(!dead)
                        dead=true;
                    else{
                        if(frame==0)
                            dead=false;
                    }
                }
                else{
                        //update
                        if(checkPerfect(stickLength,Land,stick)){
                            if(soundOn)
                                Mix_PlayChannel(0,perfect,0);
                            PFnoti.render(0,0,&pfNoti);
                            score+=20;
                            SDL_RenderPresent(renderer);
                            SDL_Delay(500);
                            Mix_HaltChannel(0);
                        }
                        else
                            score+=10;
                        Land.updateLandInfo();
                        stick.updateStickPosition(Land);
                        Man.setPosition(stick.getX()-30,stick.getY()-76);
                        renderScore.loadFont(20,PURPLE,score);
                        stickLength=0;
                        MouseDown=false;
                        runningStatus=false;
                        scrollLand=0;
                }
            }
        }
        SDL_Delay(10);
        if(!dead)
            SDL_RenderPresent(renderer);
    }
    Mix_HaltMusic();
    SDL_Delay(100);
    Mix_FreeChunk(butt_press);
    Mix_FreeChunk(over);
    Mix_FreeChunk(perfect);
    Mix_FreeChunk(horse);
    Mix_FreeMusic(gameMusic);
}

int initStartGame(SDL_Window* window,SDL_Renderer* renderer){
    //set icon window titlebar
    SDL_Surface* iconTitleBar=IMG_Load("image/icontitlebar.png");
    SDL_SetWindowIcon(window,iconTitleBar);
    //load music effects
    Mix_Music* musicMenu=Mix_LoadMUS("sound/menu.wav");
    if(!Mix_PlayingMusic())
        Mix_PlayMusic(musicMenu,-1);
    Mix_Chunk* butt_cat=Mix_LoadWAV("sound/button.wav");
    Mix_Chunk* butt_press=Mix_LoadWAV("sound/button_pressed.wav");

    gameRender buttonSound(renderer);
    SDL_Color colorkey=WHITE;
    buttonSound.loadImage("image/sound.png",&colorkey);
    SDL_Rect soundIMG,soundRender;
    soundRender={345,270,40,38};

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
    menuItem sound{345,270,40,38};
    enum button{DEFAULT=0,START,HIGHSCORE,EXIT,INST,SOUND};
    button effButton=DEFAULT;
    SDL_Event event;
    while(true){
        while(SDL_PollEvent(&event)){
            renderStart.render(0,0,&BGrect);
            if(soundOn){
                soundIMG={1,0,40,38};
                if(!Mix_PlayingMusic())
                    Mix_PlayMusic(musicMenu,-1);
            }
            else{
                soundIMG={51,0,40,38};
                Mix_HaltMusic();
            }
            buttonSound.render(0,0,&soundRender,&soundIMG);
            if(event.type==SDL_QUIT){
                return 3;
                exitGame=true;
            }
            if(start.checkArea(event.button.x,event.button.y)){
                buttonStart.render(0,0,&bStartRect,&defaultButton);
                if(effButton!=START && soundOn){
                    Mix_PlayChannel(-1,butt_cat,0);
                    effButton=START;
                }
                if(event.type==SDL_MOUSEBUTTONDOWN){
                    if(soundOn)
                        Mix_PlayChannel(-1,butt_press,0);
                    return 1;
                }
            }
            else if(highscore.checkArea(event.button.x,event.button.y)){
                buttonScore.render(0,0,&bScoreRect,&defaultButton);
                if(effButton!=HIGHSCORE && soundOn){
                    Mix_PlayChannel(-1,butt_cat,0);
                    effButton=HIGHSCORE;
                }
                if(event.type==SDL_MOUSEBUTTONDOWN){
                    if(soundOn)
                        Mix_PlayChannel(-1,butt_press,0);
                    return 2;
                }
            }
            else if(exit.checkArea(event.button.x,event.button.y)){
                buttonExit.render(0,0,&bExitRect,&defaultButton);
                if(effButton!=EXIT && soundOn){
                    Mix_PlayChannel(-1,butt_cat,0);
                    effButton=EXIT;
                }
                if(event.type==SDL_MOUSEBUTTONDOWN ){
                    if(soundOn)
                        Mix_PlayChannel(-1,butt_press,0);
                    return 3;
                    exitGame=true;
                }
            }
            else if(inst.checkArea(event.button.x,event.button.y)){
                buttonInst.render(0,0,&bInstRect);
                if(effButton!=INST && soundOn){
                    Mix_PlayChannel(-1,butt_cat,0);
                    effButton=INST;
                }
                if(event.type==SDL_MOUSEBUTTONDOWN){
                    if(soundOn)
                        Mix_PlayChannel(-1,butt_press,0);
                    return 4;
                }
            }
            else if(sound.checkArea(event.button.x,event.button.y)){
                if(effButton!=SOUND && soundOn){
                    Mix_PlayChannel(-1,butt_cat,0);
                    effButton=SOUND;
                }
                if(event.type==SDL_MOUSEBUTTONDOWN){
                    if(soundOn)
                        Mix_PlayChannel(-1,butt_press,0);
                    soundOn=!soundOn;
                }
            }
            else
                effButton=DEFAULT;
            SDL_Delay(20);
            SDL_RenderPresent(renderer);
        }
    }
    if(exitGame){
        Mix_FreeChunk(butt_cat);
        Mix_FreeChunk(butt_press);
    }
}

void initHighScore(SDL_Window* window,SDL_Renderer* renderer){
    Mix_Chunk* butt_press=Mix_LoadWAV("sound/button_pressed.wav");
    fstream scoreFile;
    menuItem buttonBack(0,345,52,52);
    highscore scoreTemp;
    string temp;
    SDL_Event event;
    gameRender scoreBoard(renderer);
    scoreBoard.loadImage("image/score.png");
    SDL_Rect scoreRect={0,0,400,400};
    scoreBoard.render(0,0,&scoreRect);
    SDL_Rect person[3];
    person[0]={131,120,0,24};
    person[1]={131,170,0,24};
    person[2]={131,225,0,24};
    SDL_Rect score[3];
    score[0]={290,120,0,24};
    score[1]={290,170,0,24};
    score[2]={290,225,0,24};
    scoreFile.open("hscore.dat",ios::app | ios::out | ios::in | ios::binary);
    vector <highscore> listScore;
    while(!scoreFile.eof()){
        scoreFile.read((char*)&scoreTemp,sizeof(highscore));
        listScore.push_back(scoreTemp);
    }
    listScore.pop_back();
    vector <highscore>:: reverse_iterator read=listScore.rbegin();
    for(int i=0;i<3;i++){
        temp=(*read).getName();
        scoreBoard.loadString(24,BLUE,temp);
        scoreBoard.setTextureSize();
        person[i].h=scoreBoard.getHeight();
        person[i].w=scoreBoard.getWidth();
        scoreBoard.render(0,0,&person[i]);
        temp=intToString((*read).getScore());
        scoreBoard.loadString(24,BLUE,temp);
        scoreBoard.setTextureSize();
        score[i].h=scoreBoard.getHeight();
        score[i].w=scoreBoard.getWidth();
        scoreBoard.render(0,0,&score[i]);
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
                if(buttonBack.checkArea(event.button.x,event.button.y)){
                    if(soundOn)
                        Mix_PlayChannel(-1,butt_press,0);
                    quit=true;
                }
            }
        }
    }
    scoreFile.close();
    SDL_Delay(100);
    Mix_FreeChunk(butt_press);
}

void initInstruction(SDL_Window* window,SDL_Renderer* renderer){
    Mix_Chunk* butt_press=Mix_LoadWAV("sound/button_pressed.wav");
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
        if(gifLoad<=80)
            gif1.render(0,0,&defaultRect);
        else if(gifLoad<=120)
            gif2.render(0,0,&defaultRect);
        else if(gifLoad<=200)
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
                if(buttonBack.checkArea(event.button.x,event.button.y)){
                    if(soundOn)
                        Mix_PlayChannel(-1,butt_press,0);
                    quit=true;
                }
            }
        }
    }
    SDL_Delay(100);
    Mix_FreeChunk(butt_press);
}
