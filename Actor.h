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
        void die() {setVisible(false);alive_ = false;cleanUp();}; //kill actor -> alive is false and should be removed
        virtual ~Actor() {return;}; //deleting actor should not delete student world
        virtual void doSomething() = 0;
        virtual bool damage(int x, int y) {return (getX() == x && getY() == y) ? true : false;}; //default: pea dies when hitting obj
        virtual bool canDie() {return false;}; //default actors cannot die(walls, factories)
        virtual bool canCollect() {return false;}; //cannot collect default actors (for thief bot use)
        bool isAlive() {return alive_;}; //getter function
        virtual bool isAffectedByPea() {return true;}; //if peas can hit objects
        virtual void cleanUp() {}; //clean up before death
        bool onSameSquare(int x, int y){return (getX() == x && getY() == y) ? true : false;}; //if cur obj is on same square as another
        bool inRange(int startX, int endX, int startY, int endY) {return (getX() >= startX && getX() <= endX && getY() >= startY && getY() <= endY)?true:false;}; //if obj is in an [a,b]x[c,d] square
        void compareDist(int constDir, int target, int &closestObj, char dir, int sDir); //compare distances between closest obj and target and self and target
        StudentWorld *getWorld() {return world_;}; //getter func for world
        virtual void collectGoodie(Actor *goodie) {}; //thief bot collecting goodie
        virtual bool isThiefBot() {return false;}; //check if thief bot or mean thief bot for factory use
        virtual bool canPush() {return false;};
        void moveDir(int dir, int &x, int &y);
        virtual bool canMarble() {return false;};
    private:
        StudentWorld *world_;
        bool alive_;
};
/*****************************
Actor Derived Classes: 
*****************************/
class Factory : public Actor
{
    public:
        Factory(double startX, double startY, StudentWorld *world, char botType)
            :Actor(world, IID_ROBOT_FACTORY, startX, startY){botType_=botType;};
        virtual void doSomething();
    private:
        char botType_; //m is meanthiefbot, t is thiefbot
};
//ACTOR DERIVED: Wall
class Wall : public Actor
{
    public:
        Wall(double startX, double startY, StudentWorld *world)
            :Actor(world, IID_WALL, startX, startY){};
        virtual void doSomething() {return;}; //do nothing
};

//ACTOR DERIVED ABC: Living Objects(can die during gameplay)
class Living : public Actor
{
    public:
        Living(double startX, double startY, int imageID, StudentWorld *world,int hp,int deathSound_, int impactSound_,int dir=none)
            :Actor(world, imageID, startX, startY,dir){hitpoints_=hp;};
        virtual bool canDie() {return true;}; //can die
        virtual void doSomething() = 0; //is an ABC, cannot be directly created
        int getHitpoints() {return hitpoints_;}; //getter func for hp
        bool updateHitpoints(int hp); //if returns false, stop dosomething
        bool damage(int x, int y); //when pea hits obj
        void restoreHealth() {hitpoints_=20;}; //setter func for hp
    private:
        int deathSound_; //to play on death 
        int impactSound_; //to play on impact
        int hitpoints_;
};

class NotAlive : public Actor
{
    public: 
        NotAlive(double startX, double startY, int imageID, StudentWorld *world,int dir=none)
        :Actor(world, imageID, startX, startY,dir){};
        virtual void doSomething() = 0; //ABC
        virtual bool isAffectedByPea() {return false;};
        virtual bool damage(int x, int y) {return false;}; //non-living objects cannot be damaged
};

class Pit : public Actor
{
    public:   
        Pit(double startX, double startY, StudentWorld *world)
            :Actor(world, IID_PIT, startX, startY) {};
        virtual void doSomething();
        virtual bool canMarble() {return true;};
};

/*****************************
NotAlive Derived Classes: 
*****************************/
//Non Living Object: Pea
class Pea : public NotAlive
{
    public:
        Pea(double startX, double startY, StudentWorld *world, int dir);
        virtual void doSomething();

    private:
        int dir_;
};

class Goodie : public NotAlive
{
    public:
        Goodie(double startX, double startY, int imageID, StudentWorld *world, int pts);
        virtual bool canCollect() {return true;};
        void doSomething(); //check if avatar on same square, increase score and play sound
        int getPoints() {return points_;};
        virtual void pickUpGoodie() = 0; //specific pick up function
    private:
        int points_; //associated points
};

/*****************************
Living Derived Classes: 
*****************************/

//Living Object: Avatar
class Avatar : public Living
{
    public:
        Avatar(double startX, double startY, StudentWorld *world);
        virtual void doSomething(); 
        void addAmmo(int numPeas) {numPeas_+=numPeas;};
        int getAmmo() {return numPeas_;};
    private:
        int numPeas_;
};

class Marble : public Living
{
    public:
        Marble(double startX, double startY, StudentWorld *world)
            :Living(startX, startY, IID_MARBLE, world, 20, SOUND_NONE, SOUND_NONE) {};
        virtual void doSomething() {}; //does nothing each tick
        virtual bool canPush() {return true;};
};

class Bot : public Living
{
    public:
        Bot(double startX, double startY, int imageID, int hp, StudentWorld *world, int deathSound_, int impactSound_,int dir);
        bool isTick();
    private:
        int ticks_;
        int tickCt_;   
};

/*****************************
Goodie Derived Classes: 
*****************************/

class Crystal : public Goodie
{
    public:
        Crystal(double startX, double startY, StudentWorld *world)
            :Goodie(startX, startY, IID_CRYSTAL, world, 50) {}
        void pickUpGoodie(); //decrement crystal ct}
        virtual bool canCollect() {return false;};
};
class Exit : public Goodie
{
    public:
        Exit(double startX, double startY, StudentWorld *world)
            :Goodie(startX, startY, IID_EXIT, world, 2000) {active_=false;setVisible(false);};
        virtual void doSomething(); //custom doSomething class
        void pickUpGoodie() {};
        virtual bool canCollect() {return false;};
    private:
        bool active_;
};
class AmmoGoodie : public Goodie
{
    public:
        AmmoGoodie(double startX, double startY, StudentWorld *world)
            :Goodie(startX, startY, IID_AMMO, world, 100) {};
        void pickUpGoodie(); //decrement crystal ct}
          
};
class HealthGoodie : public Goodie
{
    public:
        HealthGoodie(double startX, double startY, StudentWorld *world)
            :Goodie(startX, startY, IID_RESTORE_HEALTH, world, 500) {};
        void pickUpGoodie(); //restore health
};
class ExtraLifeGoodie : public Goodie
{
    public:
        ExtraLifeGoodie(double startX, double startY, StudentWorld *world)
            :Goodie(startX, startY, IID_EXTRA_LIFE, world, 1000) {};
        void pickUpGoodie(); //add life 
};

/*****************************
Bot Derived Classes: 
*****************************/
//Living Object: Rage Bot
class RageBot : public Bot
{
    public:
        RageBot(double startX, double startY, StudentWorld *world, int dir)
            :Bot(startX, startY, IID_RAGEBOT, 10, world,  SOUND_ROBOT_IMPACT, SOUND_ROBOT_DIE, dir) {};
        virtual void doSomething();
        virtual void cleanUp();

};

class ThiefBot : public Bot
{
    public:
        ThiefBot(double startX, double startY, StudentWorld *world, int imageID=IID_THIEFBOT, int hp=5)
            :Bot(startX, startY, imageID, hp, world,  SOUND_ROBOT_IMPACT, SOUND_ROBOT_DIE, right) 
            {distBeforeTurning=rand()%6+1;goodie_=nullptr;distanceTraveled=0;};
        virtual ~ThiefBot();
        virtual void doSomething();
        virtual void cleanUp();
        void deleteGoodie();
        void collectGoodie(Actor *goodie);
        void stealOrMove();
        virtual bool isThiefBot() {return true;}; 
    private:
        int distBeforeTurning;
        int distanceTraveled;
        Actor *goodie_;

};
class MeanThiefBot : public ThiefBot
{
    public:
         MeanThiefBot(double startX, double startY, StudentWorld *world)
            :ThiefBot(startX, startY, world,IID_MEAN_THIEFBOT, 8){};
        virtual void doSomething(); 
        virtual void cleanUp();
};
#endif // ACTOR_H_
