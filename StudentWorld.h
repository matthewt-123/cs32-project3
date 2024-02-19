#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"

#include "Level.h"
#include <string>
#include <vector>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
// class Actor; 
// class Avatar;

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();
  bool moveActor(Actor* actor, int newX, int newY);
  void peaDamage(int startX, int startY, Actor* pea);
  bool firePeaBot(int startX, int startY, int dir, int targetX, int targetY); //return true if able to fire, false if not
  Actor *getAvatar() {return avatar_;};
private:
  int loadLevel();
  vector<Actor *> actors_; //vector of all actor
  Avatar *avatar_;
  int bonusPts_;
};

#endif // STUDENTWORLD_H_
