#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
void moveDir(int dir, int &x, int &y){
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
    if (hitpoints_ + hp >= 0) {
        hitpoints_ += hp;
        return true;
    } else {
        die();
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
                getWorld()->moveActor(this, getX()-1,getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                getWorld()->moveActor(this, getX()+1,getY());
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                getWorld()->moveActor(this, getX(),getY()-1);
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                getWorld()->moveActor(this, getX(),getY()+1);
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
Actor::Living::Pea
*****************/
Pea::Pea(double startX, double startY, StudentWorld *world, int dir)
:NotAlive(startX, startY, IID_PEA, world, dir)
{
    dir_ = dir;
}
void Pea::doSomething()
{
    if (!isAlive()) return;
    getWorld()->peaDamage(getX(),getY(), this);
    int x = getX();
    int y = getY();
    moveDir(getDirection(), x, y);
    moveTo(x,y);
    getWorld()->peaDamage(getX(),getY(), this);
}

/****************
Actor::Living::RageBot
*****************/
RageBot::RageBot(double startX, double startY, StudentWorld *world, int dir)
:Living(startX, startY, IID_RAGEBOT, world, 10, SOUND_ROBOT_IMPACT, SOUND_ROBOT_DIE, dir)
{
    tickCt_ = 0;
    ticks_=(28-world->getLevel())/4;
    if (ticks_ < 3) ticks_ = 3;
};

void RageBot::doSomething()
{
    if (!isAlive()) return;
    Actor *avatar = getWorld()->getAvatar();
    if (tickCt_ % ticks_ == 0){
        if (!getWorld()->firePeaBot(getX(), getY(),getDirection(), avatar->getX(), avatar->getY(), this)){ //if no fire
            //move
            int x = getX(); 
            int y = getY();
            moveDir(getDirection(), x,y);
            if (!getWorld()->moveActor(this, x,y)) setDirection(getDirection() - 180);
        }

    } 
    tickCt_++;
}
void RageBot::die(){
     
}

bool Living::damage(int x, int y)
{
    if (x != getX() || y != getY()) return false; //cannot damage obj not on square
    updateHitpoints(-2);
    cerr << getHitpoints();
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

void Crystal::doSomething()
{
    if (getWorld()->getAvatar()->onSameSquare(getX(), getY()))
    {
        getWorld()->increaseScore(getPoints());
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->collectCrystal(); //decrement crystal ct
    }
}

void Exit::doSomething()
{
    if (!active_){
        if (getWorld()->remainingCrystals() > 0) return;
        else {
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