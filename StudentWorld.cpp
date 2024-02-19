#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>    // header file for stringstream
//sDir: +1 for pos dir, -1 for neg dir
bool isPlayerInSightX(vector<Actor*> &actors_, int startX, int startY, Avatar *avatar_, int sDir, int targetX, int targetY)
{
    vector<Actor*>::iterator it = actors_.begin();
    if (startY != targetY) return false; //need to be on same row to be in sight
    if (sDir > 0 && startX > targetX) return false; //pos dir, and target is to the left of shooter
    else if (sDir < 0 && startX < targetX) return false; //negative dir, and target is to the right of shooter

    int closestObjX = 1000;
    while (it != actors_.end())
    {
        if ((*it)->getY() == startY){
            int xVar = (*it)->getX();
            if (sDir > 0 && xVar < startX) break;
            else if (sDir < 0 && xVar > startX) break;
            if (abs(xVar-startX) < abs(closestObjX-startX) ) closestObjX = xVar;
        }
        it++;
    }
    return (closestObjX < targetX) ? false : true;
}
bool isPlayerInSightY(vector<Actor*> &actors_, int startX, int startY, Avatar *avatar_, int sDir, int targetX, int targetY)
{
    vector<Actor*>::iterator it = actors_.begin();
    if (startX != targetX) return false; //need to be on same row to be in sight
    if (sDir > 0 && startY > targetX) return false; //pos dir, and target is to the left of shooter
    else if (sDir < 0 && startY < targetY) return false; //negative dir, and target is to the right of shooter
    int closestObjX = 1000;
    while (it != actors_.end())
    {
        if ((*it)->getX() == startX){
            int YVar = (*it)->getY();
            if (sDir > 0 && YVar < startY) break;
            else if (sDir < 0 && YVar > startY) break;
            if (abs(YVar-startY) < abs(closestObjX-startY) ) closestObjX = YVar;
        }
        it++;
    }
    return (closestObjX < targetY) ? false : true;
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
        if (actor->onSameSquare(*it, newX, newY)){
            if ((*it) == actor) break; //ignore self
            //check if it is alive(cannot move onto non living objects)
            if ((*it)->isAffectedByPea()) return false; //is a wall or factory
            if ((*it)->canCollect()) cerr << "COLLECT ME"; //COLLECT ME!!
        }
        it++;
    }
    if (actor->onSameSquare(avatar_, newX, newY)) return false; //check avatar
    //move actor
    actor->moveTo(newX, newY);
    return true;
}

StudentWorld::~StudentWorld()
{
    cleanUp(); //delete any remaining actors when game ends with destructor. 
}
bool StudentWorld::firePeaBot(int startX, int startY, int dir, int targetX, int targetY)
{
    //check if any obstacles between bot and player
        //is the next thing in line of sight player?
    switch(dir){
        case 0: //right
            if (isPlayerInSightX(actors_, startX, startY, avatar_, 1, targetX, targetY))
                actors_.push_back(new Pea(startX+1, startY,this, 0));
            break; 
        case 90: //up
            if (isPlayerInSightY(actors_, startX, startY, avatar_, 1, targetX, targetY)) 
                actors_.push_back(new Pea(startX, startY+1,this, 90));
            break;
        case 180: //left
            if (isPlayerInSightX(actors_, startX, startY, avatar_, -1, targetX, targetY)) 
                actors_.push_back(new Pea(startX-1, startY,this, 0));
            break;
        case 270: //down
            if (isPlayerInSightY(actors_, startX, startY, avatar_, -1, targetX, targetY)) 
                actors_.push_back(new Pea(startX, startY-1,this, 270));
            break;
        default:
            break;
    }
    return true;

}
void StudentWorld::peaDamage(int startX, int startY, Actor *pea)
{
    vector<Actor*>::iterator it = actors_.begin();
    //check actors first
    bool alive = true;
    while (it != actors_.end())
    {
        if ((*it)->getX() == startX && (*it)->getY() == startY && (*it)->isAffectedByPea()) {
            (*it)->damage();
            alive = false;
        }
        it++;
    }
    //then check avatar
    if (avatar_->getX() == startX && avatar_->getY() == startY)
    {
        avatar_->damage();
        alive = false;
        pea->die();
    }
    if (!alive) pea->die();
 
}
