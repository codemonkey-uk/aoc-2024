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

int FindLoops(
    const vector< string >& og,
    const vector< string >& part1result, int steps, int candidates)
{
    int result=0;
    int checked=0;
    vector< string > resultg = og;

    for (size_t row=0;row!=og.size();row++)
    {
        for (size_t col=0;col!=og[row].size();++col)
        {
            char c1 = og[row][col];
            char c2 = part1result[row][col];

            // if its on the path, and not the original guard start location
            if (c2=='X' && !IsGuard(c1))
            {
                vector< string > test = og;
                test[row][col] = '#';

                int newSteps = 0;
                while (Step(test))
                {
                    //for (const auto& line : grid)
                    //    cout << line << endl;
                    //cout << endl;
                    newSteps++;
                    if (newSteps > 2*steps)
                    {
                        resultg[row][col] = 'O';
                        result++;
                        break;
                    }
                }
                checked++;
                cout << checked << "/" << candidates << " checked." << endl;
            }
        }
    }

    for (const auto& line : resultg)
        cout << line << endl;
    cout << endl;

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

    vector< string > og = grid;

    //for (const auto& line : grid)
    //    cout << line << endl;
    //cout << endl;

    int steps = 0;
    while (Step(grid))
    {
        //for (const auto& line : grid)
        //    cout << line << endl;
        //cout << endl;
        steps++;
    }

    cout << "Path ends after " << steps << " steps." << endl;
    //for (const auto& line : grid)
    //    cout << line << endl;
    //cout << endl;

    const int p1 = Count(grid);
    const int candidates = FindLoops(og, grid, steps, p1);

    cout << "P1: " << p1 << endl;
    cout << "P2: " << candidates << endl;
}

