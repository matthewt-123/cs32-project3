#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
    public:
        virtual Actor();
        ~Actor();
    private:
        StudentWorld world;
}
#endif // ACTOR_H_
