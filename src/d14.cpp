#include <iostream>
#include <numeric>

#include "util.h"

using namespace std;

typedef int64_t BigInt;

typedef Pos Velocity;
typedef Pos Bounds;

// bounds for example
// const Bounds bounds = {11,7};

// bounds for input
const Bounds bounds = {101,103};

struct Bot
{
    Pos p;
    Velocity v;

    Bot Advance(BigInt seconds) const
    {
        Bot result;
        result.v=v;
        result.p.col = (p.col + v.col * seconds);// % bounds.col;
        result.p.row = (p.row + v.row * seconds);// % bounds.row;

        while (result.p.col < 0) result.p.col += bounds.col;
        while (result.p.row < 0) result.p.row += bounds.row;
        if (result.p.col > bounds.col) result.p.col %= bounds.col;
        if (result.p.row > bounds.row) result.p.row %= bounds.row;
                
        return result;
    }
};

int CountBotsInBounds(const vector<Bot>& bots, int c0, int r0, int c1, int r1)
{
   int result=0;
   for(auto bot : bots)
   {
         if (bot.p.col >= c0 && bot.p.col < c1 && 
             bot.p.row >= r0 && bot.p.row < r1)
         {
              result++;
         }
   }
   return result;
}

void DebugPrint(const vector<Bot>& bots, 
    int c0=0, int r0=0, int c1=bounds.col, int r1=bounds.row)
{
    for (int row=r0;row!=r1;row++)
    {
        for (int col=c0;col!=c1;col++)
        {
            int i = CountBotsInBounds(bots, col, row, col+1, row+1);
            if (i>0 && i <=9) 
                cout << i;
            else if (i>9)
                cout << "X";
            else
                cout << ".";
        }
        cout << endl;
    }
}


vector<Bot> Advance(const vector<Bot>& bots, BigInt seconds)
{
    vector<Bot> result;
    result.reserve(bots.size());
    for(auto bot : bots)
    {
        result.push_back(bot.Advance(seconds));
    }
    return result;
}

void Fourteen()
{
    vector<Bot> bots;

    string line;
    while (getline(cin, line))
    {
        // parse positions (p) and velocities (v)
        // example: "p=0,4 v=3,-3"
        int px, py, vx, vy;
        sscanf(line.c_str(), "p=%d,%d v=%d,%d", &px, &py, &vx, &vy);
        Bot b;
        b.p.col = px;
        b.p.row = py;
        b.v.col = vx;
        b.v.row = vy;
        bots.push_back(b);
        // cout << "p=" << px << "," << py << " v=" << vx << "," << vy << endl;
    }

    DebugPrint(bots);
    cout << CountBotsInBounds(bots, 0, 0, bounds.col, bounds.row) << " / " << bots.size() << endl << endl;

    auto p1 = Advance(bots, 100);
    DebugPrint(p1);
    cout << CountBotsInBounds(bots, 0, 0, bounds.col, bounds.row) << " / " << bots.size() << endl << endl;

    int bhc = bounds.col/2;
    int bhr = bounds.row/2;

    int q1 = CountBotsInBounds(p1,   0,   0, bhc,        bhr);
                    DebugPrint(p1,   0,   0, bhc,        bhr);
    cout << "Q1: " << q1 << endl;

    int q2 = CountBotsInBounds(p1, bhc+1,   0, bounds.col, bhr);
                    DebugPrint(p1, bhc+1,   0, bounds.col, bhr);
    cout << "Q2: " << q2 << endl;

    // Robots that are exactly in the middle (horizontally or vertically) don't count as being in any quadrant


    int q3 = CountBotsInBounds(p1,   0, bhr+1, bhc,        bounds.row);
                    DebugPrint(p1,   0, bhr+1, bhc,        bounds.row);
    cout << "Q3: " << q3 << endl;

    int q4 = CountBotsInBounds(p1, bhc+1, bhr+1, bounds.col, bounds.row);
                    DebugPrint(p1, bhc+1, bhr+1, bounds.col, bounds.row);
    cout << "Q4: " << q4 << endl;

    cout << "P1: " << q1 << " " << q2 << " " << q3 << " " << q4 << " = " << q1*q2*q3*q4 << endl;

}