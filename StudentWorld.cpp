#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>    // header file for stringstream
//sDir: +1 for pos dir, -1 for neg dir
//targetX, Y is coord of player
bool isPlayerInSight(vector<Actor*> &actors_, int startX, int startY, Avatar *avatar_, int sDir, int targetX, int targetY, char dirSwitch, Actor* Bot)
{
    int shooter,constDir,target;
    switch (dirSwitch){
        case 'x':
            if (startY != targetY) return false; //need to be on same row to be in sight
            if (sDir > 0 && startX > targetX) return false; //pos dir, and target is to the left of shooter
            else if (sDir < 0 && startX < targetX) return false; //negative dir, and target is to the right of shooter
            constDir = startY; //need to be in same col
            shooter = startX; //target var of shooter
            target = targetX;
            cerr << constDir;
            break;
        case 'y':
            if (startX != targetX) return false; //need to be on same col to be in sight
            if (sDir > 0 && startY > targetY) return false; //pos dir, and target is to the left of shooter
            else if (sDir < 0 && startY < targetY) return false; //negative dir, and target is to the right of shooter
            constDir = startX; //const fixed dir
            shooter = startY; //target var of shooter
            target = targetY;
            break;
    }
    int closestObj = 1000;
    vector<Actor*>::iterator it = actors_.begin();
    while (it != actors_.end()) {it++;};
    it = actors_.begin();
    while (it != actors_.end())
    {
        if (*it != Bot) { //ignore self
            //compare distance from each actor to start
            (*it)->compareDist(constDir, shooter, closestObj, dirSwitch, sDir);
        }
        it++;    

    }
    return (abs(shooter-closestObj) < abs(shooter-target)) ? false : true;
}

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}
// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    bonusPts_ = 1000;
    crystalCt_ = 0;
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
                case Level::horiz_ragebot:
                    actors_.push_back(new RageBot(i,j,this, 0));
                    break;
                case Level::vert_ragebot:
                    actors_.push_back(new RageBot(i,j,this, 270));
                    break;
                case Level::crystal:
                    actors_.push_back(new Crystal(i,j,this));
                    crystalCt_++;
                    break;
                case Level::exit:
                    actors_.push_back(new Exit(i,j,this));
                    break;

            }
        }
    }
    return GWSTATUS_CONTINUE_GAME; //default exit
}
int StudentWorld::init()
{
    bonusPts_ = crystalCt_ = 0;
    levelFinish_ = false;    

    int r = loadLevel();
    if (r != GWSTATUS_CONTINUE_GAME) return r; //error loading game
    //reset member vars:
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
    avatar_->doSomething();
    if (!(avatar_->isAlive())) {decLives();return GWSTATUS_PLAYER_DIED;}; //Player Died
    // //cleanup: remove dead actors
    it = actors_.begin();
    while (it != actors_.end()) {
        if (!((*it)->isAlive())){ //actor is dead
            delete (*it);
            it = actors_.erase(it);
        }
        else it++;
    }
    bonusPts_--;
    if (levelFinish_) return GWSTATUS_FINISHED_LEVEL;
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
bool StudentWorld::moveActor(Actor* actor, int newX, int newY)
{
    //check square that actor wants to move into
    vector<Actor*>::iterator it = actors_.begin();
    while (it != actors_.end())
    {
        if ((*it)->onSameSquare(newX, newY)){
            if ((*it) == actor) break; //ignore self
            //check if it is alive(cannot move onto non living objects)
            if ((*it)->isAffectedByPea()) return false; //is a wall or factory
            // if ((*it)->canCollect()) cerr << "COLLECT ME"; //COLLECT ME!!
        }
        it++;
    }
    if (avatar_->onSameSquare(newX, newY)) return false; //check avatar
    //move actor
    actor->moveTo(newX, newY);
    return true;
}

StudentWorld::~StudentWorld()
{
    cleanUp(); //delete any remaining actors when game ends with destructor. 
}
bool StudentWorld::firePeaBot(int startX, int startY, int dir, int targetX, int targetY, Actor* Bot)
{
    //check if any obstacles between bot and player
        //is the next thing in line of sight player?
    switch(dir){
        case 0: //right
            if (isPlayerInSight(actors_, startX, startY, avatar_, 1, targetX, targetY, 'x', Bot)){
                actors_.push_back(new Pea(startX+1, startY,this, 0));
                return true;
            }
            break; 
        case 90: //up
            if (isPlayerInSight(actors_, startX, startY, avatar_, 1, targetX, targetY, 'y', Bot)){
                actors_.push_back(new Pea(startX, startY+1,this, 90));
                return true;
            }
            break;
        case 180: //left
            if (isPlayerInSight(actors_, startX, startY, avatar_, -1, targetX, targetY, 'x', Bot)){
                actors_.push_back(new Pea(startX-1, startY,this, 180));
                return true;
            }
            break;
        case 270: //down
            if (isPlayerInSight(actors_, startX, startY, avatar_, -1, targetX, targetY, 'y', Bot)){
                actors_.push_back(new Pea(startX, startY-1,this, 270));
                return true;
            }
            break;
        default:
            break;
    }
    return false;

}
bool StudentWorld::peaDamage(int startX, int startY, Actor *pea)
{
    //check actors first
    bool alive = true;
    //20240220 soln
    vector<Actor*>::iterator it = actors_.begin();
    while (it != actors_.end())
    {
        //damage if possible
        if ((*it)->isAffectedByPea()) {
            if ((*it)->damage(startX, startY)) pea->die();
        }
        it++;
    }
    if (!pea->isAlive()) return false;
    //then check avatar
    if (avatar_->damage(startX, startY)) {
        pea->die();
        return false;
    };
    return true;
}
void StudentWorld::collectCrystal(){
    crystalCt_--;
    if (crystalCt_ <= 0){
        cerr << "the exit (Conan Gray)";
    }
}
