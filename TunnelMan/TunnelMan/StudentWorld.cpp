#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

#include "Actor.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>


StudentWorld::StudentWorld(std::string assetDir): GameWorld(assetDir) {}

StudentWorld::~StudentWorld()
{
    // delete TunnelMan and Earth objects; similar to cleanUp()
    delete m_TunnelMan;
    for (int x = 0; x < VIEW_WIDTH; x++)
        for (int y = 0; y < 60; y++)
            if (!(x >= 30 && x <= 33 && y >= 4 && y <= 59)) // excluding central tunnel
                delete m_Earth[x][y];
    // go through vector and delete all items
    for (int i = 0; i < m_Actors.size(); i++)
    {
        delete m_Actors[i];
        m_Actors.erase(m_Actors.begin() + i);
        i--;
    }
}

// returns true if within given radius, returns false otherwise
// checks if the actor will be within a given radius of an existing object
bool StudentWorld::withinRadius(double objX, double objY, double x, double y, double radius)
{
    // getting the center coordinates of object and actor
    objX += 2;
    objY += 2;
    x += 2;
    y += 2;
    // measuring radius
    double dX = objX - x;
    double dY = objY - y;
    double r = dX*dX + dY*dY;
    if (r <= pow(radius, 2))
        return true;
    else
        return false;
}



// check if TunnelMan is within radius 3 of calling object
bool StudentWorld::checkForTunnelMan(int x, int y)
{
    int aX = getTunnelMan()->getX();
    int aY = getTunnelMan()->getY();
    // check for TunnelMan
    if (withinRadius(aX, aY, x, y, 3.0))
        return true;
    else
        return false;
}

// check if Protester is within radius 3 of calling object
bool StudentWorld::checkForProtesters(int x, int y)
{
    for (int i = 0; i < m_Protesters.size(); i++)
    {
        if (m_Protesters[i]->isLeaving())
            continue; // these protesters can't be affected further
        int pX = m_Protesters[i]->getX();
        int pY = m_Protesters[i]->getY();
        // if a protester is occupying the space
        if (withinRadius(pX, pY, x, y, 3.0))
            return true;
    }
    return false;
}

// returns true if other actors are present, false otherwise
// used for placing actors; prevents overlap
// check radius of 6 around each actor
bool StudentWorld::checkForActors(int x, int y)
{
    for (int i = 0; i < m_Actors.size(); i++)
    {
        int aX = m_Actors[i]->getX();
        int aY = m_Actors[i]->getY();
        // if another actor is occupying the space
        if (withinRadius(aX, aY, x, y, 6.0))
            return true;
    }
    return false;
}

// returns true if other boulders are present, false otherwise
// used by TunnelMan (and Protesters perhaps) in moving
// check radius of 3 around boulder
bool StudentWorld::checkForBoulders(int x, int y)
{
    // since Boulders are always the first actors added
    for (int i = 0; i < m_Boulders.size(); i++)
    {
        if (m_Boulders[i]->getState() == BOULDER_FALLING)
            continue;
        int bX = m_Boulders[i]->getX();
        int bY = m_Boulders[i]->getY();
        // if a boulder is within the given radius
        if (withinRadius(bX, bY, x, y, 3.0))
            return true;
    }
    return false;
}

// returns true if earth is present, false otherwise
bool StudentWorld::checkForEarth(int x, int y)
{
    // check if Earth is present
    if (m_Earth[x][y] != nullptr)
        if (m_Earth[x][y]->isAlive())
            return true;
    // if not, return false
    return false;
}


// function hits protesters and performs the appropriate functions (plays sounds, increases score, etc.)
// based on damage, determines what hit the protester and acts appropriately
void StudentWorld::hitProtesters(int x, int y, int damage)
{
    for (int i = 0; i < m_Protesters.size(); i++)
    {
        if (m_Protesters[i]->isLeaving())
            continue; // these protesters can't be annoyed further
        int pX = m_Protesters[i]->getX();
        int pY = m_Protesters[i]->getY();
        // if protester is occupying the space
        if (withinRadius(pX, pY, x, y, 3.0))
            {
                if (damage == 100) // boulder
                {
                    m_Protesters[i]->damage(100);
                    increaseScore(POINTS_DROP_BOULDER_ON_PROTESTER);
                }
                else if (damage == 2) // squirt
                {
                    m_Protesters[i]->damage(2);
                    if (m_Protesters[i]->getHealth() > 0)
                    {
                        playSound(SOUND_PROTESTER_ANNOYED);
                        int restTicks = fmax(50, 100 - getLevel()*10);
                        m_Protesters[i]->setRestTicks(restTicks);
                    }
                }
                else // gold nugget bribe
                    m_Protesters[i]->setBribed(true);
            }
    }
}

// returns true if the boulder will collide with another boulder
bool StudentWorld::bouldersCollide(int x, int y)
{
    // since Boulders are always the first actors added (need better way of doing this)
    for (int i = 0; i < m_numBoulders; i++)
    {
        int aX = m_Actors[i]->getX();
        int aY = m_Actors[i]->getY();
        int dX = x - aX;
        int dY = y - aY;
        if (dY == 1 && abs(dX) <= 3)
            return true;
    }
    return false;
}

// returns true if boulder should fall, false otherwise
bool StudentWorld::shouldBoulderFall(int x, int y, int boulderStatus)
{
    // if we're at the bottom of the oil field, the boulder cannot fall any further
    if (y == 0)
        return false;
    
    // for both stable and falling boulders,
    // check for earth below
    for (int cX = x; cX < x+4; cX++)
        // check for earth below (starting or continuing to fall)
        if (checkForEarth(cX, y-1))
            return false;
    
    
    // if already falling
    // check for boulders or TunnelMan
    if (boulderStatus == BOULDER_FALLING)
    {
        // check for boulders
        if (bouldersCollide(x, y))
            return false;
        // check for players
        if (checkForTunnelMan(x, y) == true)
        {
            m_TunnelMan->damage(100);
//            m_TunnelMan->kill();
            return false;
        }
        // check for protesters
        if (checkForProtesters(x, y) == true)
        {
            hitProtesters(x, y, 100);
        }
    }
    
    // otherwise, return true
    return true;
}


// for digging earth when moving/inserting boulders
void StudentWorld::dig(int dX, int dY)
{
    bool dugEarth = false; // flag variable
    for (int x = dX; x < dX + 4; x++)
        for (int y = dY; y < dY + 4; y++)
            if (checkForEarth(x, y))
            {
                m_Earth[x][y]->kill(); // so we can later check to see how far the boulder can fall
                m_Earth[x][y]->setVisible(false);
//                playSound(SOUND_DIG);
                dugEarth = true;
            }
    
    if (dugEarth == true) // reduces slow-down by playing sound once instead of four times for each earth object
        playSound(SOUND_DIG);
}



// drop gold nugget
void StudentWorld::dropGold(Gold* g)
{
    m_Actors.push_back(g);
    m_Goodies.push_back(g);
}

// add the squirt object to the actors vector
void StudentWorld::shootSquirt(Squirt* s)
{
    bool obstructed = false;
    // check if in bounds
    if (s->getX() > 63 || s->getX() < 0 || s->getY() > 63 || s->getY() < 0)
        obstructed = true;
    else {
        // check if earth or boulders are occupying the space where the squirt should spawn
        if (checkForBoulders(s->getX(), s->getY()) == true)
            obstructed = true;
        for (int x = s->getX(); x < s->getX()+4; x++)
            for (int y = s->getY(); y < s->getY()+4; y++)
                if (checkForEarth(x, y) == true)
                    obstructed = true;
    }
    
    // if the squirt is obstructed, make it invisible
    if (obstructed)
    {
        s->setVisible(false);
        return;
    }
    
    // if no obstructions are present, add the squirt to the vector
    m_Actors.push_back(s);
}

// use sonar kit to search for gold and barrels (within radius 12 from passed coordinates)
void StudentWorld::useSonar(int x, int y)
{
    for (int i = 0; i < m_Goodies.size(); i++)
    {
        int mX = m_Goodies[i]->getX();
        int mY = m_Goodies[i]->getY();
        // if another actor is occupying the space
        if (withinRadius(mX, mY, x, y, 12.0))
            m_Goodies[i]->setVisible(true);
    }
}

// return pointer to TunnelMan
TunnelMan* StudentWorld::getTunnelMan()
{
    return m_TunnelMan;
}

int StudentWorld::getTick() {return m_ticks;}


// checking to see if there's a path between two x or y coordinates
// needs to check 4x4 for earth not 1x1: added 4 to outer loops
bool StudentWorld::checkXPath(int x, int y1, int y2)
{
    // checking to see if path remains in bounds
    if (y1 > 60 || y1 < 0 || y2 > 60 || y2 < 0)
        return false;
    
    // check which is the higher y value
    if (y1 < y2) // move from y1 to y2
    {
        for (int cY = y1; cY < y2+4; cY++)
        {
            // check for boulders along path
            if (checkForBoulders(x, cY))
                return false;
            // check for earth along path
            for (int cX = x; cX < x+4; cX++)
                if (checkForEarth(cX, cY))
                    return false;
        }
    }
    
    else // otherwise move from y2 to y1
    {
        for (int cY = y2; cY < y1+4; cY++)
        {
            // check for boulders along path
            if (checkForBoulders(x, cY))
                return false;
            // check for earth along path
            for (int cX = x; cX < x+4; cX++)
                if (checkForEarth(cX, cY))
                    return false;
        }
    }
    
    // if no obstruction is found
    return true;
}

// return true if path is clear, return false otherwise
bool StudentWorld::checkYPath(int y, int x1, int x2)
{
    // checking to see if path remains in bounds
    if (x1 > 60 || x1 < 0 || x2 > 60 || x2 < 0)
        return false;
    
    // check which is the higher x value
    if (x1 < x2) // move from x1 to x2
    {
        for (int cX = x1; cX < x2+4; cX++)
        {
            // check for boulders along path
            if (checkForBoulders(cX, y))
                return false;
            // check for earth along path
            for (int cY = y; cY < y+4; cY++)
                if (checkForEarth(cX, cY))
                    return false;
        }
    }
    
    else // otherwise move from x2 to x1
    {
        for (int cX = x2; cX < x1+4; cX++)
        {
            // check for boulders along path
            if (checkForBoulders(cX, y))
                return false;
            // check for earth along path
            for (int cY = y; cY < y+4; cY++)
                if (checkForEarth(cX, cY))
                    return false;
        }
    }
    
    // if no obstruction is found
    return true;
}


// for maintaining count of remaining actors
void StudentWorld::decNumBoulders() {m_numBoulders--;}

void StudentWorld::decNumBarrels() {m_numBarrels--;}

void StudentWorld::decNumProtesters() {m_numProtesters--;}

void StudentWorld::addNewActors()
{
    m_numProtesters = m_Protesters.size();
    int maxNumProtesters = fmin(15, 2 + getLevel() * 1.5);
    int minTicksSinceLastProtester = fmax(25, 200 - getLevel());
    int probabilityOfHardcore = fmin(90, getLevel() * 10 + 30);
    
    if (m_numProtesters < maxNumProtesters && m_ticksSinceProtester > minTicksSinceLastProtester)
    {
        int protesterProbability = 1 + rand() % 100;
        Protester* p;
        if (protesterProbability <= probabilityOfHardcore)
            p = new hardcoreProtester(this);
        else
            p = new regularProtester(this);
        m_Actors.push_back(p);
        m_Protesters.push_back(p);
        m_numProtesters++;
        m_ticksSinceProtester = 0;
    }
    else
        m_ticksSinceProtester++;
    
    int probabilityOfGoodie = getLevel() * 25 + 300; // actual value is 1/probabilityOfGoodie
    int spawnProbability = 1 + rand() % probabilityOfGoodie;
    if (spawnProbability == 1) // 1 in probabilityOfGoodie odds
    {
        // Sonar is 0.2 prob; Water is 0.8 prob
        // Sonar (0,60); Water wherever there's no 4x4 of Earth
        Goodie* g = nullptr;
        int goodieProbability = 1 + rand() % 5;
        
        if (goodieProbability == 1) // SONAR (1/5 odds)
            g = new Sonar(this, 0, 60);
        
        else // WATER (4/5 odds)
        {
            while (g == nullptr) // while the water has not yet been added
            {
                // pick out random coordinates
                int rX = rand() % 61; // 0 <= rX <= 60
                int rY = rand() % 61; // 0 <= rY <= 60
                // checking to see if the position is clear
                if (checkXPath(rX, rY, rY) == false) // using a path check function for obstructions
                    continue;
                g = new Water(this, rX, rY);
            }
        }
        m_Actors.push_back(g);
        m_Goodies.push_back(g);
    }
}

void StudentWorld::allActorsDoSomething()
{
    // ask TunnelMan & Actors to do something for each tick
    m_TunnelMan->doSomething();
    for (int i = 0; i < m_Actors.size(); i++)
        m_Actors[i]->doSomething();
}

void StudentWorld::removeDeadActors()
{
    // Actors vector
    for (int i = 0; i < m_Actors.size(); i++)
        if (m_Actors[i]->isAlive() == false)
        {
            delete m_Actors[i];
            // remove from vector
            m_Actors.erase(m_Actors.begin() + i);
            i--;
        }
    // Protesters vector
    for (int i = 0; i < m_Protesters.size(); i++)
        if (m_Protesters[i]->isAlive() == false)
        {
            m_Protesters.erase(m_Protesters.begin() + i);
            i--;
        }
    // Boulder vector
    for (int i = 0; i < m_Boulders.size(); i++)
        if (m_Boulders[i]->isAlive() == false)
        {
            m_Boulders.erase(m_Boulders.begin() + i);
            i--;
        }
    // Goodies vector
    for (int i = 0; i < m_Goodies.size(); i++)
        if (m_Goodies[i]->isAlive() == false)
        {
            m_Goodies.erase(m_Goodies.begin() + i);
            i--;
        }
}

void StudentWorld::setDisplayText()
{
    int level = getLevel();
    int lives = getLives();
    int health = m_TunnelMan->getHealth() * 10;
    int water = m_TunnelMan->getWater();
    int gold = m_TunnelMan->getGold();
    int barrelsLeft = m_numBarrels;
    int sonar = m_TunnelMan->getSonar();
    int score = getScore();
    
    // Formatting
    // Lvl: 52 Lives: 3 Hlth: 80% Wtr: 20 Gld: 3 Oil Left: 2 Sonar: 1 Scr: 321000
    ostringstream displayText;
    displayText << "Lvl: " << setw(2) << level << "  ";
    displayText << "Lives: " << setw(1) << lives << "  ";
    displayText << "Hlth: " << setw(3) << health << "%  ";
    displayText << "Wtr: " << setw(2) << water << "  ";
    displayText << "Gld: " << setw(2) << gold << "  ";
    displayText << "Oil Left: " << setw(2) << barrelsLeft << "  ";
    displayText << "Sonar: " << setw(2) << sonar << "  ";
    displayText.fill('0');
    displayText << "Scr: " << setw(6) << score;
    
    // Converting to string and setting
    string s = displayText.str();
    setGameStatText(s);
}


int StudentWorld::init()
{
    m_ticks = 0;
    
    // initialize TunnelMan and Earth objects in correct locations
    m_TunnelMan = new TunnelMan(this);
    
    for (int x = 0; x < VIEW_WIDTH; x++)
        for (int y = 0; y < 60; y++) // excluding top
            if (!(x >= 30 && x <= 33 && y >= 4 && y <= 59)) // excluding central tunnel
                m_Earth[x][y] = new Earth(this, x, y);
    
    
    // initialize Boulders
    m_numBoulders = fmin(getLevel() / 2 + 2, 9);
    int numPlaced = 0;
    // distribute over x = 0, 60 and y = 20, 56
    while (numPlaced < m_numBoulders)
    {
        int rX = rand() % 61; // 0 <= rX <= 60
        int rY = 20 + rand() % 37; // 20 <= rY <= 56
        // accounting for central tunnel
        if (rX >= 27 && rX <= 33 && rY >= 4 && rY <= 59) // adjust for boulder jutting into tunnel
            continue;
        if (checkForActors(rX, rY) == true)
            continue;
        Boulder* boul = new Boulder(this, rX, rY);
        dig(rX, rY);
        m_Actors.push_back(boul);
        m_Boulders.push_back(boul);
        numPlaced++;
    }
    
    
    // intialize barrels of oil
    m_numBarrels = fmin(2 + getLevel(), 21);
    numPlaced = 0;
    while (numPlaced < m_numBarrels)
    {
        int rX = rand() % 61; // 0 <= rX <= 60
        int rY = rand() % 57; // 0 <= rY <= 56
        // accounting for central tunnel
        if (rX >= 27 && rX <= 33 && rY >= 4 && rY <= 59)
            continue;
        if (checkForActors(rX, rY) == true)
            continue;
        Barrel* bar = new Barrel(this, rX, rY);
        m_Actors.push_back(bar);
        m_Goodies.push_back(bar);
        numPlaced++;
    }
    
    // initialize gold nuggest
    m_numGoldNuggets = fmax(5 - getLevel()/2, 2);
    numPlaced = 0;
    while (numPlaced < m_numGoldNuggets)
    {
        int rX = rand() % 61; // 0 <= rX <= 60
        int rY = rand() % 57; // 0 <= rY <= 56
        // accounting for central tunnel
        if (rX >= 27 && rX <= 33 && rY >= 4 && rY <= 59)
            continue;
        if (checkForActors(rX, rY) == true)
            continue;
        Gold* g = new Gold(this, rX, rY, false);
        m_Actors.push_back(g);
        m_Goodies.push_back(g);
        numPlaced++;
    }
    
    
    // initialize protester members
    m_numProtesters = 0;
    m_ticksSinceProtester = 201;
    
    
    return GWSTATUS_CONTINUE_GAME;
}



int StudentWorld::move()
{
    m_ticks++;
    
    setDisplayText();
    
    addNewActors();
    allActorsDoSomething();
    removeDeadActors();
    
    if (m_TunnelMan->getHealth() <= 0)
        m_TunnelMan->kill();
    
    if (m_TunnelMan->isAlive() == false)
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    if (m_numBarrels == 0)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}




void StudentWorld::cleanUp()
{
    // delete TunnelMan and Earth objects; similar to destructor
    delete m_TunnelMan;
    for (int x = 0; x < VIEW_WIDTH; x++)
        for (int y = 0; y < 60; y++) // excluding top
            if (!(x >= 30 && x <= 33 && y >= 4 && y <= 59)) // excluding central tunnel
                delete m_Earth[x][y];
    
    // go through vectors and delete all items
    for (int i = 0; i < m_Actors.size(); i++)
    {
        delete m_Actors[i];
        m_Actors.erase(m_Actors.begin() + i);
        i--;
    }
    // Protesters vector
    for (int i = 0; i < m_Protesters.size(); i++)
        {
            m_Protesters.erase(m_Protesters.begin() + i);
            i--;
        }
    // Boulder vector
    for (int i = 0; i < m_Boulders.size(); i++)
        {
            m_Boulders.erase(m_Boulders.begin() + i);
            i--;
        }
    // Goodies vector
    for (int i = 0; i < m_Goodies.size(); i++)
        {
            m_Goodies.erase(m_Goodies.begin() + i);
            i--;
        }
}
