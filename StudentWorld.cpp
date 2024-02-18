#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>    // header file for stringstream
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    bonusPts_ = 1000;
}
int StudentWorld::loadLevel()
{
    int currLevel = getLevel();
    if (currLevel > 99) return GWSTATUS_PLAYER_WON; //game complete
    string levelInt = (currLevel < 10) ? "0" + to_string(currLevel) : to_string(currLevel);
    string levelStr = "level" + levelInt + ".txt";
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(levelStr);
    if (result == Level::load_fail_file_not_found ||result == Level:: load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR; //error
    for (int i = 0; i < 15; i++){ //column
        for (int j = 0; j < 15; j++) //row
        {
            switch(lev.getContentsOf(i, j)){
                case Level::player:
                    if (avatar_ != nullptr) return GWSTATUS_LEVEL_ERROR; //cannot have 2 avatars!
                    avatar_ = new Avatar(i,j,this);
                    break;
                case Level::wall:
                    actors_.push_back(new Wall(i,j,this));
                    break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME; //default exit
}
int StudentWorld::init()
{
    int r = loadLevel();
    if (r != GWSTATUS_CONTINUE_GAME) return r; //error loading game
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you type q
    vector<Actor*>::iterator it = actors_.begin();
    while (it != actors_.end()) {
        (*it)->doSomething();
        it++;
    }
    int a = avatar_->doSomething();
    if (a != GWSTATUS_CONTINUE_GAME) return a; //Player Died
    //cleanup: remove dead actors
    it = actors_.begin();
    while (it != actors_.end()) {
        if (!(*it)->isAlive()){
            delete *it;
            it = actors_.erase(it);
        }
        it++;
    }
    bonusPts_--;
    setGameStatText("Game will end when you type q");
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    vector<Actor*>::iterator it = actors_.begin();
    while (it != actors_.end())
    {
        delete *it; //free any remaining actors
        it = actors_.erase(it); //delete item from vector
    }
    if (avatar_ != nullptr) {delete avatar_;avatar_=nullptr;};

}
void StudentWorld::moveActor(Actor* actor, int newX, int newY)
{
    //check square that actor wants to move into
    vector<Actor*>::iterator it = actors_.begin();
    while (it != actors_.end())
    {
        if ((*it)->getX() == newX && (*it)->getY() == newY){
            //check if it is alive(cannot move onto non living objects)
            if (!(*it)->canDie()) return;
        }
        it++;
    }
    //move actor
    actor->moveTo(newX, newY);
}

StudentWorld::~StudentWorld()
{
    cleanUp(); //delete any remaining actors when game ends with destructor. 
}