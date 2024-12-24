#include <iostream>
#include <queue>
#include <set>
#include <map>
#include "util.h"

const bool veryVerbose=false;

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

int Count(char c, const vector<string>& map)
{
    int result=0;
    for (uint row=0;row!=map.size();row++)
    {
        for (uint col=0;col!=map[row].size();col++)
        {
            if (map[row][col] == c)
            {
                result++;
            }
        }
    }
    return result;
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

    static set<Bot> history;
    typedef set<Bot>::iterator iterator;
    iterator previous = history.end();
    
    
    Dir Direction() const { return ::Direction(c); }
    Bot Rotate(char m) const
    {
        Bot result(*this);
        result.previous = history.insert(*this).first;
        result.c = m == 'C' ? RotateClockwise(c) : RotateAnitClockwise(c);
        result.cost += 1000;
        return result;
    }
    Bot Translate(Dir d) const
    {
        Bot result(*this);
        result.previous = history.insert(*this).first;
        result.p += d;
        result.cost += 1;
        return result;
    }

    bool HistoryLess(const Bot& b) const
    {
        // same history, they are the same node
        if (previous == b.previous) return false;

        // if no history, then this is less than b
        if (previous == history.end()) return true;

        //  compare previous nodes 
        return *previous < *b.previous;
    }
    // for set
    bool operator<(const Bot& b) const 
    {
        return p< b.p || 
              (p==b.p && c < b.c) || 
              (p==b.p && c==b.c && HistoryLess(b));
    }

    bool operator==(const Bot& b) const { return p == b.p && c == b.c; }
    bool operator!=(const Bot& b) const { return !(*this==b); }

    Bot Move(const vector<string>& map, Move m) const
    {
        // you can always rotate
        if (m.IsRotate())
        {   
            //if (commitMove)
            //{
            //    map[p.row][p.col] = c;
            //}
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
            //if (commitMove)
            //{
            //    map[next.p.row][next.p.col] = c;
            //}
            return next;
        }

        // bot did not move
        return *this;
    }
};

//static
set<Bot> Bot::history;

static void DebugPrint(const vector<string>& map)
{
    for (auto row : map)
    {
        cout << row << endl;
    }
}
static void DebugPrint(Bot bot, Pos start, vector<string> debugMap)
{
    cout << "bot " << bot.p.col << "," << bot.p.row << " " << bot.c << " " << bot.cost << endl;
    while(bot.p != start)
    {
        debugMap[bot.p.row][bot.p.col] = bot.c;
        //bot.p -= ::Direction(map[bot.p.row][bot.p.col]);
        bot = *bot.previous;              
    }
    DebugPrint(debugMap);
}

struct BotCostCompare
{
    bool operator()(Bot l, Bot r) const { return l.cost > r.cost; }
};

BigInt FindPath(Bot start, Pos goal, vector<string>& mapInOut, int limit=INT_MAX)
{
    // the working map will contain incomplete routes
    const vector<string> map = mapInOut;

    BigInt cost_limit = numeric_limits<BigInt>::max();

    std::map<char, Grid<BigInt> > costs;
    costs['>'] = Grid<BigInt>(map[0].size(), map.size());
    costs['>'].fill(cost_limit);
    costs['v'] = Grid<BigInt>(map[0].size(), map.size());
    costs['v'].fill(cost_limit);
    costs['<'] = Grid<BigInt>(map[0].size(), map.size());
    costs['<'].fill(cost_limit);
    costs['^'] = Grid<BigInt>(map[0].size(), map.size());
    costs['^'].fill(cost_limit);

    // priorty queue of bots ordered by cost
    priority_queue<Bot, std::vector<Bot>, BotCostCompare> q;

    q.push(start);
    q.push(start.Move(map, Move('C')));
    q.push(start.Move(map, Move('A')));    

    const int initialLimit = limit;

    while (!q.empty() && limit--)
    {
        const Bot bot = q.top();
        q.pop();

        if (veryVerbose) 
        {
            DebugPrint(bot, start.p, map);
        }

        if (bot.cost > costs[bot.c].get(bot.p.col, bot.p.row)) continue;
        costs[bot.c].get(bot.p.col, bot.p.row) = bot.cost;

        if (bot.cost > cost_limit) break;

        // if we reached the goal, save the cost
        if (bot.p == goal)
        {
            cost_limit = bot.cost;
            // copy the route the bot took into the map but walking it back
            Bot b=bot;
            while(b.p != start.p)
            {
                mapInOut[b.p.row][b.p.col] = 'O';
                b = *b.previous;
            }
            cout << "Found path with cost " << bot.cost << endl;
            DebugPrint(mapInOut);
            continue;
        }

        Bot next = bot.Move(map, Move('T'));
        if (next!=bot)
        {
            if (veryVerbose) 
                cout << "-> " << next.p.col << "," << next.p.row << " " << next.c << " " << next.cost << endl;
            
            // allow for bots to be rotated  before or after arrivals
            BigInt cost = costs[next.c].get(next.p.col, next.p.row);
            if (next.cost <= cost)
            {
                q.push(next);
                
                // and add rotations
                Bot next_c = next.Move(map, Move('C'));
                if (next_c.cost < costs[next_c.c].get(next_c.p.col, next_c.p.row))
                {
                    q.push(next_c);
                    costs[next.c].get(next.p.col, next.p.row) = next.cost;
                }
                Bot next_a = next.Move(map, Move('A'));
                if (next_a.cost < costs[next_a.c].get(next_a.p.col, next_a.p.row))
                {
                    q.push(next_a);
                    costs[next.c].get(next_a.p.col, next_a.p.row) = next.cost;
                }
            }
            else if (veryVerbose)
            {
                cout << "skip " << next.p.col << "," << next.p.row << ": " 
                    << next.cost << " > " <<  costs[next.c].get(next.p.col, next.p.row) << endl;
            }
        }
        else  if (veryVerbose)
            {
                cout << "blocked " << next.p.col << "," << next.p.row << ";" << endl;
            }
    }

    if (cost_limit == numeric_limits<BigInt>::max())
    {
        cerr << "Failed after " << initialLimit-limit << " iterations, with " 
            << q.size() << " pending nodes." << endl;
    }

    // no path found
    return cost_limit;
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
    cout << "P2: " << Count('O',map)+1 << endl;
}