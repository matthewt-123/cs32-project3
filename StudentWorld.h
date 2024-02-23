#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <vector>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor; 
class Avatar;

class StudentWorld : public GameWorld
{
public:
  //needed functions
  StudentWorld(std::string assetPath);
  ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();


  bool moveActor(Actor* actor, int newX, int newY); //check if actor can be moved to new square
  bool peaDamage(int startX, int startY, Actor* pea); //true if damaged false if pea died
  bool firePeaBot(int startX, int startY, int dir, int targetX, int targetY, Actor* Bot); //return true if able to fire, false if not
  void firePlayer(int startX, int startY, int dir);
  void collectCrystal() {crystalCt_--;};
  int remainingCrystals() {return crystalCt_;};
  void levelComplete() {levelFinish_=true;};
  Avatar *getAvatar() {return avatar_;};
  bool stealGoodie(Actor *thiefBot, int x, int y);
  void addActor(Actor *actor) {tmp_.push_back(actor);};
  int countThiefBots(double startX, double endX, double startY, double endY);
  void deleteActor(Actor* actor, bool toDelete) {delete_.insert({actor, toDelete});}; //mark element to be removed from actor array (NOT DELETED)
  bool moveMarble(Actor* marble, int newX, int newY);
  void checkPit(int x, int y, Actor *pit);
private:
  string formatString(); //format top text
  bool isPlayerInSight(int startX, int startY, Avatar *avatar_, int sDir, int targetX, int targetY, char dirSwitch, Actor* Bot);
  int loadLevel(); //load level and add all actors
  vector<Actor *> actors_; //vector of all actor
  Avatar *avatar_;
  int bonusPts_;
  int crystalCt_; //ct of crystals
  vector<Actor *> tmp_; //tmp array. cannot add to actors_ array while iterating
  map<Actor *, bool> delete_; //tmp array, cannot delete while iterating. map address to whether obj should be deleted
  bool levelFinish_; //whether a level has been finished
};

#endif // STUDENTWORLD_H_
