#include <iostream>

#include "util.h"

using namespace std;

bool IsGuard(char c)
{
    return c=='^' || c=='>' || c=='<' || c=='v';
}

struct Direction
{
    int dc;
    int dr; 
};

Direction GuardDirection(char c)
{
    switch(c)
    {
        case '^': return { 0,-1 };
        case '>': return { 1, 0 };
        case '<': return {-1, 0 };
        case 'v': return { 0, 1 };
    }
    
    cerr << "invalid guard char '" << c << "'" << endl;
    return {0,0};
}

char TurnRight(char c)
{
    switch(c)
    {
        case '^': return '>';
        case '>': return 'v';
        case '<': return '^';
        case 'v': return '<';
    }
    
    cerr << "invalid guard char '" << c << "'" << endl;
    return c;
}


bool Step(vector< string >& grid)
{
    for (size_t row=0;row!=grid.size();row++)
    {
        for (size_t col=0;col!=grid[row].size();++col)
        {
            char c = grid[row][col];

            if (IsGuard(c))
            {
                auto step = GuardDirection(c);
                int r2 = row + step.dr;
                int c2 = col + step.dc;

                grid[row][col] = 'X';

                // complete by going out of bounds
                if (r2<0 || r2>=grid.size()) return false;
                if (c2<0 || c2>=grid[row].size()) return false;

                // blocked
                if (grid[r2][c2]=='#')
                {
                    grid[row][col] = TurnRight (c);
                    return true;
                }
                
                grid[r2][c2]=c;
            }
        }
    }

    return true;
}

int Count(const vector< string >& grid)
{
    int result=0;
    for (size_t row=0;row!=grid.size();row++)
    {
        for (size_t col=0;col!=grid[row].size();++col)
        {
            char c = grid[row][col];
            if (c=='X') result++;
        }
    }
    return result;
}

void Six()
{   
    vector< string > grid;
    string line;

    while (std::getline(std::cin, line))
    {
        grid.push_back(line);  
    }

    for (const auto& line : grid)
        cout << line << endl;
    cout << endl;

    while (Step(grid))
    {
        for (const auto& line : grid)
            cout << line << endl;
        cout << endl;
    }

    for (const auto& line : grid)
        cout << line << endl;
    cout << endl;

    cout << "P1: " << Count(grid) << endl;
    //cout << "P2: " << fixedTotal << endl;
}

