#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

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
Avatar::Avatar(double startX, double startY, StudentWorld *world)
:Living(startX, startY, IID_PLAYER, world, right)
{
    updateHitpoints(20);
    numPeas_ = 20;
}

int Avatar::doSomething()
{
    if (!isAlive()) return GWSTATUS_PLAYER_DIED;
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
                return GWSTATUS_PLAYER_DIED;
                break;
            default:
                break;
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}
