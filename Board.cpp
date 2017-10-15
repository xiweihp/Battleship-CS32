#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <vector>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
      // TODO:  Decide what private members you need.  Here's one that's likely
      //        to be useful:
    char points[MAXROWS][MAXCOLS];
    const Game& m_game;
    struct Ship{
        int m_id;
        char m_symbol;
        int m_length;
    };
    
    vector<Ship> ships;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            points[r][c] = '.';
        }
    }

}

void BoardImpl::clear()
{
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            points[r][c] = '.';
        }
    }
}

void BoardImpl::block()
{
      // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (randInt(2) == 0)
            {
                points[r][c] = '#'; // block cell (r,c)
            }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            if(points[r][c] == '#')
                points[r][c] = '.'; //unblock cell (r,c) if blocked
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    
    if(shipId < 0 || shipId >= m_game.nShips())
        return false; //The shipId is invalid
    if(!m_game.isValid(topOrLeft))
        return false;

    char symbol = m_game.shipSymbol(shipId);
    int length = m_game.shipLength(shipId);
    
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (points[r][c] == symbol)
            {
                return false;
            }
        }
        //The ship with that ship ID has previously been placed on this Board and not yet been unplaced since its most recent placement.
    }
    
    if(dir == HORIZONTAL)
    {
        int i = topOrLeft.c;
        while(i < (topOrLeft.c + length))
        {
            if(i >= m_game.cols())//The ship would be partly or fully outside the board.
                return false;
            if(points[topOrLeft.r][i] != '.')//The ship would overlap an already-placed ship./blocked space
                return false;//Or the ship would overlap one or more positions that were blocked by a previous call to the block function.
            i++;
        }
        
        i = topOrLeft.c;
        
        Ship temp;
        temp.m_id = shipId;
        temp.m_symbol = symbol;
        temp.m_length = length;
        ships.push_back(temp);
 
        while(i < (topOrLeft.c + length))
        {
            points[topOrLeft.r][i] = symbol;
            i++;
        }
        
    }
    else if(dir == VERTICAL)
    {
        int i = topOrLeft.r;
        while(i < (topOrLeft.r + length))
        {
            if(i >= m_game.rows())//The ship would be partly or fully outside the board.
                return false;
            if(points[i][topOrLeft.c] != '.')
                return false;
            //The ship would overlap an already-placed ship/blocked space Or the ship would overlap one or more positions that were blocked by a previous call to the block function.
            i++;
        }
        
        i = topOrLeft.r;
        Ship temp;
        temp.m_id = shipId;
        temp.m_symbol = symbol;
        temp.m_length = length;
        ships.push_back(temp);

        
        while(i < (topOrLeft.r + length))
        {
            points[i][topOrLeft.c] = symbol;
            i++;
        }
    }
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if(shipId < 0 || shipId >= m_game.nShips())
        return false; //The shipId is invalid
    if(!m_game.isValid(topOrLeft))
        return false;

    char symbol = m_game.shipSymbol(shipId);
    int length = m_game.shipLength(shipId);
    
    if(dir == HORIZONTAL)
    {
        if((topOrLeft.c + length) > m_game.cols())
            return false;
        for(int i = topOrLeft.c;i< topOrLeft.c + length;i++)
        {
            if(points[topOrLeft.r][i] != symbol)
            {
                return false;
            }
        }
        for(int j = 0; j<ships.size();j++)
        {
            if(ships[j].m_id == shipId)
            {
                ships[j].m_symbol = '.';
                ships[j].m_length = -1;
                ships[j].m_id = -1;
            }
        }
        for(int i = topOrLeft.c;i< topOrLeft.c + length;i++)
        {
            points[topOrLeft.r][i] = '.';
        }
    }
    
    if(dir == VERTICAL)
    {
        if((topOrLeft.r + length) > m_game.rows())
            return false;
        for(int i = topOrLeft.r;i< topOrLeft.r + length;i++)
        {
            if(points[i][topOrLeft.c] != symbol)
            {
                return false;
            }
        }
        for(int j = 0; j<ships.size();j++)
        {
            if(ships[j].m_id == shipId)
            {
                ships[j].m_symbol = '.';
                ships[j].m_length = -1;
                ships[j].m_id = -1;
            }
        }
        for(int i = topOrLeft.r;i< topOrLeft.r + length;i++)
        {
            points[i][topOrLeft.c] = '.';
        }
    }
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
    cout << "  ";
    for(int i = 0; i < m_game.cols();i++)
        cout << i;
    cout << endl; //first line
    for(int i = 0; i < m_game.rows(); i++)
    {
        cout << i << " ";
        for(int j = 0 ; j< m_game.cols(); j++)
        {
            if(shotsOnly == false)
                cout << points[i][j];
            else
            {
                if(points[i][j] != '.'&&points[i][j] != 'X'&&points[i][j] != 'o')
                    cout<<'.';
                else
                    cout << points[i][j];
            }
        }
        cout << endl;
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    if(!m_game.isValid(p))
        return false;//out of boundary
    char origin = points[p.r][p.c];
    if(origin == 'o' || origin == 'X')
        return false;
    
    if(origin != '.')
    {
        int shotPos = -1;
        for(int i = 0; i < ships.size(); i++)
        {
            if(ships[i].m_symbol == origin)
            {
                ships[i].m_length--;
                shotPos = i;
            }
        }
        points[p.r][p.c] = 'X';
        shotHit = true;
        if(ships[shotPos].m_length == 0)
        {
            shipDestroyed = true;
            shipId = ships[shotPos].m_id;
        }
        
    }
    else
    {
        points[p.r][p.c] = 'o';
        shotHit = false;
        shipDestroyed = false;
    }
    
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
    for(int i = 0; i < ships.size();i++)
    {
        if(ships[i].m_length > 0)
            return false;
    }
    
    return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
