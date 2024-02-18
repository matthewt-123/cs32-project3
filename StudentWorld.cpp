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
}


int StudentWorld::loadLevel()
{
    int currLevel = getLevel();
    if (currLevel > 99) return 2; //game complete
    string levelInt = (currLevel < 10) ? "0" + to_string(currLevel) : to_string(currLevel);
    string levelStr = "level" + levelInt + ".txt";
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(levelStr);
    if (result == Level::load_fail_file_not_found ||result == Level:: load_fail_bad_format)
        return 1; //error
    for (int i = 0; i < 15; i++){ //column
        for (int j = 0; j < 15; j++) //row
        {
            switch(lev.getContentsOf(i, j)){
                case Level::player:
                    break;
                case Level::wall:
                    actors_.push_back(new Wall(i,j,this));
            }
        }
    }
    return 0; //default exit
}

int StudentWorld::init()
{
    int ll = loadLevel();
    if (ll == 1) return GWSTATUS_LEVEL_ERROR;
    else if (ll == 2) return GWSTATUS_PLAYER_WON;
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you type q

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
}
StudentWorld::~StudentWorld()
{
    cleanUp(); //delete any remaining actors when game ends with destructor. 
}
