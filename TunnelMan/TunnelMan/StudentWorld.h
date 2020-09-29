#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

#include <vector>

class Actor;
class TunnelMan;
class Earth;
class Boulder;
class Protester;
class Goodie;
class Gold;
class Squirt;

const unsigned int POINTS_REGULAR_PROTESTER_GIVES_UP = 100;
const unsigned int POINTS_HARDCORE_PROTESTER_GIVES_UP = 250;
const unsigned int POINTS_DROP_BOULDER_ON_PROTESTER = 500;
const unsigned int POINTS_BRIBE_REGULAR_PROTESTER = 25;
const unsigned int POINTS_BRIBE_HARDCORE_PROTESTER = 50;
const unsigned int POINTS_FIND_BARREL_OF_OIL = 1000;
const unsigned int POINTS_FIND_GOLD_NUGGET = 10;
const unsigned int POINTS_FIND_WATER = 100;
const unsigned int POINTS_FIND_SONAR_KIT = 75;



class StudentWorld : public GameWorld
{
public:
    // constructor
    StudentWorld(std::string assetDir);
    
    // destructor
    ~StudentWorld();
    
    // withinRadius takes the coordinates of two objects and a radius, then calculates the distance between the centers of the two objects and checks to see if the distance is smaller than the radius
    // returns true if within given radius, returns false otherwise
    // checks if the actor will be within a given radius of an existing object
    bool withinRadius(double objX, double objY, double x, double y, double radius);
    
    // check if Actor is within radius N of calling object (whose coordinates are passed), and return true if this is the case
    // returns true if TunnelMan is within radius 3 of calling object
    bool checkForTunnelMan(int x, int y);
    // check if Protester is within radius 3 of calling object
    bool checkForProtesters(int x, int y);
    // returns true if other actors are present, false otherwise
    // used for placing actors; prevents overlap
    // check radius of 6 around each actor
    bool checkForActors(int x, int y);
    // returns true if other boulders are present, false otherwise
    // used by TunnelMan (and Protesters perhaps) in moving
    // check radius of 3 around boulder
    bool checkForBoulders(int x, int y);
    // returns true if earth is present at (x,y), false otherwise
    bool checkForEarth(int x, int y);
    
    // passes in coordinates of Actor doing the hitting (normally after checking if a protester is in range) and the damage it's doing
    // function hits protesters and performs the appropriate functions (plays sounds, increases score, etc.)
    // based on damage, determines what hit the protester and acts appropriately
    void hitProtesters(int x, int y, int damage);
    // returns true if the falling boulder (whose coordinates are passed) will collide with another boulder
    bool bouldersCollide(int x, int y);
    // returns true if boulder should fall, false otherwise; boulderStatus determines course of action
    bool shouldBoulderFall(int x, int y, int boulderStatus);
    // for digging earth when moving/inserting boulders, TunnelMan moves, etc.
    // digs 4x4 grid with passes coordinates as bottom left corner
    void dig(int dX, int dY);
    
    // drop gold nugget (add to actors STL container)
    void dropGold(Gold* g);
    // add the squirt object to the actors vector, after ensuring that there are no obstructions
    void shootSquirt(Squirt* s);
    // use sonar kit to search for gold and barrels (within radius 12 from passed coordinates)
    void useSonar(int x, int y);
    // return pointer to TunnelMan
    TunnelMan* getTunnelMan();
    // return current tick
    int getTick();
    
    // checking to see if there's a clear path between two x or y coordinates
    bool checkXPath(int x, int y1, int y2);
    bool checkYPath(int y, int x1, int x2);
    
    // for maintaining count of remaining actors
    void decNumBoulders();
    void decNumBarrels();
    void decNumProtesters();
    
    // startup game, initialize oilfield and actors
    virtual int init();
    // allow actions for each tick
    virtual int move();
    // free up memory by removing actors that're no longer required
    virtual void cleanUp();

private:
    // self-explanatory
    void addNewActors();
    void allActorsDoSomething();
    void removeDeadActors();
    void setDisplayText();
    
    // keep track of Earth and TunnelMan objects
    TunnelMan* m_TunnelMan;
    Earth* m_Earth[VIEW_WIDTH][VIEW_HEIGHT];
    // keep track of everything else
    std::vector<Actor*> m_Actors;
    std::vector<Boulder*> m_Boulders;
    std::vector<Protester*> m_Protesters;
    std::vector<Goodie*> m_Goodies;
    // keep track of level counts for actors
    int m_numBoulders;
    int m_numBarrels;
    int m_numGoldNuggets;
    int m_numProtesters;
    // count ticks
    int m_ticks;
    int m_ticksSinceProtester;
};

#endif // STUDENTWORLD_H_
