#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>    // header file for stringstream
#include <iomanip>  // defines the manipulator setw
using namespace std;
//sDir: +1 for pos dir, -1 for neg dir
//targetX, Y is coord of player
bool StudentWorld::isPlayerInSight(int startX, int startY, Avatar *avatar_, int sDir, int targetX, int targetY, char dirSwitch, Actor* Bot)
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
        if (*it != Bot && (*it)->isAffectedByPea()) { //ignore self
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
                case Level::ammo:
                    actors_.push_back(new AmmoGoodie(i,j,this));
                    break;
                case Level::extra_life:
                    actors_.push_back(new ExtraLifeGoodie(i,j,this));
                    break;    
                case Level::mean_thiefbot_factory:
                    actors_.push_back(new Factory(i,j,this, 'm'));
                    break;    
                case Level::thiefbot_factory:
                    actors_.push_back(new Factory(i,j,this, 't'));
                    break;    
                case Level::marble:
                    actors_.push_back(new Marble(i,j,this));
                    break;    
                case Level::pit:
                    actors_.push_back(new Pit(i,j,this));
                    break;             
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME; //default exit
}
//check if marble can be moved to new dir (one past where it is)
bool StudentWorld::moveMarble(Actor* marble, int newX, int newY)
{
    vector<Actor*>::iterator it = actors_.begin();
    while (it != actors_.end()) {
        //looking for whatever is on next square one over
        if ((*it)->onSameSquare(newX, newY)) {
            if((*it)->isAffectedByPea() && !(*it)->canMarble()) return false;

        };
        it++;
    }
    return true;
}

int StudentWorld::init()
{
    bonusPts_ = crystalCt_ = 0;
    levelFinish_ = false;    
    bonusPts_ = 1000;

    int r = loadLevel();
    if (r != GWSTATUS_CONTINUE_GAME) return r; //error loading game
    //reset member vars:
    return GWSTATUS_CONTINUE_GAME;
}
string StudentWorld::formatString()
{
    string output = "";
    ostringstream oss;
	oss.setf(ios::fixed);
    oss.fill('0');
    oss << "  Score: " << setw(7) << getScore();
    output += oss.str();

    ostringstream level;
	level.setf(ios::fixed);
    level.fill('0');
    level << "  Level: " << setw(2) << getLevel();
    output += level.str();

    ostringstream lives;
	lives.setf(ios::fixed);
    lives.fill(' ');
    lives << "  Lives: " << setw(2) << getLives();
    output += lives.str();

    ostringstream health;
	health.setf(ios::fixed);
	health.precision(0);
    health.fill(' ');
    health << "  Health: " << setw(3) << (avatar_->getHitpoints() / 20.0) * 100 << "%"; //float division
    output += health.str();

    ostringstream ammo;
	ammo.setf(ios::fixed);
    ammo.fill(' ');
    ammo << "  Ammo: " << setw(2) << (avatar_->getAmmo());
    output += ammo.str();

    ostringstream bonus;
	bonus.setf(ios::fixed);
    bonus.fill(' ');
    bonus << "  Bonus: " << setw(4) << bonusPts_;
    output += bonus.str();
    return output;
}
int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run and terminate after you type q
    vector<Actor*>::iterator it = actors_.begin();
    while (it != actors_.end()) {
        (*it)->doSomething();
        it++;
    }
    avatar_->doSomething();
    //add new items from tmp to actor_ vector
    vector<Actor*>::iterator tmp_it = tmp_.begin();
    while (tmp_it != tmp_.end()) {
        actors_.push_back(*tmp_it);
        tmp_it++;
    }
    tmp_.clear();

    //delete item from delete_ vector
    map<Actor*, bool>::iterator erase_it = delete_.begin();
    while (erase_it != delete_.end()) {
        actors_.erase(find(actors_.begin(), actors_.end(), erase_it->first));
        if (erase_it->second) delete erase_it->first;

        erase_it++;
    }    
    delete_.clear();

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
    if (bonusPts_ > 0) bonusPts_--;
    if (levelFinish_) {
        increaseScore(bonusPts_);
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    setGameStatText(formatString());

	return GWSTATUS_CONTINUE_GAME; 
}

void StudentWorld::cleanUp()
{
    vector<Actor*>::iterator it = actors_.begin();
    int ct = 0;

    while (it != actors_.end())
    {
        delete *it; //free any remaining actors
        ct++;
        it = actors_.erase(it); //delete item from vector
    }
    actors_.clear();
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
            if (actor == avatar_ && (*it)->canPush()) { //only actor can move marble
                int x = newX;
                int y = newY;
                actor->moveDir(actor->getDirection(), x, y); //new marble dir
                if (moveMarble((*it), x, y)) {(*it)->moveTo(x, y);return true;};
                return false;
            }
            if ((*it)->isAffectedByPea()) return false; //is a wall or factory
        }
        it++;
    }
    if (avatar_->onSameSquare(newX, newY)) return false; //check avatar
    //move actor
    return true;
}

StudentWorld::~StudentWorld()
{
    cleanUp(); //delete any remaining actors when game ends with destructor. 
}
void StudentWorld::firePlayer(int startX, int startY, int dir)
{
    switch(dir)
    {
        case 0: //right
            tmp_.push_back(new Pea(startX+1, startY,this, dir));
            break;
        case 90: //up
            tmp_.push_back(new Pea(startX, startY+1,this, dir));
            break;
        case 180:
            tmp_.push_back(new Pea(startX-1, startY,this, dir));
            break;
        case 270:
            tmp_.push_back(new Pea(startX, startY-1,this, dir));
            break;
    }
}
bool StudentWorld::firePeaBot(int startX, int startY, int dir, int targetX, int targetY, Actor* Bot)
{
    //check if any obstacles between bot and player
        //is the next thing in line of sight player?
    switch(dir){
        case 0: //right
            if (isPlayerInSight(startX, startY, avatar_, 1, targetX, targetY, 'x', Bot)){
                tmp_.push_back(new Pea(startX+1, startY,this, 0));
                playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            break; 
        case 90: //up
            if (isPlayerInSight(startX, startY, avatar_, 1, targetX, targetY, 'y', Bot)){
                tmp_.push_back(new Pea(startX, startY+1,this, 90));
                playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            break;
        case 180: //left
            if (isPlayerInSight(startX, startY, avatar_, -1, targetX, targetY, 'x', Bot)){
                tmp_.push_back(new Pea(startX-1, startY,this, 180));
                playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            break;
        case 270: //down
            if (isPlayerInSight(startX, startY, avatar_, -1, targetX, targetY, 'y', Bot)){
                tmp_.push_back(new Pea(startX, startY-1,this, 270));
                playSound(SOUND_ENEMY_FIRE);
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
    if (!pea->isAlive()) return false;
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

bool StudentWorld::stealGoodie(Actor *thiefBot, int x, int y){
    vector<Actor*>::iterator it = actors_.begin();
    bool r_val = false;
    while (it != actors_.end())
    {
        if ((*it)->onSameSquare(x, y)) {
            //damage if possible
            if ((*it)->canCollect()) {
                r_val = true; //returns true if on same square as goodie
                if (rand()%10 == 6){
                    thiefBot->collectGoodie(*it);
                    deleteActor(*it, false); //remove from actors array and store address in thiefbot
                    playSound(SOUND_ROBOT_MUNCH);
                    return true;
                }
            }
        }
        it++;
    }    
    return r_val;
}
int StudentWorld::countThiefBots(double startX, double endX, double startY, double endY)
{
    vector<Actor*>::iterator it = actors_.begin();
    int ct = 0;
    while (it != actors_.end())
    {
        if ((*it)->isThiefBot() && (*it)->inRange(startX, endX, startY, endY)) {
            ct++;
        }
        it++;
    }    
    return ct;
}

void StudentWorld::checkPit(int x, int y, Actor *pit)
{
    vector<Actor*>::iterator it = actors_.begin();
    while (it != actors_.end())
    {
        if ((*it)->canPush() && (*it)->onSameSquare(x, y)) {
            deleteActor(*it, true); //delete pit
            deleteActor(pit, true);
        }
        it++;
    }     
}