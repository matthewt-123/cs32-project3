#ifndef ACTOR_H_
#define ACTOR_H_
#define TEST
#ifdef TEST
#include <iostream>
using namespace std;
#endif
#include "GraphObject.h"
class StudentWorld;
class Player;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
/*****************************
Actor Base Class: 
Constructor needs world ptr, image, (x,y), optional: dir and size
die(): kills actor by setting its aliveness to false and marking it for deletion
Fully virtual function doSomething()
*****************************/
class Actor : public GraphObject
{
    public:
        Actor(StudentWorld *world, int imageID, double startX, double startY,int dir=-1) 
        :GraphObject(imageID, startX, startY ,dir)
        {world_=world; alive_=true;setVisible(true);};
        void die() {alive_ = false;}; //kill actor -> alive is false and should be removed
        virtual ~Actor() {return;}; //deleting actor should not delete student world
        virtual void doSomething() = 0;
        virtual void damage() {}; //default: nothing happens when hit by pea
        virtual bool canDie() {return false;}; //default actors cannot die(walls, factories)
        virtual bool canCollect() {return false;}; //cannot collect default actors
        bool isAlive() {return alive_;}; //getter function
        virtual bool isAffectedByPea() {return true;};
        bool onSameSquare(Actor* a1, int x, int y){return (a1->getX() == x && a1->getY() == y) ? true : false;};
        StudentWorld *getWorld() {return world_;};
    private:
        StudentWorld *world_;
        bool alive_;

};
//ACTOR DERIVED: Wall
class Wall : public Actor
{
    public:
        Wall(double startX, double startY, StudentWorld *world)
        :Actor(world, IID_WALL, startX, startY){};
        virtual void doSomething() {return;}; //
        virtual ~Wall() {return;};
};
//ACTOR DERIVED: Factory
//TODO

//ACTOR DERIVED ABC: Living Objects(can die during gameplay)
class Living : public Actor
{
    public:
        Living(double startX, double startY, int imageID, StudentWorld *world,int hp,int dir=none)
        :Actor(world, imageID, startX, startY,dir){hitpoints_=hp;};
        virtual bool canDie() {return true;};
        virtual void damage(); //default: remove 2 hp and dies if no hp left
        virtual void doSomething() = 0; //is an ABC, cannot be directly created
        int getHitpoints() {return hitpoints_;};
        bool updateHitpoints(int hp); //if returns false, stop dosomething
    private:
        int hitpoints_;
};
//Living Object: Avatar
class Avatar : public Living
{
    public:
        Avatar(double startX, double startY, StudentWorld *world);
        virtual void doSomething(); 
    private:
        int numPeas_;
};

//Living Object: Rage Bot
class RageBot : public Living
{
    public:
        RageBot(double startX, double startY, StudentWorld *world, int dir);
        virtual void doSomething();
    private:
        int ticks_;
        int tickCt_;
};


class NotAlive : public Actor
{
    public:
        NotAlive(double startX, double startY, int imageID, StudentWorld *world,int dir=none)
        :Actor(world, imageID, startX, startY,dir){};
        virtual void doSomething() = 0; //ABC
        virtual bool isAffectedByPea() {return false;};
        virtual void damage() {}; //non-living objects cannot be damaged


};

//Non Living Object: Pea
class Pea : public NotAlive
{
    public:
        Pea(double startX, double startY, StudentWorld *world, int dir);
        virtual void doSomething();
    private:
        int dir_;

};

#endif // ACTOR_H_
