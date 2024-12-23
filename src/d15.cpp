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
            if (map[row][col] == 'O') total += GPS({col,row});
        }
    }
    return total;
}

Bot Move(vector<string>& map, const Bot& bot, char m)
{
    Dir d = Direction(m);
    Pos next = bot+d;

    char from = map[bot.row][bot.col];

    // dont go off edge
    if (bot.row < 0 || bot.row >= map.size() || bot.col < 0 || bot.col >= map[0].size())
    {
        return bot;
    }

    // blocked by a wall
    if (map[next.row][next.col] == '#')
    {
        return bot;
    }

    if (map[next.row][next.col] == '.')
    {
        map[bot.row][bot.col] = '.';
        map[next.row][next.col] = from;
        return next;
    }

    if (map[next.row][next.col] == 'O')
    {
        // moved "next" (a wall) out the way recusivly
        if (Move(map, next, m)!=next)
        {
            map[bot.row][bot.col] = '.';
            map[next.row][next.col] = from;
            return next;
        }
    }

    // bot did not move
    return bot;
}

void DebugPrint(const vector<string>& map)
{
    for (auto row : map)
    {
        cout << row << endl;
    }
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

    Bot bot = FindBot(map);

    for (auto move : moves)
    {
        bot = Move(map, bot, move);

        //cout << "Move " << move << endl;
        //DebugPrint(map);
    }

    cout << "P1: " << AllGPS(map) << endl;

}