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
void Avatar::move(int dir){

}

void Avatar::doSomething()
{
    if (!isAlive()) return;
    int ch = 0;
    if (getWorld()->getKey(ch)){
        cerr << ch << endl;
        switch (ch){
            case KEY_PRESS_LEFT:
                setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                break;
            case KEY_PRESS_ESCAPE:
                break;
            default:
                return;
        }
    }
}
