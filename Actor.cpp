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
void Actor::compareDist(Actor* a, int constDir, int shooter, int &closestObj, char dir, int sDir){
    //1: ignore non in line actors
    int cVar = 0; //variable coordinate(actual)
    if (dir == 'x') {
        if (a->getY() != constDir) return; //if not in row
        cVar = a->getX();

    } else if (dir == 'y'){
        if (a->getX() != constDir) return; //if not in col
        cVar = a->getY();
    }
    if (sDir > 0 && cVar < shooter) return; //pos dir, and obj is behind shooter
    else if (sDir < 0 && cVar > shooter) return; //neg dir and obj is in front of shooter
    if (abs(cVar-shooter) < abs(closestObj-shooter) ) closestObj = cVar; //if obj-shooter closer than prev closestobj-shooter, update
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
:Living(startX, startY, IID_PLAYER, world, right, 20)
{
    updateHitpoints(20);
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
                return;
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
:Living(startX, startY, IID_RAGEBOT, world, 10, dir)
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
bool Living::damage(int x, int y)
{
    if (x != getX() || y != getY()) return false; //cannot damage obj not on square
    updateHitpoints(-2);
    if(getHitpoints() <= 0) {
        die();
    }
    cerr << "ouch";
    return true;
}