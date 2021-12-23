// twentythree.cpp
// Amphipod

#include <iostream>
#include <deque>
#include "geometry/vector2d.h"
#include "astar.h"

using namespace std;
extern bool debug;

const int map_h = 4;

// #define USE_SAMPLE

// start TEST
#ifndef USE_SAMPLE
const char* start_str[map_h] = {
//  "#############",
    "#...........#",
    "###B#D#C#A###",
    "  #C#D#B#A#  ",
    "  #########  "
};
#else
// start EXAMPLE
const char* start_str[map_h] = {
//  "#############"
    "#...........#",
    "###B#C#B#D###",
    "  #A#D#C#A#  ",
    "  #########  "
};
#endif

const char* goal_str[map_h] = {
//  "#############",//-
    "#...........#",//0
    "###A#B#C#D###",//1
    "  #A#B#C#D#  ",//2
    "  #########  " //3
//   01234567890AB
};

const char* blank_str[map_h] = {
//  "#############",//-
    "#...........#",//0
    "###.#.#.#.###",//1
    "  #.#.#.#.#  ",//2
    "  #########  " //3
//   01234567890AB
};

typedef Geometry::Vector2d<int> Vec;
typedef Geometry::Vector2d<int> Pos;
struct PuzzleState;

bool IsRoomColumn(const Pos& pos)
{
    return (pos.GetX()==3 || pos.GetX()==5 || pos.GetX()==7 || pos.GetX()==9);
}

int TargetColumn(char type)
{
    switch (type){
        case 'A': return 3;
        case 'B': return 5;
        case 'C': return 7;
        case 'D': return 9;
    }
    return -1;
}


struct Amphipod
{
    Pos pos{0,0};
    char type{0};
    bool moving{false};
    
    size_t hash() const {
        size_t result = (pos.GetX()-1) + pos.GetY() * 11;
        result <<= 3;
        result |= type-'A';
        result <<= 1;
        result |= moving;
        return result;
    }
    
    int StepCost() const
    {
        switch (type){
            case 'A': return 1;
            case 'B': return 10;
            case 'C': return 100;
            case 'D': return 1000;
        }
        return -1;
    }
    
    int TargetColumn() const
    {
        return ::TargetColumn (type);
    }
    
    int MinStepsToGoal(Pos p2) const
    {
        // in the hallway, 
        // distance to goal is distance to column + 1 step into it
        if (p2.GetY()==0)
            return abs(p2.GetX()-TargetColumn())+1;
        // in the target room
        if (p2.GetX()==TargetColumn()) 
            return 0;
        else 
        // in the wrong room: move to hallway, move to column, move down 1
            return p2.GetY() + abs(p2.GetX()-TargetColumn()) + 1;
    }
    
    int MinStepsToGoal() const
    {
        return MinStepsToGoal(pos);
    }
    
    // Pos Goal(const PuzzleState&);
    
    int CostToGoal() const
    {
        return MinStepsToGoal() * StepCost();
    }
    
    // Amphipods will never stop on the space immediately outside any room
    bool MustMove() const
    {
        return pos.GetY()==0 && IsRoomColumn(pos);
    }
    
    bool CanReachTargetColumn(const PuzzleState& p)const;
    
    bool ValidMove(const PuzzleState& p, Vec move)const;
    int CountMoves(const PuzzleState&)const;
    Pos GetMove(const PuzzleState& p, int i)const;
};

struct PuzzleMove
{
    int pod_index{-1};
    Pos dest{0,0};
};

struct PuzzleState
{
    static constexpr int NumPods=8;
    Amphipod pods[NumPods];
    
    PuzzleState ApplyMove(const PuzzleMove& move) const
    {
        PuzzleState result(*this);
        
        assert( IsFree(move.dest) );
        assert( astar::manhattan_distance(
            result.pods[move.pod_index].pos[0],
            result.pods[move.pod_index].pos[1],
            move.dest[0],
            move.dest[1])==1 );
        
        result.pods[move.pod_index].pos=move.dest;

        for (int i=0;i!=NumPods;++i)
            result.pods[i].moving = (i==move.pod_index && move.dest.GetY()==0);
        
        return result;
    }
    
    bool Finished() const 
    {
        return MinStepsToGoal()==0;
    }
    
    int CostToGoal()const
    {
        int result = 0;
        for (int i=0;i!=NumPods;++i)
            result += pods[i].CostToGoal();
        return result;
    }
    
    int MinStepsToGoal()const
    {
        int result = 0;
        for (int i=0;i!=NumPods;++i)
            result += pods[i].MinStepsToGoal();
        return result;
    }
    
    PuzzleMove GetMove(int move)const
    {
        for(int i=0;i!=NumPods;++i)
            if (pods[i].MustMove()) 
                return PuzzleMove{i,pods[i].GetMove(*this, move)};

        for(int i=0;i!=NumPods;++i)
        {
            int n = pods[i].CountMoves(*this);
            if (move<n) 
                return PuzzleMove{i,pods[i].GetMove(*this, move)};
            move -= n;
        }
        
        return PuzzleMove{};
    }
    
    int CountMoves()const
    {
        for(int i=0;i!=NumPods;++i)
            if (pods[i].MustMove()) return pods[i].CountMoves(*this);
        int total=0;
        for(int i=0;i!=NumPods;++i)
            total += pods[i].CountMoves(*this);
        return total;
    };
    
    bool IsFree(const Pos& pos)const
    {
        for (int i=0;i!=NumPods;++i)
            if (pods[i].pos==pos) return false;
        
        if (pos.GetY()<0) return false;
        if (pos.GetY()>2) return false;
        
        if (pos.GetY()==0)
            return pos.GetX()>0 && pos.GetX()<12;
        else if (pos.GetY()<=2)
            return IsRoomColumn(pos);
        return false;
    }
    
    bool IsRoomFree(char type) const
    {
        int x = TargetColumn(type);
        for(int i=0;i!=NumPods;++i)
            if (pods[i].pos.GetX()==x && pods[i].type!=type) return false;
        return true;
    }
    
    char GetChar(Pos p) const;
    void Print() const;
    
    // -- astar node interface
    size_t hash() const {
        size_t result=0;
        for (int i=0;i!=NumPods;++i)
        {
            result = result * 31 + pods[i].hash();
        }
        return result;
    };
    
    struct iterator{

        const PuzzleState* mFromState;
        int mI=0;
        
        iterator(const PuzzleState* current) : mFromState(current) {}
        iterator(const PuzzleState* current, int i ) : mFromState(current), mI(i) {}
        iterator() : mFromState(nullptr) {} 
        
        //copy constructor and assignment operator should be available

        typedef int cost_type; //typedef required, must be scalar type

        PuzzleState operator*()const{return value();}
        
        PuzzleState value()const{
            return mFromState->ApplyMove(
                mFromState->GetMove(mI)
            );
        }
        
        cost_type cost()const{
             return mFromState->pods[mFromState->GetMove(mI).pod_index].StepCost();
        }
    
        iterator& operator++(){mI++;return*this;}
        bool operator!=(iterator v){
            return (mI!=v.mI);// || *mFromState!=*v.mFromState);
        };//used by search

    };

    //Get first, and past-end iterators
    iterator begin()const{return iterator(this);}
    iterator end()const{return iterator(this,CountMoves());}

    //equality operator, required
    //note: fuzzy equality may be useful
    bool operator==(const PuzzleState& rhs)const{
        for (int i=0;i!=NumPods;++i)
        {
            if (rhs.GetChar(pods[i].pos)!=GetChar(pods[i].pos)) 
                return false;
        }
        assert(MinStepsToGoal()==rhs.MinStepsToGoal());
        return true;
    }
    bool operator!=(const PuzzleState& b)const{
        return !(*this==b);
    }
};

bool Amphipod::CanReachTargetColumn(const PuzzleState& p)const
{
    assert( pos.GetY()==0 );
    
    int tx = TargetColumn();
    if (tx==pos.GetX()) return true;
    
    int dx = pos.GetX()>tx?-1:1;
    Pos p2=pos;
    while (p2.GetX()!=tx)
    {
        p2 += {dx,0};
        if (!p.IsFree(p2)) return false;
    }
    
    return true;
}

bool Amphipod::ValidMove(const PuzzleState& p, Vec move)const
{
    Pos dest = pos;
    dest += move;
 
     assert(astar::manhattan_distance(pos[0],pos[1],dest[0],dest[1]));
     
    // Once an amphipod stops moving in the hallway, 
    // it will stay in that spot until it can move into a room.
    if (!moving && pos.GetY()==0)
    {
        if (!CanReachTargetColumn(p)) return false;
        if (p.IsRoomFree(type)==false) return false;
    }

    // move from hallway into room
    if (pos.GetY()==0 && dest.GetY()==1)
    {
        // only move into your own room
        if (TargetColumn()!=dest.GetX()) return false;
        if (p.IsRoomFree(type)==false) return false;
    }
    
    // basic space-free check
    if (!p.IsFree(dest)) return false;
    
    return true;
}

static const Vec moves[]={{1,0},{0,1},{-1,0},{0,-1}};

int Amphipod::CountMoves(const PuzzleState& p)const
{
    int total = 0;
    for(Vec m:moves)
    {
        if (ValidMove(p,m))
            total++;
    }
    return total;
}

Pos Amphipod::GetMove(const PuzzleState& p, int i)const
{
    for(Vec m:moves)
    {
        if (ValidMove(p,m))
        {
            if (i==0)
                return Pos(pos+m);
            --i;
        }
    }
    assert(false);
    return Pos{0,0};
}

char PuzzleState::GetChar(Pos p) const
{
    char c = blank_str[p.GetY()][p.GetX()];
    
    for (int i=0;i!=NumPods;++i)
    {
        if (pods[i].pos==p)
        {
            c = pods[i].type;
            if (pods[i].moving) c=tolower(c);
            break;
        }
    }
    
    return c;
}

void PuzzleState::Print() const
{
    cout << "hash: " << hash() << endl;
    cout << "#############" << endl;
    for (int y=0;y!=map_h;++y)
    {
        char c;
        for (int x=0;(c=GetChar({x,y}));++x)
        {
            cout << c;
        }
        cout << endl;
    }
}

PuzzleState ReadPuzzle(const char** puzzle_str)
{
    int count=0;
    PuzzleState result;
    for (int y=0;y!=map_h;y++)
    {
        for (int x=0;puzzle_str[y][x];++x)
        {
            char c = puzzle_str[y][x];
            if (c>='A' && c<='D')
            {
                result.pods[count].pos.SetX(x);
                result.pods[count].pos.SetY(y);
                result.pods[count].type=c;
                count++;
            }
        }
    }
    assert(count==PuzzleState::NumPods);
    sort(&result.pods[0],&result.pods[count],
        [](const auto& a, const auto& b){
            if (a.type < b.type) return true;
            else if (a.type==b.type)
            {
                if (a.pos.GetX()<b.pos.GetX()) return true;
                else if (a.pos.GetX()==b.pos.GetX()) 
                    return (a.pos.GetY()<b.pos.GetY());
            }
            return false;
        }
    );
    
    return result;
}

void SelfTest()
{
    PuzzleState a = ReadPuzzle(start_str);
    PuzzleState b = ReadPuzzle(goal_str);
    assert(a.Finished()==false);
    assert(b.Finished()==true);
};

void ReportProgress(const PuzzleState& puzzle)
{
    puzzle.Print();
    cout << "At least " << puzzle.MinStepsToGoal() << " moves to go." << endl;
    cout << "Minimum energy cost " << puzzle.CostToGoal() << "." << endl;
    cout << puzzle.CountMoves() << " possible moves." << endl;

    if (puzzle.hash()==86284223)
    {
        cout << "--- MOVES: " << endl;
        for (auto m:puzzle)
        {
            m.Print();
        }
        cout << "--- (end)" << endl;
    }
}

struct astar_behaviour
{
    int best_node_h = 0;
    double heuristic( const PuzzleState& a,const  PuzzleState&)
    {
        // report progress
        auto result = a.CostToGoal();
        if (result<best_node_h)
        {
            cout << result << endl;
            a.Print();
            best_node_h= result;
        }
        
        return result;
    }
    bool filter( const PuzzleState& )
    {
        return false;
    }
};

void TwentyThree()
{
    SelfTest();
    PuzzleState puzzle = ReadPuzzle(start_str);
    PuzzleState end = ReadPuzzle(goal_str);
    
    cout << "-- FROM --" << endl;
    ReportProgress(puzzle);
    cout << "-- TO --" << endl;
    ReportProgress(end);
    
    {
        cout << "-- GO --" << endl;

        astar_behaviour behaviour;
        behaviour.best_node_h = puzzle.CostToGoal();
        
        deque< PuzzleState > results;
        astar::config<PuzzleState> cfg;
    
        if (astar::astar(puzzle, end, results, behaviour, cfg))
        {
            cout << "Success!" << endl;
            if (debug) 
            {
                auto itr = results.begin();
                auto last = *itr++;
                while(itr!=results.end())
                {
                    auto r = *itr++;
                    ReportProgress(r);
                    // is r a child of last? 
                    assert( find(last.begin(),last.end(),r)!=last.end() );
                    last=r;
                }
            }
            cout << "Total moves: " << results.size() << endl;
            cout << "Total cost: " << cfg.route_cost << endl;
        }
    }    
}