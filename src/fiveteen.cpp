#include <iostream>
#include <deque>
#include "util.h"

#define ASTAR_STATS

#include "astar.h"

using namespace std;

void Print(Grid< int >& grid)
{
    for (int y=0;y!=grid.height;++y)
    {
        for (int x=0;x!=grid.width;++x)
        {
            cout << grid.get(x,y) % 10;
        }
        cout << endl;
    }
    cout << endl;
}

struct Vec { int x; int y; };
Vec adjacent[4] = {
    {-1,0}, {0,-1},
    { 1,0}, {0 ,1}
};

struct astar_node
{
    const Grid< int >* mGrid;
    int mX; int mY;
    
    //default, & copy constructor, &
    astar_node():mGrid(nullptr),mX(0),mY(0){}
    //assignment operator should be available
    astar_node(
        const Grid< int >* pGrid,
        int x, int y
    )
    : mGrid(pGrid),mX(x),mY(y)
    {
    }

    size_t hash() const {
        if (mGrid) return 1 + mX * mGrid->width + mY;
        else return 0;
    };
    
    //example_node::iterator
    //for fetching connected nodes, and costs

    struct iterator{

        const astar_node* mNode;
        int mI=0;
        
        iterator(const astar_node* current) : mNode(current) {}
        iterator(const astar_node* current, int i ) : mNode(current), mI(i) {}
        iterator() : mNode(nullptr) {} 
        
        //copy constructor and assignment operator should be available

        typedef int cost_type;	//typedef required, must be scalar type

        int X() const {return mNode->mX + adjacent[mI].x;}
        int Y() const {return mNode->mY + adjacent[mI].y;}
        astar_node value()const{
            return astar_node(
                mNode->mGrid,
                X(),
                Y()
            );
        }
        
        cost_type cost()const{
            if (X()>=0 && X()<mNode->mGrid->width &&
                Y()>=0 && Y()<mNode->mGrid->height)
                return mNode->mGrid->get(X(),Y());
            else 
                return 999999;
        }
    
        iterator& operator++(){mI++;return*this;}
        bool operator!=(iterator v){
            return (mI!=v.mI || *mNode!=*v.mNode);
        };//used by search

    };

    //Get first, and past-end iterators
    iterator begin()const{return iterator(this);}
    iterator end()const{return iterator(this,4);}

    //equality operator, required
    //note: fuzzy equality may be useful
    bool operator==(const astar_node b)const{
        return mX==b.mX && mY==b.mY;
    }
    bool operator!=(const astar_node b)const{
        return !(*this==b);
    }

};

struct astar_behaviour
{
    double heuristic( astar_node a, astar_node b )
    {
        return astar::manhattan_distance(a.mX, a.mY, b.mX, b.mY);
    }
    bool filter( astar_node node )
    {
        if (node.mX<0 || node.mX>=node.mGrid->width) return true;
        if (node.mY<0 || node.mY>=node.mGrid->height) return true;
        else return false;
    }
};

void PrintMasked(Grid< int >& grid, deque< astar_node >& mask)
{
    for (int y=0;y!=grid.height;++y)
    {
        for (int x=0;x!=grid.width;++x)
        {
            if (find_if( 
                mask.begin(), mask.end(), 
                [x,y](astar_node node){return node.mX==x && node.mY==y;})
                ==mask.end())
                cout << ' ';
            else
                cout << grid.get(x,y) % 10;
        }
        cout << endl;
    }
    cout << endl;
}

void Fiveteen()
{
    // load 1d decimal char into a grid of int
    string line;
    getline(cin,line);
    const int w = line.size();
    vector<string> lines;
    do
    {
        assert(line.size()==(size_t)w);
        lines.push_back(line);
    } while (getline(cin,line));
    const int h = lines.size();
    Grid< int > grid( w, h );
    for (int x=0;x!=w;++x)
    {
        for (int y=0;y!=h;++y)
        {
            int i = lines[y][x]-'0';
            assert(i >= 0 && i < 10);
            grid.get(x,y) = i;
        }
    }
    if (debug) Print(grid);

    // PART 1
    {
        astar_node start( &grid, 0,0 );
        astar_node end( &grid, grid.width-1, grid.height-1 );
        astar_behaviour behaviour;
        deque< astar_node > results;
        astar::config<astar_node> cfg;
    
        if (astar::astar(start, end, results, behaviour, cfg))
        {
            cout << "Success" << endl;
            if (debug) PrintMasked(grid, results);
        
            cout << "Total risk: " << cfg.route_cost << endl;
        }
    }
    
    // PART 2
    Grid< int > grid2( w*5, h*5 );
    for (int x=0;x!=w*5;++x)
    {
        for (int y=0;y!=h*5;++y)
        {
        
            int i = 0;
            if (y<h && x<w)
            {
                i = lines[y][x]-'0';
            }
            else
            {
                int nx=x,ny=y;
                // prefer left, but if hard left try above
                if (nx >= w) nx-=w;
                else ny-=h;
                
                i = grid2.get(nx,ny) + 1;
                if (i>9) i = 1;
            }
            
            assert(i >= 0 && i < 10);
            grid2.get(x,y) = i;
        }
    }    
    if (debug) Print(grid2);
    
    {
        astar_node start( &grid2, 0,0 );
        astar_node end( &grid2, grid2.width-1, grid2.height-1 );
        astar_behaviour behaviour;
        deque< astar_node > results;
        astar::config<astar_node> cfg;
    
        if (astar::astar(start, end, results, behaviour, cfg))
        {
            cout << "Success" << endl;
            if (debug) PrintMasked(grid2, results);
        
            cout << "Total risk: " << cfg.route_cost << endl;
        }
    }
    

}