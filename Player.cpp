#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class HumanPlayer : public Player
{
  public:
    HumanPlayer(std::string nm, const Game& g);
    virtual bool isHuman() const { return true; }
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    char getHorV(int length);

};

HumanPlayer::HumanPlayer(std::string nm, const Game& g):Player(nm,g)
{}

char HumanPlayer::getHorV(int shipId)
{
    cout << "Enter h or v for direction of "<< game().shipName(shipId) <<" (length "<< game().shipLength(shipId)<< "): ";
    string dir = "init";
    getline(cin,dir);
    if(dir[0] == 'h' || dir[0] == 'v')
        return dir[0];
    else
    {
        cout << "Direction must be h or v." << endl;
        return getHorV(shipId);
    }
}

bool HumanPlayer::placeShips(Board& b)
{
    cout<< name() << " must place 5 ships." << endl;
    b.display(false);
    
    for(int i = 0; i < game().nShips();i++)
    {
        char dir = getHorV(i);
        Direction direction = HORIZONTAL;
        switch (dir) {
            case 'h':
                direction = HORIZONTAL;
                break;
            case 'v':
                direction = VERTICAL;
                break;
        }
        int r = -1, c = -1;
        cout << "Enter row and column of leftmost cell (e.g. 3 5):";
        bool getInt = getLineWithTwoIntegers(r, c);
        Point topOrleft;
        topOrleft.r = r;
        topOrleft.c = c;
        bool bePlaced = b.placeShip(topOrleft, i, direction);
        while(!getInt||!bePlaced)
        {
            if(!getInt)
                cout << "You must enter two integers." << endl;
            else
                cout << "The ship can not be placed there." <<endl;
            cout << "Enter row and column of leftmost cell (e.g. 3 5):";
            getInt = getLineWithTwoIntegers(r, c);
            topOrleft.r = r;
            topOrleft.c = c;
            bePlaced = b.placeShip(topOrleft, i, direction);
        }
        if(i != game().nShips()-1)
            b.display(false);

    }
    return true;
}

Point HumanPlayer::recommendAttack()
{
    Point target;
    int row = -1, col = -1;
    cout << "Enter the row and column to attack (e.g, 3 5):";
    bool getInt = getLineWithTwoIntegers(row, col);
    while(getInt == false)
    {
        cout << "You must enter two integers." << endl;
        cout << "Enter the row and column to attack (e.g, 3 5):";
        getInt = getLineWithTwoIntegers(row, col);
    }
    target.r = row;
    target.c = col;
    
    return target;
}

void HumanPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
    // for HumanPlayer, the function needs not do anything.
}

void HumanPlayer::recordAttackByOpponent(Point /* p */)
{
    // for HumanPlayer, the function needs not do anything.
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player
{
public:
    MediocrePlayer(std::string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    Point m_transition;
    bool legalMove(int& shipId, Board& b);
    int m_state;
    vector<Point> Points;
    bool NotYetAttacked(Point p);
    vector<Point> theSet;

};
MediocrePlayer::MediocrePlayer(std::string nm, const Game& g):Player(nm, g),m_transition(0,0),m_state(1)
{
    
}

bool MediocrePlayer::legalMove(int& shipId , Board& b)
{
    Point leftOrtop(-1,-1);
    if(shipId == game().nShips())
    {
        return true;
    }
    for(int i = 0; i < game().rows(); i++)
    {
        for(int j = 0; j < game().cols(); j++)
        {
            leftOrtop.r = i;
            leftOrtop.c = j;
            Direction dir = HORIZONTAL;
            bool couldBePlaced = b.placeShip(leftOrtop, shipId, dir);
            if(couldBePlaced)
            {
                int update = shipId + 1;
                bool result = legalMove(update, b);
                if(result)
                    return true;
                else
                    b.unplaceShip(leftOrtop, shipId, dir);
            }
            
            dir = VERTICAL;
            couldBePlaced = b.placeShip(leftOrtop, shipId, dir);
            if(couldBePlaced)
            {
                int update = shipId + 1;
                bool result = legalMove(update, b);
                if(result)
                    return true;
                else
                    b.unplaceShip(leftOrtop, shipId, dir);
            }
        }
    }
    return false;
}

bool MediocrePlayer::placeShips(Board& b)
{
    int count = 0;
    while(count <= 50)
    {
        b.block();
        int idBegin = 0;
        bool result = legalMove(idBegin, b);
        if(result)
        {
            b.unblock();
            return true;
        }
        b.unblock();
        count++;
    }
    return false;
}

bool MediocrePlayer::NotYetAttacked(Point p)
{
    if(Points.empty())
        return true;
    vector<Point>::iterator pos = Points.begin();
    while(pos != Points.end())
    {
        if(((*pos).r == p.r) &&((*pos).c == p.c))
            return false;
        else
            pos++;
    }
    return true;
}

Point MediocrePlayer::recommendAttack()
{
    if (m_state == 2)
    {
        int pos = randInt(theSet.size());
        Point p = theSet[pos];
        vector<Point>::iterator it = theSet.begin();
        Points.push_back(p);
        for(int i = 0; i <= pos; i++)
        {
            if(i == pos)
                theSet.erase(it);
            else
                it++;
        }
        if(theSet.empty())
            m_state = 1;
        return p;
    }
    else
    {
        Point p = game().randomPoint();
        while(!NotYetAttacked(p))
        {
            p = game().randomPoint();
        }
        Points.push_back(p);
        return p;
    }
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                        bool shipDestroyed, int shipId)
{
    if ((m_state == 1) &&(shotHit) && (!shipDestroyed))
    {
        m_transition = p;
        Point temp;
        for(int i = p.r - 4; i <= p.r + 4 ; i ++)
        {
            temp.r = i;
            temp.c = p.c;
            if(game().isValid(temp))
                theSet.push_back(temp);
        }
        for(int j = p.c - 4; j <= p.c + 4; j++)
        {
            temp.r = p.r;
            temp.c = j;
            if(game().isValid(temp))
                theSet.push_back(temp);
        }
        vector<Point>::iterator it = theSet.begin();
        while(it!=theSet.end())
        {
            if(!NotYetAttacked(*it))
                theSet.erase(it);
            else
                it++;
        }
        m_state = 2;
    }
    if((m_state == 2) && (shotHit) && (shipDestroyed))
    {
        theSet.clear();
        m_state = 1;
    }
}

void MediocrePlayer::recordAttackByOpponent(Point /* p */)
{
    // for MediocrePlayer, the function needs not do anything.
}


// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class GoodPlayer : public Player
{
public:
    GoodPlayer(std::string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                    bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    Point lastCellAttacked;
    int m_state;
    bool NotYetAttacked(Point p);
    vector<Point> Points;
    vector<Point> theHSet;
    vector<Point> theSet;
    vector<Point> defaultSet;
    int m_default;
    Point m_transition;
    void setTheSet(Point p);
    bool stdPlaceShips(Board& b);
};

GoodPlayer::GoodPlayer( std::string nm, const Game& g ): Player(nm,g),lastCellAttacked(0,0),m_transition(0,0), m_default(0)
{
    m_state = 1;
    
    for(int j = 0; j<= game().rows()/2 + 1;j++)
    {
        if(j%2 == 0)
        {
            for(int i = 0; i < game().cols();i++)
            {
                if(i%4 == 0)
                {
                    Point temp(j, i);
                    if(game().isValid(temp))
                        defaultSet.push_back(temp);
                }
            }
            for(int i = 0; i < game().cols();i++)
            {
                if(i%2 == 0 && i%4 != 0)
                {
                    Point temp(j, i);
                    if(game().isValid(temp))
                        defaultSet.push_back(temp);
                }
            }
        }
        if(j%2 == 1)
        {
            for(int i = 0; i < game().cols();i++)
            {
                if(i%4 == 1)
                {
                    Point temp(j  , i);
                    if(game().isValid(temp))
                        defaultSet.push_back(temp);
                    
                }
                if(i%4 == 3)
                {
                    Point temp(j, i);
                    if(game().isValid(temp))
                        defaultSet.push_back(temp);
                }
                
            }
        }
        
    }
    for (int j = game().rows()/4 + 1 ; j< game().rows() ;j++)
    {
        
        if(j%2 == 0)
        {
            for(int i = 0; i < game().cols();i++)
            {
                if(i%4 == 0)
                {
                    Point temp(j, i);
                    if(game().isValid(temp))
                        defaultSet.push_back(temp);
                }
            }
            for(int i = 0; i < game().cols();i++)
            {
                if(i%2 == 0 && i%4 != 0)
                {
                    Point temp(j, i);
                    if(game().isValid(temp))
                        defaultSet.push_back(temp);
                }
            }
        }
        if(j%2 == 1)
        {
            for(int i = 0; i < game().cols();i++)
            {
                if(i%4 == 1)
                {
                    Point temp(j  , i);
                    if(game().isValid(temp))
                        defaultSet.push_back(temp);
                    
                }
                if(i%4 == 3)
                {
                    Point temp(j, i);
                    if(game().isValid(temp))
                        defaultSet.push_back(temp);
                }
                
            }
        }
        
    }
    

}

bool GoodPlayer::stdPlaceShips(Board& b)
{
    bool result = true;
    for(int i = 0; i < game().nShips() && result;i++)
    {
        Point LeftOrTop;
        Direction dir;
        if(i == 0)
        {
            LeftOrTop = Point(0,6);
            dir = VERTICAL;
        }
        else if(i == 1)
        {
            LeftOrTop = Point(6,6);
            dir = HORIZONTAL;
        }
        else if(i == 2)
        {
            LeftOrTop = Point(2,1);
            dir = VERTICAL;
        }
        else if(i == 3)
        {
            LeftOrTop = Point(5,2);
            dir = VERTICAL;
        }
        else if(i == 4)
        {
            LeftOrTop = Point(8,4);
            dir = VERTICAL;
        }
        else
            return false;
        
        result = b.placeShip(LeftOrTop, i, dir);
        
    }
    return result;
}

bool GoodPlayer::placeShips(Board& b)
{

    bool result = stdPlaceShips(b);
    if (result)
        return true;
    b.clear();
    for(int i = 0; i < game().nShips();i++)
    {
        Point leftOrtop = game().randomPoint();
        bool bePlaced = false;
        while(!bePlaced)
        {
            int dir = randInt(2);
            leftOrtop = game().randomPoint();
            if(dir == 1)
            {
                Direction dire = HORIZONTAL;
                bePlaced = b.placeShip(leftOrtop, i, dire);
            }
            else if(dir == 0)
            {
                Direction dire = VERTICAL;
                bePlaced = b.placeShip(leftOrtop, i, dire);
            }
        }
        
    }
    return true;
}

bool GoodPlayer::NotYetAttacked(Point p)
{
    if(Points.empty())
        return true;
    vector<Point>::iterator pos = Points.begin();
    while(pos != Points.end())
    {
        if(((*pos).r == p.r) &&((*pos).c == p.c))
            return false;
        else
            pos++;
    }
    return true;
}

Point GoodPlayer::recommendAttack()
{
    Point defaultP;
    if(m_state == 2)
    {
        if(theHSet.empty())
            m_state = 1;
        else
        {
            int pos = randInt(theHSet.size());
            Point p = theHSet[pos];
            Points.push_back(p);
            theHSet.erase(theHSet.begin() + pos);
            return p;
        }
        
    }
    if(m_state == 3)
    {
        if(theSet.empty())
            m_state = 1;
        else
        {
            Point p = theSet[0];
            theSet.erase(theSet.begin());
            Points.push_back(p);
            return p;
        }
    }
    if(m_state == 1)
    {
        while(m_default < defaultSet.size())
        {
            if(!NotYetAttacked(defaultSet[m_default]) )
            {
                m_default++;
            }
            else
            {
                Points.push_back(defaultSet[m_default]);
                return defaultSet[m_default];
            }
        }
        defaultP = game().randomPoint();
        while(!NotYetAttacked(defaultP))
        {
            defaultP = game().randomPoint();
        }
        Points.push_back(defaultP);
        return defaultP;
        
    }
    return defaultP;
}

void GoodPlayer::setTheSet(Point p)
{
    if(p.r == m_transition.r)//hori
    {
        for(int i = 1; i <= 4; i ++)
        {
            Point temp;
            temp.c = m_transition.c - i;
            temp.r = m_transition.r;
            if(game().isValid(temp))
                theSet.push_back(temp);
            temp.c = m_transition.c + i;
            temp.r = m_transition.r;
            if(game().isValid(temp))
                theSet.push_back(temp);
        }
    }
    else if(p.c == m_transition.c)//verti
    {
        for(int i = 1; i <= 4; i ++)
        {
            Point temp;
            temp.c = m_transition.c;
            temp.r = m_transition.r - i;
            if(game().isValid(temp))
                theSet.push_back(temp);
            temp.c = m_transition.c;
            temp.r = m_transition.r + i;
            if(game().isValid(temp))
                theSet.push_back(temp);
        }
    }
    vector<Point>::iterator it1 = theSet.begin();
    while(it1!=theSet.end())
    {
        if(!NotYetAttacked(*it1))
            theSet.erase(it1);
        else
            it1++;
    }

}
void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
                                bool shipDestroyed, int shipId)
{
    if((m_state == 1)&&(shotHit) && (!shipDestroyed))
    {
        m_transition = p;
        Point temp;
        temp.r = p.r-1;
        temp.c = p.c;
        if(game().isValid(temp))
            theHSet.push_back(temp);
        temp.r = p.r + 1;
        temp.c = p.c;
        if(game().isValid(temp))
            theHSet.push_back(temp);
        temp.r = p.r;
        temp.c = p.c - 1;
        if(game().isValid(temp))
            theHSet.push_back(temp);
        temp.r = p.r;
        temp.c = p.c + 1;
        if(game().isValid(temp))
            theHSet.push_back(temp);
        vector<Point>::iterator it1 = theHSet.begin();
        while(it1!=theHSet.end())
        {
            if(!NotYetAttacked(*it1))
                theHSet.erase(it1);
            else
                it1++;
        }
        m_state = 2;
    }
    else if((m_state == 2) && (shotHit) && (shipDestroyed))
    {
        theHSet.clear();
        m_state = 1;
    }
    else if((m_state == 2) && (shotHit) && (!shipDestroyed))
    {
        lastCellAttacked = p;
        theHSet.clear();
        setTheSet(lastCellAttacked);
        m_state = 3;
    }
    else if((m_state == 3) && (shotHit) && (shipDestroyed))
    {
        theSet.clear();
        m_state = 1;
    }

 
}


void GoodPlayer::recordAttackByOpponent(Point /* p */)
{
    // for MediocrePlayer, the function needs not do anything.
}

/*
void GoodPlayer::recordAttackByOpponent(Point p)
{
    
}*/


//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}
