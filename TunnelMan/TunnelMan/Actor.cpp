#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

#include <cmath>

// Actor

Actor::Actor(StudentWorld* sw, int imageID, int x, int y): GraphObject(imageID, x, y)
{
    m_world = sw;
    m_alive = true;
    m_imageID = imageID;
}

Actor::Actor(StudentWorld* sw, int imageID, int x, int y, Direction d, float s, unsigned int p): GraphObject(imageID, x, y, d, s, p)
{
    m_world = sw;
    m_alive = true;
    m_imageID = imageID;
}

Actor::~Actor() {}

StudentWorld* Actor::getWorld() {return m_world;}

void Actor::kill() {m_alive = false;}

bool Actor::isAlive() {return m_alive;}




// Earth

Earth::Earth(StudentWorld* sw, int x, int y): Actor(sw,TID_EARTH, x, y, right, 0.25, 3) {setVisible(true);}

Earth::~Earth() {}

void Earth::doSomething() {}





// TunnelMan

TunnelMan::TunnelMan(StudentWorld* sw): Actor(sw, TID_PLAYER, TUNNELMAN_START_X, TUNNELMAN_START_Y)
{
    m_health = 10;
    m_gold = 0;
    m_sonar = 1;
    m_water = 5;
    setVisible(true);
}

TunnelMan::~TunnelMan() {}


int TunnelMan::getHealth() {return m_health;}
int TunnelMan::getWater() {return m_water;}
int TunnelMan::getSonar() {return m_sonar;}
int TunnelMan::getGold() {return m_gold;}

void TunnelMan::incWater() {m_water += 5;}
void TunnelMan::incSonar() {m_sonar++;}
void TunnelMan::incGold() {m_gold++;}

void TunnelMan::damage(int hitPoints) {m_health -= hitPoints;}



void TunnelMan::doSomething() {
    // if dead, return without doing anything
    if (!isAlive())
        return;
    int ch;
    if(getWorld()->getKey(ch) == true)
    {
        switch (ch) {
            case KEY_PRESS_ESCAPE:
                kill();
                break;
                
            case KEY_PRESS_LEFT:
                if (getDirection() == left) // if already facing direction, move
                {
                    if (getX() == 0) // on left wall
                        moveTo(getX(), getY());
                    // check if boulder is present
                    else if (getWorld()->checkForBoulders(getX()-1, getY()))
                        moveTo(getX(), getY());
                    else
                        moveTo(getX()-1, getY());
                }
                else // otherwise face direction
                    setDirection(left);
                break;
                
            case KEY_PRESS_RIGHT:
                
                if (getDirection() == right)
                {
                    if (getX() == 60)
                        moveTo(getX(), getY());
                    // check if boulder is present
                    else if (getWorld()->checkForBoulders(getX()+1, getY()))
                        moveTo(getX(), getY());
                    else
                        moveTo(getX()+1, getY());
                }
                else
                    setDirection(right);
                break;
                
            case KEY_PRESS_UP:
                if (getDirection() == up)
                {
                    if (getY() == 60)
                        moveTo(getX(), getY());
                    // check if boulder is present
                    else if (getWorld()->checkForBoulders(getX(), getY()+1))
                        moveTo(getX(), getY());
                    else
                        moveTo(getX(), getY()+1);
                }
                else
                    setDirection(up);
                break;
                
            case KEY_PRESS_DOWN:
                if (getDirection() == down)
                {
                    if (getY() == 0)
                        moveTo(getX(), getY());
                    // check if boulder is present
                    else if (getWorld()->checkForBoulders(getX(), getY()-1))
                        moveTo(getX(), getY());
                    else
                        moveTo(getX(), getY()-1);
                }
                else
                    setDirection(down);
                break;
                
            case KEY_PRESS_SPACE:
                if (m_water != 0)
                {
                    int x = getX();
                    int y = getY();
                    if (getDirection() == up)
                        y += 4;
                    else if (getDirection() == down)
                        y -= 4;
                    else if (getDirection() == right)
                        x += 4;
                    else if (getDirection() == left)
                        x -= 4;
                    Squirt* s = new Squirt(getWorld(), x, y, getDirection());
                    getWorld()->shootSquirt(s);
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    m_water--;
                }
                break;
                
            case 'Z':
            case 'z':
                if (m_sonar != 0)
                {
                    getWorld()->useSonar(getX(), getY());
                    m_sonar--;
                }
                break;
                
            case KEY_PRESS_TAB:
                if (m_gold != 0)
                {
                    // add gold to world
                    Gold* g = new Gold(getWorld(), getX(), getY(), true);
                    getWorld()->dropGold(g);
                    m_gold--;
                }
                break;
                
        }
    }
    getWorld()->dig(getX(), getY()); // dig earth after moving
}




// Boulder

Boulder::Boulder(StudentWorld* sw, int x, int y): Actor(sw, TID_BOULDER, x, y, down, 1, 1)
{
    m_state = BOULDER_STABLE;
    setVisible(true);
}

Boulder::~Boulder() {}

int Boulder::getState() {return m_state;}

void Boulder::doSomething()
{
    // if dead, do nothing
    if (!isAlive())
        return;
    
    
    if (m_state == BOULDER_STABLE)
    {
        if (getWorld()->shouldBoulderFall(getX(), getY(), m_state))
        {
            m_state = BOULDER_WAITING;
            m_wait = getWorld()->getTick();
            return;
        }
    }
    
    
    if (m_state == BOULDER_WAITING)
    {
        int interval = getWorld()->getTick() - m_wait;
        if (interval <= 30)
        {
            return;
        }
        else// if 30 ticks have passed
        {
            m_state = BOULDER_FALLING;
            getWorld()->playSound(SOUND_FALLING_ROCK);
        }
    }
    
    if (m_state == BOULDER_FALLING)
    {
        if (getWorld()->shouldBoulderFall(getX(), getY(), m_state))
            moveTo(getX(), getY()-1);
        // once boulder has come to a rest
        else
        {
            kill();
            getWorld()->decNumBoulders(); // should be temporary measure
        }
    }
}


// Goodie

Goodie::Goodie(StudentWorld* sw, int imageID, int x, int y): Actor(sw, imageID, x, y, right, 1, 2) {m_ticks = 0;}

Goodie::~Goodie() {}

int Goodie::getTicks() {return m_ticks;}
void Goodie::incTicks() {m_ticks++;}


// Barrel

Barrel::Barrel(StudentWorld* sw, int x, int y): Goodie(sw, TID_BARREL, x, y) {setVisible(false);}

Barrel::~Barrel() {}

void Barrel::doSomething()
{
    // if dead, do nothing
    if (!isAlive())
        return;
    
    
    int tmX = getWorld()->getTunnelMan()->getX();
    int tmY = getWorld()->getTunnelMan()->getY();
    // check if TunnelMan is within radius 3.0
    if (getWorld()->withinRadius(getX(), getY(), tmX, tmY, 3.0))
    {
        kill();
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(POINTS_FIND_BARREL_OF_OIL);
        // tell studentworld a barrel was picked up
        getWorld()->decNumBarrels();
    }
    
    // check if TunnelMan is within radius 4.0
    if (getWorld()->withinRadius(getX(), getY(), tmX, tmY, 4.0))
    {
        setVisible(true);
        return;
    }
}


// Gold

Gold::Gold(StudentWorld*sw, int x, int y, bool dropped): Goodie(sw, TID_GOLD, x, y)
{
    m_dropped = dropped;
    setVisible(m_dropped); // if it has yet to be picked up, it is invisible; else, visible
}

Gold::~Gold() {}

void Gold::doSomething()
{
    // check if alive
    if (!isAlive())
        return;
    
    if (m_dropped == false) // pickup-able by TunnelMan
    {
        int tmX = getWorld()->getTunnelMan()->getX();
        int tmY = getWorld()->getTunnelMan()->getY();
        // check if TunnelMan is within radius 3.0
        if (getWorld()->withinRadius(getX(), getY(), tmX, tmY, 3.0))
        {
            kill();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            getWorld()->increaseScore(POINTS_FIND_GOLD_NUGGET);
            // tell tunnelman a gold nugget was picked up
            getWorld()->getTunnelMan()->incGold();
        }
        
        // check if TunnelMan is within radius 4.0
        if (getWorld()->withinRadius(getX(), getY(), tmX, tmY, 4.0))
        {
            setVisible(true);
            return;
        }
    }
    
    else // pickup-able by protesters
    {
        if (getWorld()->checkForProtesters(getX(), getY()) == true)
        {
            kill();
            getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
            // tell protester he's been bribed
            getWorld()->hitProtesters(getX(), getY(), -1);
        }
        
        // check if the Gold has run out of time
        if (getTicks() == 100)
            kill();
        
        incTicks();
    }
}


// Sonar


Sonar::Sonar(StudentWorld* sw, int x, int y): Goodie(sw, TID_SONAR, x, y) {setVisible(true);}

Sonar::~Sonar() {}

void Sonar::doSomething()
{
    // check if alive
    if (!isAlive())
        return;
    
    
    if (getWorld()->checkForTunnelMan(getX(), getY()) == true) // if TunnelMan is in range
    {
        kill();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getTunnelMan()->incSonar();
        getWorld()->increaseScore(POINTS_FIND_SONAR_KIT);
    }
    
    // check if the Sonar Kit has run out of time
    int maxTicks = fmax(100, 300 - 10 * getWorld()->getLevel());
    if (getTicks() == maxTicks)
        kill();
    
    incTicks();
}

// Water

Water::Water(StudentWorld* sw, int x, int y): Goodie(sw, TID_WATER_POOL, x, y) {setVisible(true);}

Water::~Water() {}

void Water::doSomething()
{
    // check if alive
    if (!isAlive())
        return;
    
    
    if (getWorld()->checkForTunnelMan(getX(), getY()) == true) // if TunnelMan is in range
    {
        kill();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getTunnelMan()->incWater();
        getWorld()->increaseScore(POINTS_FIND_WATER);
    }
    
    // check if the Water has run out of time
    int maxTicks = fmax(100, 300 - 10 * getWorld()->getLevel());
    if (getTicks() == maxTicks)
        kill();
    
    incTicks();
}


// Squirt

Squirt::Squirt(StudentWorld* sw, int x, int y, Direction d): Actor(sw, TID_WATER_SPURT, x, y, d, 1, 1)
{
    m_travelDistance = 4;
    setVisible(true);
}

Squirt::~Squirt() {}

void Squirt::doSomething()
{
    if (!isAlive())
        return;
    
    // if protester(s) are within range
    if (getWorld()->checkForProtesters(getX(), getY()))
    {
        getWorld()->hitProtesters(getX(), getY(), 2);
        kill();
        return;
    }
    
    // if travel distance reached
    if (m_travelDistance == 0)
    {
        kill();
        return;
    }
    
    m_travelDistance--;
    
    // move one step in direction
    switch (getDirection()) {
        case up:
            if (getWorld()->checkXPath(getX(), getY(), getY()+1))
                moveTo(getX(), getY()+1);
            else
                kill();
            break;
            
        case down:
            if (getWorld()->checkXPath(getX(), getY(), getY()-1))
                moveTo(getX(), getY()-1);
            else
                kill();
            break;
            
        case right:
            if (getWorld()->checkYPath(getY(), getX(), getX()+1))
                moveTo(getX()+1, getY());
            else
                kill();
            break;
            
        case left:
            if (getWorld()->checkYPath(getY(), getX(), getX()-1))
                moveTo(getX()-1, getY());
            else
                kill();
            break;
            
        default:
            break;
    }
}


// Protester

Protester::Protester(StudentWorld* sw, int imageID, bool hardcore)
    : Actor(sw, imageID, PROTESTER_START_X, PROTESTER_START_Y, left, 1, 0)
{
    m_hardcore = hardcore;
    if (m_hardcore)
        m_health = 20;
    else
        m_health = 5;
    m_bribed = false;
    m_leaveOilField = false;
    m_restTicks = fmax(0, 3 - getWorld()->getLevel()/4);
    m_ticksSinceLastShout = 0;
    m_ticksSincePerpendicular = 0;
    m_numSquaresInDirection = 8 + rand() % 53; // 8 <= m_nSTMICD <= 60
    setVisible(true);
    
    // populate oilField array
    for (int x = 0; x < VIEW_WIDTH; x++)
        for (int y = 0; y < VIEW_HEIGHT; y++)
            m_oilField[x][y] = false;
}

Protester::~Protester() {}

void Protester::doSomething()
{
    // check if protester is dead
    if (!isAlive())
        return;
    
    bribe();
    
    // check if protester has given up
    if (!isLeaving() && getHealth() <= 0)
    {
        leaveOilField();
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        setRestTicks(0);
        if (m_hardcore) // adjusting point total accordingly
            getWorld()->increaseScore(POINTS_HARDCORE_PROTESTER_GIVES_UP);
        else
            getWorld()->increaseScore(POINTS_REGULAR_PROTESTER_GIVES_UP);
        findPathToExit(getX(), getY()); // find path to exit
    }
    
    // check if protester is resting
    if (getRestTicks() != 0 && getWorld()->getTick() % getRestTicks() != 0)
        return;
    
    else if (getRestTicks() == fmax(50, 100 - getWorld()->getLevel()*10)) // resetting from stunned state
        setRestTicks(fmax(0, 3 - getWorld()->getLevel()/4));
    
    // count active tick without shouting
    incTicksSinceLastShout();
    // count active tick without moving in perpendicular direction
    incTicksSincePerpendicular();
    
    // if protester is annoyed and leaving
    if (isLeaving())
    {
        // check if at exit point
        if (getX() == PROTESTER_START_X && getY() == PROTESTER_START_Y)
            kill();
        else
            moveToExit();
        return;
    }
    
    
    // getting TunnelMan coordinates
    int tX = getWorld()->getTunnelMan()->getX();
    int tY = getWorld()->getTunnelMan()->getY();
    
    int dX = tX - getX(); // +ve = right; -ve = left
    int dY = tY - getY(); // +ve = up; -ve = down
    
    // if TunnelMan is in shouting range
    if (abs(dX) <= 4 && abs(dY) <= 4)
        if ((dX == 4 && getDirection() == right) || (dX == -4 && getDirection() == left)
            || (dY == 4 && getDirection() == up) || (dY == -4 && getDirection() == down))
            if (getTicksSinceLastShout() > 15)
            {
                getWorld()->playSound(SOUND_PROTESTER_YELL);
                getWorld()->getTunnelMan()->damage(2);
                resetTicksSinceLastShout();
                return;
            }
          
    // special Hardcore Protester TunnelMan tracker
    if ((abs(dX) <= 4 && abs(dY) <= 4) == false)
        if (trackTunnelMan()) // regular protester always returns false, does nothing
            return;
    
    
    // if TunnelMan is in the protester's line of sight
    if ((dY == 0 && abs(dX) > 4) || (dX == 0 && abs(dY) > 4))
    {
        if (dX == 0) // potential vertical line of sight
        {
            // see if path is clear
            if (getWorld()->checkXPath(getX(), getY(), tY))
            {
                if (dY > 0) // move up
                {
                    setDirection(up);
                    moveTo(getX(), getY()+1);
                    zeroNumSquaresInDirection();
                }
                else // move down
                {
                    setDirection(down);
                    moveTo(getX(), getY()-1);
                    zeroNumSquaresInDirection();
                }
                return;
            }
        }
        
        else if (dY == 0) // potential horizontal line of sight
        {
            // see if path is clear
            if (getWorld()->checkYPath(getY(), getX(), tX))
            {
                if (dX > 0) // move right
                {
                    setDirection(right);
                    moveTo(getX()+1, getY());
                    zeroNumSquaresInDirection();
                }
                else // move left
                {
                    setDirection(left);
                    moveTo(getX()-1, getY());
                    zeroNumSquaresInDirection();
                }
                return;
            }
        }
    }
    
    
    // if TunnelMan is not in any line of sight
    decNumSquaresInDirection();
    
    
    // check if protester needs a new direction
    if (getNumSquaresInDirection() <= 0)
    {
        bool foundNewDirection = false;
        while (!foundNewDirection)
        {
            // picking a random new direction
            int direction = 1 + rand() % 4; // 1 = UP; 2 = DOWN; 3 = RIGHT; 4 = LEFT;
            
            if (direction == 1 && getWorld()->checkXPath(getX(), getY(), getY()+1)) // UP
            {
                setDirection(up);
                resetNumSquaresInDirection();
                foundNewDirection = true;
            }
            
            else if (direction == 2 && getWorld()->checkXPath(getX(), getY(), getY()-1)) // DOWN
            {
                setDirection(down);
                resetNumSquaresInDirection();
                foundNewDirection = true;
            }
            
            else if (direction == 3 && getWorld()->checkYPath(getY(), getX(), getX()+1)) // RIGHT
            {
                setDirection(right);
                resetNumSquaresInDirection();
                foundNewDirection = true;
            }
            
            else if (direction == 4 && getWorld()->checkYPath(getY(), getX(), getX()-1)) // LEFT
            {
                setDirection(left);
                resetNumSquaresInDirection();
                foundNewDirection = true;
            }
        }
    }
    
    // check if perpendicular paths are available
    else if (getTicksSincePerpendicular() > 200)
    {
        if (getDirection() == right || getDirection() == left)
        {
            if (getWorld()->checkXPath(getX(), getY(), getY()+1) ||
                getWorld()->checkXPath(getX(), getY(), getY()-1)) // if upward or downward path is open
            {
                if (getWorld()->checkXPath(getX(), getY(), getY()+1) &&
                    getWorld()->checkXPath(getX(), getY(), getY()-1)) // both paths are open
                    {
                        int direction = 1 + rand() % 2; // 1 = UP; 2 = DOWN;
                        if (direction == 1) // UP
                        {
                            setDirection(up);
                            resetNumSquaresInDirection();
                            resetTicksSincePerpendicular();
                        }
                        else // DOWN
                        {
                            setDirection(down);
                            resetNumSquaresInDirection();
                            resetTicksSincePerpendicular();
                        }
                    }
                
                else if (getWorld()->checkXPath(getX(), getY(), getY()+1)) // upward path only
                {
                    setDirection(up);
                    resetNumSquaresInDirection();
                    resetTicksSincePerpendicular();
                }
                else // downward path only
                {
                    setDirection(down);
                    resetNumSquaresInDirection();
                    resetTicksSincePerpendicular();
                }
                    
            }
        }
        else if (getDirection() == up || getDirection() == down)
        {
            if (getWorld()->checkYPath(getY(), getX(), getX()+1) ||
                getWorld()->checkYPath(getY(), getX(), getX()-1)) // if right or left path are open
            {
                if (getWorld()->checkYPath(getY(), getX(), getX()+1) &&
                getWorld()->checkYPath(getY(), getX(), getX()-1)) // both paths are open
                {
                    int direction = 3 + rand() % 2; // 3 = RIGHT; 4 = LEFT;
                    if (direction == 3) // RIGHT
                    {
                        setDirection(right);
                        resetNumSquaresInDirection();
                        resetTicksSincePerpendicular();
                    }
                    else // LEFT
                    {
                        setDirection(left);
                        resetNumSquaresInDirection();
                        resetTicksSincePerpendicular();
                    }
                }
                else if (getWorld()->checkYPath(getY(), getX(), getX()+1)) // rightward path only
                {
                    setDirection(right);
                    resetNumSquaresInDirection();
                    resetTicksSincePerpendicular();
                }
                else // leftward path only
                {
                    setDirection(left);
                    resetNumSquaresInDirection();
                    resetTicksSincePerpendicular();
                }
            }
        }
    }
    
    // move one step in direction
    switch (getDirection()) {
        case up:
            if (getWorld()->checkXPath(getX(), getY(), getY()+1))
                moveTo(getX(), getY()+1);
            else
                zeroNumSquaresInDirection();
            break;
            
        case down:
            if (getWorld()->checkXPath(getX(), getY(), getY()-1))
                moveTo(getX(), getY()-1);
            else
                zeroNumSquaresInDirection();
            break;
            
        case right:
            if (getWorld()->checkYPath(getY(), getX(), getX()+1))
                moveTo(getX()+1, getY());
            else
                zeroNumSquaresInDirection();
            break;
            
        case left:
            if (getWorld()->checkYPath(getY(), getX(), getX()-1))
                moveTo(getX()-1, getY());
            else
                zeroNumSquaresInDirection();
            break;
            
        default:
            break;
    }
}

int Protester::getHealth() {return m_health;}
void Protester::damage(int hitPoints) {m_health -= hitPoints;}

bool Protester::wasBribed() {return m_bribed;}
void Protester::setBribed(bool b) {m_bribed = b;}

int Protester::getRestTicks() {return m_restTicks;}
void Protester::setRestTicks(int ticks) {m_restTicks = ticks;}

int Protester::getTicksSinceLastShout() {return m_ticksSinceLastShout;}
void Protester::incTicksSinceLastShout() {m_ticksSinceLastShout++;}
void Protester::resetTicksSinceLastShout() {m_ticksSinceLastShout = 0;}

int Protester::getTicksSincePerpendicular() {return m_ticksSincePerpendicular;}
void Protester::incTicksSincePerpendicular() {m_ticksSincePerpendicular++;}
void Protester::resetTicksSincePerpendicular() {m_ticksSincePerpendicular = 0;}

int Protester::getNumSquaresInDirection() {return m_numSquaresInDirection;}
void Protester::decNumSquaresInDirection() {m_numSquaresInDirection--;}
void Protester::zeroNumSquaresInDirection() {m_numSquaresInDirection = 0;}
void Protester::resetNumSquaresInDirection() {m_numSquaresInDirection = 8 + rand() % 53;}

bool Protester::isLeaving() {return m_leaveOilField;}
void Protester::leaveOilField() {m_leaveOilField = true;}


// function to find a path to the exit for the protester
bool Protester::findPathToExit(int x, int y)
{
    // put current point's coordinates into coord struct
    coord point;
    point.x = x;
    point.y = y;
    
    m_oilField[point.x][point.y] = true; // mark point as discovered
    
    // check if endpoint has been reached (base case)
    if (point.x == PROTESTER_START_X && point.y == PROTESTER_START_Y)
    {
        m_path.push(point); // push point onto stack
        return true; // returning true indicates this is part of the path
    }
    
    // creating booleans for each possible path direction
    bool p1 = false, p2 = false, p3 = false, p4 = false;
    
    // recursively call function for each direction
    if (m_oilField[point.x][point.y+1] == false && getWorld()->checkXPath(point.x, point.y, point.y+1)) // UP
        p1 = findPathToExit(point.x, point.y+1);
    
    if (m_oilField[point.x+1][point.y] == false && getWorld()->checkYPath(point.y, point.x, point.x+1)) // RIGHT
        p2 = findPathToExit(point.x+1, point.y);
    
    if (m_oilField[point.x][point.y-1] == false && getWorld()->checkXPath(point.x, point.y, point.y-1)) // DOWN
        p3 = findPathToExit(point.x, point.y-1);
        
    if (m_oilField[point.x-1][point.y] == false && getWorld()->checkYPath(point.y, point.x, point.x-1)) // LEFT
        p4 = findPathToExit(point.x-1, point.y);
    
    // if any of the paths return true, that means that the endpoint was found at the end of it
    // if so, we should push this point onto the stack
    if (p1 || p2 || p3 || p4)
        m_path.push(point);
    
    // if this point is also on the path, returns true; otherwise, returns false
    return (p1 || p2 || p3 || p4);
}


void Protester::moveToExit()
{
    if (m_path.empty())
        findPathToExit(getX(), getY());
    // copy coordinates of top point
    coord point = m_path.top();
    m_path.pop();
    
    if (point.y > getY()) // UP
        setDirection(up);
    else if (point.y < getY()) // DOWN
        setDirection(down);
    else if (point.x > getX()) // RIGHT
        setDirection(right);
    else if (point.x < getX()) // LEFT
        setDirection(left);
    
    moveTo(point.x, point.y);
}



// Regular Protester

regularProtester::regularProtester(StudentWorld* sw): Protester(sw, TID_PROTESTER, false) {}

regularProtester::~regularProtester() {}

// overloaded function to react to being bribed
void regularProtester::bribe()
{
    // check if the protester was bribed
    if (!isLeaving() && wasBribed())
    {
        getWorld()->increaseScore(POINTS_BRIBE_REGULAR_PROTESTER);
        leaveOilField();
        setBribed(false);
    }
}

// overloaded function does nothing
bool regularProtester::trackTunnelMan() {return false;}



// Hardcore Protester

hardcoreProtester::hardcoreProtester(StudentWorld* sw): Protester(sw, TID_HARD_CORE_PROTESTER, true)
{
    // populate oilField array
    for (int x = 0; x < VIEW_WIDTH; x++)
        for (int y = 0; y < VIEW_HEIGHT; y++)
            m_hardcoreOilField[x][y] = false;
}

hardcoreProtester::~hardcoreProtester() {}

// overloaded function to react to being bribed
void hardcoreProtester::bribe()
{
    // check if the protester was bribed
    if (wasBribed())
    {
        getWorld()->increaseScore(POINTS_BRIBE_HARDCORE_PROTESTER);
        setRestTicks(fmax(50, 100 - getWorld()->getLevel()*10));
        setBribed(false);
    }
}

// overloaded function to track TunnelMan
bool hardcoreProtester::trackTunnelMan()
{
    // reset oilField array
    for (int x = 0; x < VIEW_WIDTH; x++)
        for (int y = 0; y < VIEW_HEIGHT; y++)
            m_hardcoreOilField[x][y] = false;
    
    int moves = -1; // so first position is move 0
    int maxMoves = 16 + getWorld()->getLevel() * 2;
    bool pathExists = findPathToTunnelMan(getX(), getY(), getWorld()->getTunnelMan()->getX(), getWorld()->getTunnelMan()->getY(), moves, maxMoves);
    if (pathExists) // if such a path exists
    {
        moveToTunnelMan();
        return true;
    }
    
    return false;
}

// function to find path to TunnelMan for hardcore protester
bool hardcoreProtester::findPathToTunnelMan(int x, int y, int tX, int tY, int moves, int maxMoves)
{
    moves++;
    if (moves == maxMoves) // out of range (base case)
        return false;
    
    // put current point's coordinates into coord struct
    coord point;
    point.x = x;
    point.y = y;
    
    m_hardcoreOilField[point.x][point.y] = true; // mark point as discovered
    
    // check if TunnelMan has been reached (base case)
    if (point.x == tX && point.y == tY)
    {
        m_hardcorePath.push(point); // push point onto stack
        return true; // returning true indicates this is part of the path
    }
    
    // creating booleans for each possible path direction
    bool p1 = false, p2 = false, p3 = false, p4 = false;
    
    // recursively call function for each direction
    if (m_hardcoreOilField[point.x][point.y+1] == false && getWorld()->checkXPath(point.x, point.y, point.y+1)) // UP
        p1 = findPathToTunnelMan(point.x, point.y+1, tX, tY, moves, maxMoves);
    
    if (m_hardcoreOilField[point.x+1][point.y] == false && getWorld()->checkYPath(point.y, point.x, point.x+1)) // RIGHT
        p2 = findPathToTunnelMan(point.x+1, point.y, tX, tY, moves, maxMoves);
    
    if (m_hardcoreOilField[point.x][point.y-1] == false && getWorld()->checkXPath(point.x, point.y, point.y-1)) // DOWN
        p3 = findPathToTunnelMan(point.x, point.y-1, tX, tY, moves, maxMoves);
        
    if (m_hardcoreOilField[point.x-1][point.y] == false && getWorld()->checkYPath(point.y, point.x, point.x-1)) // LEFT
        p4 = findPathToTunnelMan(point.x-1, point.y, tX, tY, moves, maxMoves);
    
    // if any of the paths return true, that means that the endpoint was found at the end of it
    // if so, we should push this point onto the stack
    if ((p1 || p2 || p3 || p4) == true && moves != 0)
        m_hardcorePath.push(point);
    
    // if this point is also on the path, returns true; otherwise, returns false
    return (p1 || p2 || p3 || p4);
}


void hardcoreProtester::moveToTunnelMan()
{
    // copy coordinates of top point
    coord point = m_hardcorePath.top();
    m_hardcorePath.pop();
    
    if (point.y > getY()) // UP
        setDirection(up);
    else if (point.y < getY()) // DOWN
        setDirection(down);
    else if (point.x > getX()) // RIGHT
        setDirection(right);
    else if (point.x < getX()) // LEFT
        setDirection(left);
    
    moveTo(point.x, point.y);
}
