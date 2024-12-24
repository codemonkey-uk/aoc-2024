#include <iostream>
#include <queue>

#include "util.h"

using namespace std;
typedef int64_t BigInt;
typedef Pos Dir;

Pos Find(char c, const vector<string>& map)
{
    for (uint row=0;row!=map.size();row++)
    {
        for (uint col=0;col!=map[row].size();col++)
        {
            if (map[row][col] == c)
            {
                return {static_cast<int>(col),static_cast<int>(row)};
            }
        }
    }
    return {-1,-1};
}

Pos FindStart(const vector<string>& map)
{
    return Find('S', map);
}

Pos FindEnd(const vector<string>& map)
{
    return Find('E', map);
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

constexpr char RotateClockwise(char c)
{
    switch(c)
    {
        case '>': return 'v';
        case '<': return '^';
        case '^': return '>';
        case 'v': return '<';;
    }
    return c;
}
constexpr char RotateAnitClockwise(char c)
{
    switch(c)
    {
        case '>': return '^';
        case '<': return 'v';
        case '^': return '<';
        case 'v': return '>';
    }
    return c;
}

struct Move 
{
    Move(char m) : m(m) {}

    // T = translate, 
    // C = clockwise, A = anticlockwise
    char m;

    // move in current direction (T) = 1, or rotate (C,A) = 1000
    int Cost() const { return m == 'T' ? 1 : 1000; }
    bool IsMove() const { return m == 'T'; }
    bool IsRotate() const { return !IsMove(); }
};

struct Bot
{
    // ordered for optimal struct packing, 
    // largest/strictest alignement first
    BigInt cost;
    Pos p;
    char c;

    Dir Direction() const { return ::Direction(c); }
    Bot Rotate(char m) const
    {
        Bot result(*this);
        result.c = m == 'C' ? RotateClockwise(c) : RotateAnitClockwise(c);
        result.cost += 1000;
        return result;
    }
    Bot Translate(Dir d) const
    {
        Bot result(*this);
        result.p += d;
        result.cost += 1;
        return result;
    }

    bool operator==(const Bot& b) const { return p == b.p && c == b.c; }
    bool operator!=(const Bot& b) { return !(*this==b); }

    Bot Move(vector<string>& map, Move m, bool commitMove=true) const
    {
        // you can always rotate
        if (m.IsRotate())
        {   
            if (commitMove)
            {
                map[p.row][p.col] = c;
            }
            return Rotate(m.m);
        }

        // if not a rotation, must be translation in current direction
        Dir d = Direction();
        const Bot next = Translate(d);

        // dont go off edge
        if (next.p.row < 0 || next.p.row >= map.size() || 
            next.p.col < 0 || next.p.col >= map[0].size())
        {
            return *this;
        }

        const char too = map[next.p.row][next.p.col];    

        // can only enter empty spaces
        if (too == '.' || too == 'E')
        {
            if (commitMove)
            {
                map[next.p.row][next.p.col] = c;
            }
            return next;
        }

        // bot did not move
        return *this;
    }
};

static void DebugPrint(const vector<string>& map)
{
    for (auto row : map)
    {
        cout << row << endl;
    }
}

struct BotCompare
{
    bool operator()(Bot l, Bot r) const { return l.cost > r.cost; }
};

BigInt FindPath(Bot start, Pos goal, vector<string>& map, int limit=INT_MAX)
{
    // priorty queue of bots ordered by cost
    priority_queue<Bot, std::vector<Bot>, BotCompare> q;

    q.push(start);
    q.push(start.Move(map, Move('C')));
    q.push(start.Move(map, Move('A')));    

    while (!q.empty() && limit--)
    {
        //DebugPrint(map);

        Bot bot = q.top();
        q.pop();

        // if we reached the goal, return the cost
        if (bot.p == goal)
        {
            return bot.cost;
        }

        // if able to translate
        Bot next = bot.Move(map, Move('T'));
        if (next!=bot)
        {
            q.push(next);
            // and add rotations
            q.push(next.Move(map, Move('C')));
            q.push(next.Move(map, Move('A')));
        }
    }

    // no path found
    return -1;
};

void Sixteen()
{
    string line;

    vector<string> map;

    // input part 1, the map
    while(getline(cin, line))
    {
        if (line.empty()) break;
        map.push_back(line);
    }

    Bot bot;
    bot.cost = 0;
    bot.p = FindStart(map);
    bot.c = '>'; // start facing east

    Pos goal = FindEnd(map);

    auto result = FindPath(bot, goal, map);
    DebugPrint(map);

    cout << "P1: " << result << endl;
}