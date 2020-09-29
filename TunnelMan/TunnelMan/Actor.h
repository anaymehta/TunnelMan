#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#include <stack>

const int BOULDER_STABLE = 0;
const int BOULDER_WAITING = 1;
const int BOULDER_FALLING = 2;

const int TUNNELMAN_START_X = 30;
const int TUNNELMAN_START_Y = 60;

const int PROTESTER_START_X = 60;
const int PROTESTER_START_Y = 60;

class StudentWorld;

class Actor: public GraphObject
{
public:
    // constructors
    Actor(StudentWorld* sw, int imageID, int x, int y);
    Actor(StudentWorld* sw, int imageID, int x, int y, Direction d, float s, unsigned int p);
    // destructor
    virtual ~Actor();

    // virtual method doSomething()
    virtual void doSomething() = 0;
    
    // returns pointer to StudentWorld
    StudentWorld* getWorld();
    
    // kills Actor
    void kill();
    // checks if Actor is still alive
    bool isAlive();
    
private:
    StudentWorld* m_world;
    bool m_alive;
    int m_imageID;
};

// Earth
class Earth: public Actor
{
public:
    // constructor
    Earth(StudentWorld* sw, int x, int y);
    // destructor
    virtual ~Earth();
    // does nothing
    virtual void doSomething();
};

// TunnelMan
class TunnelMan: public Actor
{
public:
    // constructor
    TunnelMan(StudentWorld* sw);
    // destructor
    virtual ~TunnelMan();
    
    // accessor and mutator methods
    int getHealth();
    int getWater();
    int getSonar();
    int getGold();
    void incWater();
    void incSonar();
    void incGold();
    
    // registers damage to TunnelMan by protesters or boulders
    void damage(int hitPoints);
    
    // doSomething(): lets user pick action
    virtual void doSomething();
    
private:
    int m_health;
    int m_water;
    int m_sonar;
    int m_gold;
};

// Boulder
class Boulder: public Actor
{
public:
    // constructor
    Boulder(StudentWorld* sw, int x, int y);
    // destructor
    virtual ~Boulder();
    
    // returns boulder's state (stable, waiting, falling)
    int getState();
    
    // deals with boulder falling
    virtual void doSomething();
    
private:
    int m_state; // represent boulder's state
    int m_wait; // used to measure out 30 waiting state ticks
};


// Goodie
class Goodie: public Actor
{
public:
    // constructor
    Goodie(StudentWorld* sw, int imageID, int x, int y);
    // destructor
    virtual ~Goodie();
    // pure virtual function
    virtual void doSomething() = 0;
    
    // accessor and mutator for counting ticks
    int getTicks();
    void incTicks();
    
private:
    int m_ticks;
};


// Barrel
class Barrel: public Goodie
{
public:
    // constructor
    Barrel(StudentWorld* sw, int x, int y);
    // destructor
    virtual ~Barrel();
    // overloaded function to do stuff
    virtual void doSomething();
};


// Gold

class Gold: public Goodie
{
public:
    // constructor
    Gold(StudentWorld* sw, int x, int y, bool dropped);
    // destructor
    virtual ~Gold();
    // overloaded function to do stuff
    virtual void doSomething();
    
private:
    bool m_dropped;
};


// Sonar

class Sonar: public Goodie
{
public:
    // constructor
    Sonar(StudentWorld* sw, int x, int y);
    // destructor
    virtual ~Sonar();
    // overloaded function to do stuff
    virtual void doSomething();
};

// Water

class Water: public Goodie
{
public:
    // constructor
    Water(StudentWorld* sw, int x, int y);
    // destructor
    virtual ~Water();
    // overloaded function to do stuff
    virtual void doSomething();
};



// Squirt

class Squirt: public Actor
{
public:
    // constructor
    Squirt(StudentWorld* sw, int x, int y, Direction d);
    // destructor
    virtual ~Squirt();
    // overloaded function to do stuff
    virtual void doSomething();
private:
    int m_travelDistance; // starts at 4 and decrements
};



// Protester

class Protester: public Actor
{
public:
    // constructor
    Protester(StudentWorld* sw, int imageID, bool hardcore);
    // destructor
    virtual ~Protester();

    // overloaded function to carry out all common protester actions
    virtual void doSomething();
    // pure virtual function to be overloaded by derived class implementation for taking bribe
    virtual void bribe() = 0;
    // pure virtual function to be overloaded by derived class implementation for tracking TunnelMan
    virtual bool trackTunnelMan() = 0;
    
    // accessor method
    int getHealth();
    // registers damage to protester by TunnelMan or boulders
    void damage(int hitPoints);
    
    // accessor and mutator methods
    bool wasBribed();
    void setBribed(bool b);
    // accessor and mutator methods
    int getRestTicks();
    void setRestTicks(int ticks);
    // accessor and mutator methods
    bool isLeaving();
    void leaveOilField();
    
    struct coord // struct for coordinates of each point
    {
        int x;
        int y;
    };
        
private:
    // accessor and mutator methods (and reset/zeroing methods)
    int getTicksSinceLastShout();
    void incTicksSinceLastShout();
    void resetTicksSinceLastShout();
    
    int getTicksSincePerpendicular();
    void incTicksSincePerpendicular();
    void resetTicksSincePerpendicular();
    
    int getNumSquaresInDirection();
    void decNumSquaresInDirection(); // decrement by one
    void zeroNumSquaresInDirection(); // set to zero
    void resetNumSquaresInDirection();
    
    // uses depth first algorithm and stack to find path to exit for protester
    bool findPathToExit(int x, int y);
    // moves one step toward exit after finding path in previous step
    void moveToExit();
    
    int m_health;
    bool m_hardcore;
    bool m_bribed;
    bool m_leaveOilField; // to indicate need to return to (60,60) [via queue-based maze algorithm]
    int m_restTicks;
    int m_ticksSinceLastShout;
    int m_ticksSincePerpendicular;
    int m_numSquaresInDirection;
    
    // Queue-based exit algorithm
    bool m_oilField[VIEW_WIDTH][VIEW_HEIGHT];
    std::stack <coord> m_path;
};


// Regular Protester

class regularProtester: public Protester
{
public:
    // constructor
    regularProtester(StudentWorld* sw);
    // destructor
    virtual ~regularProtester();
    // overloaded function takes bribe and leaves oil field (regular protester)
    virtual void bribe();
    // overloaded functon does nothing
    virtual bool trackTunnelMan();
};

// Hardcore Protester

class hardcoreProtester: public Protester
{
public:
    // constructor
    hardcoreProtester(StudentWorld* sw);
    // destructor
    virtual ~hardcoreProtester();
    // overloaded function takes bribe and stares it at for some time, then continues (hardcore protester)
    virtual void bribe();
    // overloaded function tracks tunnelman if he is reachable within a certain number of legal moves
    virtual bool trackTunnelMan();
    
private:
    // finds path to tunnelman within maxMoves
    bool findPathToTunnelMan(int x, int y, int tX, int tY, int moves, int maxMoves);
    // takes step toward tunnelman
    void moveToTunnelMan();
    
    bool m_hardcoreOilField[VIEW_WIDTH][VIEW_HEIGHT];
    std::stack <coord> m_hardcorePath;
};

#endif // ACTOR_H_
