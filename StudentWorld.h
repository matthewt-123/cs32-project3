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
  bool peaDamage(int startX, int startY, Actor* pea); //true if damaged false if pea died
  bool firePeaBot(int startX, int startY, int dir, int targetX, int targetY, Actor* Bot); //return true if able to fire, false if not
  void collectCrystal();
  int remainingCrystals() {return crystalCt_;};
  void levelComplete() {levelFinish_=true;};
  Actor *getAvatar() {return avatar_;};
private:
  int loadLevel();
  vector<Actor *> actors_; //vector of all actor
  Avatar *avatar_;
  int bonusPts_;
  int crystalCt_;
  bool levelFinish_;
};

#endif // STUDENTWORLD_H_
