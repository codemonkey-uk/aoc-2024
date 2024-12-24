#include <iostream>
#include <numeric>

#include "util.h"

using namespace std;
typedef int64_t BigInt;
typedef Pos Bot;
typedef Pos Dir;

Pos FindBot(const vector<string>& map)
{
    for (int row=0;row!=map.size();row++)
    {
        for (int col=0;col!=map[row].size();col++)
        {
            if (map[row][col] == '@')
            {
                return {col,row};
            }
        }
    }
    return {-1,-1};
}

constexpr Dir Direction(char c)
{
    switch(c)
    {
        case '>': return {1,0};
        case '<': return {-1,0};
        case '^': return {0,-1};
        case 'v': return {0,1};
    }
    return {0,0};
}

BigInt GPS(Pos p)
{
    return p.col + p.row*100;
}

BigInt AllGPS(const vector<string>& map)
{
    BigInt total=0;
    for (int row=0;row!=map.size();row++)
    {
        for (int col=0;col!=map[row].size();col++)
        {
            auto c = map[row][col];
            if (c == 'O' || c=='[') total += GPS({col,row});
        }
    }
    return total;
}

Bot Move(vector<string>& map, const Bot& bot, char m, bool commitMove=true)
{
    Dir d = Direction(m);
    const Pos next = bot+d;

    // dont go off edge
    if (bot.row < 0 || bot.row >= map.size() || bot.col < 0 || bot.col >= map[0].size())
    {
        return bot;
    }

    const char from = map[bot.row][bot.col];
    const char too = map[next.row][next.col];    

    // blocked by a wall
    if (too == '#')
    {
        return bot;
    }

    if (too == '.')
    {
        if (commitMove)
        {
            map[bot.row][bot.col] = '.';
            map[next.row][next.col] = from;
        }
        return next;
    }

    if (too == '[' || too == ']')
    {
        // moves left or right work same as with O
        if (m=='>' || m=='<')
        {
            // move "next" (a wall) out the way recusivly
            if (Move(map, next, m)!=next)
            {
                if (commitMove)
                {
                    map[bot.row][bot.col] = '.';
                    map[next.row][next.col] = from;
                }
                return next;
            }
            else
            {
                return bot;
            }
        }
        // moves up and down move connected blocks only if both halves can move
        else if (m=='^' || m=='v')
        {
            Dir conex = (too == '[') ? Dir{1,0} : Dir{-1,0};
            Pos connected = next+conex;

            // need to check the connecting box can move
            bool canMoveBoth = 
                // can move self
                Move(map, next, m, false)!=next &&
                Move(map, connected, m, false)!=connected;

            if (canMoveBoth)
            {
                Move(map, connected, m);
                Move(map, next, m);
                
                if (commitMove)
                {
                    map[bot.row][bot.col] = '.';
                    map[next.row][next.col] = from;
                }
                return next;
            }

            return bot;
        }
    }

    if (too == 'O')
    {
        // moved "next" (a wall) out the way recusivly
        if (Move(map, next, m)!=next)
        {
            if (commitMove)
            {
                map[bot.row][bot.col] = '.';
                map[next.row][next.col] = from;
            }
            return next;
        }
    }

    // bot did not move
    return bot;
}

static void DebugPrint(const vector<string>& map)
{
    for (auto row : map)
    {
        cout << row << endl;
    }
}

vector<string> Widen(const vector<string>& map)
{
    vector<string> result;
    result.reserve(map.size());

    for (auto row : map)
    {
        string newrow;
        newrow.reserve(row.size()*2);

        for (auto c : row)
        {
            switch (c)
            {
                case '#': newrow += "##"; break;
                case 'O': newrow += "[]"; break;
                case '.': newrow += ".."; break;
                case '@': newrow += "@."; break;                
            }
        }

        result.push_back(newrow);
    }

    return result;
}

void Fiveteen()
{
    string line;

    vector<string> map;

    // input part 1, the map
    while(getline(cin, line))
    {
        if (line.empty()) break;
        map.push_back(line);
    }

    // input part 2, moves
    string moves;
    while(getline(cin, line))
    {
        if (line.empty()) break;
        moves += line;
    }

    vector<string> map2 = Widen(map);

    Bot bot = FindBot(map);

    for (auto move : moves)
    {
        bot = Move(map, bot, move);

        //cout << "Move " << move << endl;
        //DebugPrint(map);
    }

    DebugPrint(map);
    cout << "P1: " << AllGPS(map) << endl;

    bot = FindBot(map2);
    DebugPrint(map2);

    for (auto move : moves)
    {
        bot = Move(map2, bot, move);

        //cout << "Move " << move << endl;
        //DebugPrint(map2);
    }

    DebugPrint(map2);
    cout << "P2: " << AllGPS(map2) << endl;
    // 1543229 to low
}