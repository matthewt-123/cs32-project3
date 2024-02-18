#ifndef ACTOR_H_
#define ACTOR_H_

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
        Actor(StudentWorld *world, int imageID, double startX, double startY,int dir=0, double size = 1.0) 
        :GraphObject(imageID, startX, startY ,dir, size)
        {world=world; alive_=true;};
        void die() {alive_ = false;}; //kill actor -> alive is false and should be removed
        virtual ~Actor() {return;}; //deleting actor should not delete student world
        virtual void doSomething() = 0;
    private:
        StudentWorld *world;
        bool alive_;

};
class Wall : public Actor
{
    public:
        Wall(double startX, double startY, StudentWorld *world)
        :Actor(world, IID_WALL, startX, startY){setVisible(true);};
        virtual void doSomething() {return;}; //
        virtual ~Wall() {return;};
};
#endif // ACTOR_H_
