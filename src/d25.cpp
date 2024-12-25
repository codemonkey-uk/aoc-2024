#include <iostream>
#include <queue>
#include <set>
#include <map>
#include "util.h"

using namespace std;

// typedef int64_t BigInt;
struct Pins
{
    int lengths[5] ={0,0,0,0,0};
};

void DebugPrint(Pins p)
{
    for (int i=0;i!=5;i++)
    {
        cout << p.lengths[i] << " ";
    }
    cout << endl;
}

vector<Pins> Locks;
vector<Pins> Keys;

void Process(const vector<string>& lines)
{
    Pins p;

    // if top row is filled (#), then the lines are a lock
    if (lines[0][0] == '#')
    {
        for (int col=0;col!=lines[0].size();col++)
        {
            for (int row=1;row!=lines.size();row++)
            {
                if (lines[row][col] == '#')
                {
                    p.lengths[col]++;
                }
                else
                {
                    break;
                }
            }
        }
        Locks.push_back(p);
    }
    else
    {
        for (int col=0;col!=lines[0].size();col++)
        {
            for (int row=lines.size()-2;row>=0;row--)
            {
                if (lines[row][col] == '#')
                {
                    p.lengths[col]++;
                }
                else
                {
                    break;
                }
            }
        }
        Keys.push_back(p);
    }
}

bool Match(Pins lock, Pins key)
{
    int result=0;
    for (int i=0;i!=5;i++)
    {
        if (lock.lengths[i] + key.lengths[i] > 5)
        {
            // overlap, no fit
            return false;
        }
    }

    // no overlap, potential fit
    return true;
}

int CountMatches()
{
    int result=0;
    for (int l=0;l!=Locks.size();++l)
    {
        for (int k=0;k!=Keys.size();++k)
        {
            if (Match(Locks[l], Keys[k]))
            {
                result++;
            }
        }
    }
    return result;
}

void TwentyFiveChristmasDay()
{
    string line;
    vector<string> lines;
    while(getline(cin, line))
    {
        if (line.empty())
        {
            Process(lines);
            lines.clear();
        }
        else
        {
            lines.push_back(line);
        }
    }
    if (!lines.empty()) Process(lines);

    cout << "Locks: " << Locks.size() << " Keys: " << Keys.size() << endl;
    //DebugPrint(Locks[0]);
    //DebugPrint(Keys[0]);
    cout << "P1: " << CountMatches() << endl;
}