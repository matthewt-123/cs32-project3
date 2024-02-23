#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
void Actor::moveDir(int dir, int &x, int &y){
    switch(dir){
        case 0:
            x++;
            break;
        case 90:
            y++;
            break;
        case 180:
            x--;
            break;
        case 270:
            y--;
            break;
        default:
            break;
    }
}
//usage: a is an actor input, constDir is the variable held const(row/col)
//target is the target val, closestObj is the closest obj to 
//
void Actor::compareDist(int constDir, int shooter, int &closestObj, char dir, int sDir){
    //1: ignore non in line actors
    int cVar = 0; //variable coordinate(actual)
    if (dir == 'x') {
        if (getY() != constDir) return; //if not in row
        cVar = getX();
    } else if (dir == 'y'){
        if (getX() != constDir) return; //if not in col
        cVar = getY();
    }
    //cVar is the variable direction of the actor
    if (sDir > 0 && cVar < shooter) return; //pos dir, and obj is behind shooter
    else if (sDir < 0 && cVar > shooter) return; //neg dir and obj is in front of shooter
    if (abs(cVar-shooter) < abs(closestObj-shooter) ) closestObj = cVar;//if obj-shooter closer than prev closestobj-shooter, update
}


/**************
Actor::Living ABC
***************/
//update hp: return true if still alive, false if not
bool Living::updateHitpoints(int hp)
{
    if (hitpoints_ + hp > 0) {
        hitpoints_ += hp;
        return true;
    } else {
        die();
        getWorld()->playSound(deathSound_);
        return false;
    }
}
/****************
Actor::Living::Avatar
*****************/
Avatar::Avatar(double startX, double startY, StudentWorld *world)
:Living(startX, startY, IID_PLAYER, world, 20, SOUND_PLAYER_IMPACT, SOUND_PLAYER_DIE, right)
{
    numPeas_ = 20;
}

void Avatar::doSomething()
{
    if (!isAlive()) return;
    int ch = 0;
    if (getWorld()->getKey(ch)){
        switch (ch){
            case KEY_PRESS_LEFT:
                setDirection(left);
                if (getWorld()->moveActor(this, getX()-1,getY())) moveTo(getX()-1, getY());;
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (getWorld()->moveActor(this, getX()+1,getY())) moveTo(getX()+1, getY());
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if(getWorld()->moveActor(this, getX(),getY()-1)) moveTo(getX(), getY()-1);
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                if (getWorld()->moveActor(this, getX(),getY()+1)) moveTo(getX(), getY()+1);
                break;
            case KEY_PRESS_ESCAPE:
                die();
                break;
            case KEY_PRESS_SPACE:
                if (numPeas_ > 0){
                    getWorld()->firePlayer(getX(), getY(), getDirection());
                    numPeas_--;
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                }
                break;
            default:
                break;
        }
    }
}
/****************
Actor::NotAlive::Pea
*****************/
Pea::Pea(double startX, double startY, StudentWorld *world, int dir)
    :NotAlive(startX, startY, IID_PEA, world, dir)
{
    dir_ = dir;
}
void Pea::doSomething()
{
    getWorld()->peaDamage(getX(),getY(), this);
    int x = getX();
    int y = getY();
    moveDir(getDirection(), x, y);
    moveTo(x,y);
    getWorld()->peaDamage(getX(),getY(), this);
}
void Pit::doSomething()
{
    getWorld()->checkPit(getX(), getY(), this);
}

Bot::Bot(double startX, double startY, int imageID, int hp, StudentWorld *world, int deathSound_, int impactSound_,int dir)
:Living(startX, startY, imageID, world, hp, SOUND_ROBOT_IMPACT, SOUND_ROBOT_DIE, dir)
{
    tickCt_ = 0;
    ticks_=(28-world->getLevel())/4;
    if (ticks_ < 3) ticks_ = 3;
}
bool Bot::isTick()
{
    bool r_val = (tickCt_ % ticks_ == 0) ? true : false;
    tickCt_++;
    return r_val;
}
/****************
Actor::Living::RageBot
*****************/

void RageBot::doSomething()
{
    if (!isAlive()) return;
    Actor *avatar = getWorld()->getAvatar();
    if (isTick()){
        if (!getWorld()->firePeaBot(getX(), getY(),getDirection(), avatar->getX(), avatar->getY(), this)){ //if no fire
            //move
            int x = getX(); 
            int y = getY();
            moveDir(getDirection(), x,y);
            if (!getWorld()->moveActor(this, x,y)) setDirection(getDirection() - 180);
            else moveTo(x,y);
        }
    } 
}
void RageBot::cleanUp() {getWorld()->increaseScore(100);};

/****************
Actor::Living::ThiefBot
*****************/
void ThiefBot::doSomething()
{
    if (!isAlive()) return;
    if (isTick()){
        stealOrMove();
    }
}
//steal goodie or move, changing direction
void ThiefBot::stealOrMove()
{
    if (goodie_ != nullptr || !getWorld()->stealGoodie(this, getX(), getY())){ //if has goodie or not on same square as goodie
        int x = getX();
        int y = getY();
        //try to move
        moveDir(getDirection(), x, y);
        //if exceeded steps in direction OR obstacle
            //turn
        //else
            //move, increment dist travelled
        if (!getWorld()->moveActor(this, x, y) || distanceTraveled  % distBeforeTurning != 0) {
            //rand turn
            int turnCt = 0;
            bool obstacle = true;
            while (turnCt < 4 && obstacle){
                //check all 4 directions, setting random direction
                int dir = 90 * (rand()%4);
                turnCt++;
                int x = getX();
                int y = getY();
                moveDir(dir, x,y); //try to move actor
                if (getWorld()->moveActor(this, x, y)){
                    obstacle = false;
                    setDirection(dir);
                    moveTo(x,y);
                    break;
                }
                else obstacle = true;
            }
        }
        else {
            //else: steps not exceeded and no obstacle
            moveTo(x,y);
            distanceTraveled++;
        }
    }
}
void ThiefBot::collectGoodie(Actor *goodie)
{
    goodie_ = goodie;
    goodie_->setVisible(false);
}
void ThiefBot::cleanUp()
{
    getWorld()->increaseScore(10);
    deleteGoodie();
}
void ThiefBot::deleteGoodie()
{
    if (goodie_ != nullptr){
        goodie_->moveTo(getX(), getY());
        goodie_->setVisible(true);
        getWorld()->addActor(goodie_);
        goodie_ = nullptr;
    }
}
ThiefBot::~ThiefBot()
{
    if (goodie_ != nullptr) {delete goodie_;};
}
void MeanThiefBot::doSomething()
{
    if (isTick()){
        Avatar *avatar = getWorld()->getAvatar();
        if (!getWorld()->firePeaBot(getX(), getY(),getDirection(), avatar->getX(), avatar->getY(), this)) stealOrMove();
    }
}

void MeanThiefBot::cleanUp()
{
    getWorld()->increaseScore(20);
    deleteGoodie();
}
bool Living::damage(int x, int y)
{
    if (x != getX() || y != getY()) return false; //cannot damage obj not on square
    updateHitpoints(-2);
    if(getHitpoints() <= 0) {
        die();
        getWorld()->playSound(deathSound_);
    }
    getWorld()->playSound(impactSound_);
    cerr << "ouch";
    return true;
}
/*************
Actor::NotAlive::Goodie is an ABC
**************/
Goodie::Goodie(double startX, double startY, int imageID, StudentWorld *world, int pts)
:NotAlive(startX, startY, imageID, world)
{
    points_ = pts;
}
void Goodie::doSomething()
{
    if (getWorld()->getAvatar()->onSameSquare(getX(), getY()))
    {
        getWorld()->increaseScore(getPoints());
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        pickUpGoodie(); //decrement crystal ct
    } 
}

void Exit::doSomething()
{
    if (!active_){
        if (getWorld()->remainingCrystals() > 0) return;
        else {
            getWorld()->playSound(SOUND_REVEAL_EXIT);
            active_ = true;
            setVisible(true);
        };
    }
    if (getWorld()->getAvatar()->onSameSquare(getX(), getY()))
    {
        getWorld()->increaseScore(getPoints());
        getWorld()->levelComplete();
    }
}
void Factory::doSomething()
{
    if (getWorld()->countThiefBots(getX()-3, getX() + 3, getY() -3, getY()+3) < 3)
    {
        if (rand()%50 == 6){
            if (botType_ == 'm'){
                Actor* newMeanThief = new MeanThiefBot(getX(), getY(), getWorld());
                getWorld()->addActor(newMeanThief);
            } else if (botType_ == 't') {
                Actor* newThief = new ThiefBot(getX(), getY(), getWorld());
                getWorld()->addActor(newThief);
            }
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }

    }
}
void HealthGoodie::pickUpGoodie() {getWorld()->getAvatar()->restoreHealth();}; //restore health
void AmmoGoodie::pickUpGoodie(){getWorld()->getAvatar()->addAmmo(20);}; //add ammo
void Crystal::pickUpGoodie() {getWorld()->collectCrystal();}; //decrement crystal ct
void ExtraLifeGoodie::pickUpGoodie() {getWorld()->incLives();}; //add life 
