// twentythree.cpp
// Amphipod

#include <iostream>
#include <deque>
#include "geometry/vector2d.h"
#include "astar.h"

using namespace std;
extern bool debug;

#define PART2

#if defined(PART2)
const int map_h = 6; // was 4 in part 1
#else
const int map_h = 4; // was 4 in part 1
#endif

const int max_y = map_h-2;

const char* start_str[map_h] = {
//  "###########",
    "...........",
    "##B#D#C#A##",
#if defined(PART2)
    " #D#C#B#A# ", // added in part2
    " #D#B#A#C# ", // added in part2
#endif 
    " #C#D#B#A# ",
    " ######### "
//   01234567890
};

const char* goal_str[map_h] = {
//  "###########",//-
    "...........",//0
    "##A#B#C#D##",//1
#if defined(PART2)    
    " #A#B#C#D# ", // added in part2
    " #A#B#C#D# ", // added in part2    
#endif
    " #A#B#C#D# ",//2
    " ######### " //3
//   01234567890
};

const char* blank_str[map_h] = {
//  "###########",//-
    "...........",//0
    "##.#.#.#.##",//1
#if defined(PART2)    
    " #.#.#.#.# ", // added in part2
    " #.#.#.#.# ", // added in part2    
#endif
    " #.#.#.#.# ",//4 (2)
    " ######### " //5 (3)
//   01234567890
};

typedef Geometry::Vector2d<int8_t> Vec;
typedef Geometry::Vector2d<int8_t> Pos;
struct PuzzleState;

constexpr int X=0;
constexpr int Y=1;

bool IsRoomColumn(const Pos::BaseType& pos)
{
    return (pos[X]==2 || pos[X]==4 || pos[X]==6 || pos[X]==8);
}

int RoomIndex(int x)
{
    switch(x){
        case 2: return 0;
        case 4: return 1;
        case 6: return 2;
        case 8: return 3; 
    }
    return -1;
}

int TargetColumn(char type)
{
    switch (type){
        case 'A': return 2;
        case 'B': return 4;
        case 'C': return 6;
        case 'D': return 8;
    }
    return -1;
}

bool MustMove(const Pos& pos) 
{
    return pos.GetY()==0 && IsRoomColumn(pos);
}

struct Amphipod
{
    Pos pos{0,0};
    char type{0};
    bool moving:1;
    Amphipod():moving(false){};
    
    size_t hash() const {
        
        // 
        size_t what = 1+type-'A';//1,2,3,4 = 3 bits
        int where = 0;
        // in a room
        if (pos.GetY()>0)
        {
            // position within the room
            where += pos.GetY()-1; // 0,1,2,3 = 4 spots
            where += RoomIndex(pos.GetX()) * 4;
            // 3*4 = 12 + 3 = 15, 
            // 15*3 = 45 bits
        }
        else
        {
            // some collisions but less then just X
            where = 21 - pos.GetX();
        }
        
        return what << where*3;
        
        /*
        size_t result = pos.GetX() + pos.GetY() * 11;
        result <<= 3;
        result |= type-'A';
        result <<= 1;
        result |= moving;
        return result;
        */
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
    
    int MinStepsToGoal(Pos p2, const PuzzleState& p) const;
    
    int MinStepsToGoal(const PuzzleState& p) const
    {
        return MinStepsToGoal(pos,p);
    }
    
    int CostToGoal(const PuzzleState& p) const
    {
        return MinStepsToGoal(p) * StepCost();
    }
    
    // Amphipods will never stop on the space immediately outside any room
    bool MustMove() const
    {
        return pos.GetY()==0 && IsRoomColumn(pos);
    }
    
    bool CanReachTargetColumn(const PuzzleState& p)const;
    
    bool ValidMove(const PuzzleState& p, Vec::BaseType move)const;
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
#if defined(PART2)
    static constexpr int NumPods=16;    // doubled to 16 in part 2
#else
    static constexpr int NumPods=8;
#endif

    Amphipod pods[NumPods];
    
    PuzzleState ApplyMove(const PuzzleMove& move) const
    {
        PuzzleState result(*this);
        
        assert( IsFree(move.dest) );
        
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
            result += pods[i].CostToGoal(*this);
        return result;
    }
    
    int MinStepsToGoal()const
    {
        int result = 0;
        for (int i=0;i!=NumPods;++i)
        {
            result += pods[i].MinStepsToGoal(*this);
        }
        
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
    
    bool IsFree(const Pos::BaseType& pos)const
    {
        // top & bottom of map
        if (pos[1]<0) return false;
        if (pos[1]>max_y) return false;
        // sides of map
        if (pos[0]<0 || pos[0]>=11) return false;
        // rooms
        if (pos[1]>=1 && !IsRoomColumn(pos)) return false;
        // other pods
        for (int i=0;i!=NumPods;++i)
            if (pods[i].pos==pos) return false;

        return true;
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
            result = result | pods[i].hash();
        }
        return result;
    };
    
    struct iterator{

        const PuzzleState* mFromState;
        int mI=0;
        mutable PuzzleMove mCachedMove;
        
        iterator(const PuzzleState* current) : mFromState(current) {}
        iterator(const PuzzleState* current, int i ) : mFromState(current), mI(i) {}
        iterator() : mFromState(nullptr) {} 
        
        //copy constructor and assignment operator should be available

        typedef int cost_type; //typedef required, must be scalar type

        PuzzleState operator*()const{return value();}
        
        PuzzleState value()const{
            mCachedMove = mFromState->GetMove(mI);
            return mFromState->ApplyMove(mCachedMove);
        }
        
        cost_type cost()const{
             return mFromState->pods[mCachedMove.pod_index].StepCost() * 
                mFromState->pods[mCachedMove.pod_index].pos.ManhattanDistance(
                    mCachedMove.dest
                );
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

int Amphipod::MinStepsToGoal(Pos p2, const PuzzleState& p) const
{
    // distance to goal is distance to column 
    int steps = abs(p2.GetX()-TargetColumn());

    // in the wrong room: add move to hallway distance & 1 step
    if (p2.GetX()!=TargetColumn())
    {
        steps += p2.GetY() + 1;
    }
    else 
    {
        // once in the target room
        // need to fill gaps downwards
        while (p2.GetY()<max_y) 
        {
            p2.SetY( p2.GetY()+1 );
            if (p.GetChar(p2)!=type) steps++;
        }
    }
    
    // cout << type << " - " << steps << endl;
    return steps;
}
    
bool Amphipod::CanReachTargetColumn(const PuzzleState& p)const
{
    assert( pos.GetY()==0 );
    
    int tx = TargetColumn();
    if (tx==pos.GetX()) return true;
    
    int dx = pos.GetX()>tx?-1:1;
    Pos p2=pos;
    while (p2.GetX()!=tx)
    {
        p2 += Vec(dx,0);
        if (!p.IsFree(p2)) return false;
    }
    
    return true;
}

bool Amphipod::ValidMove(const PuzzleState& p, Vec::BaseType dest)const
{
    // Once an amphipod stops moving in the hallway, 
    // it will stay in that spot until it can move into a room.
    if (!moving && pos[Y]==0)
    {
        if (!CanReachTargetColumn(p)) return false;
        if (p.IsRoomFree(type)==false) return false;
    }

    // move from hallway into room
    if (pos[Y]==0 && dest[Y]>=1)
    {
        // only move into your own room
        if (TargetColumn()!=dest[X]) return false;
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
        Pos dest(pos);
        while (ValidMove(p,dest+=m))
        {
            if (::MustMove(dest))
            {
                for(Vec m2:moves)
                {
                    if (m2!=-m)
                    {
                        Pos dest2(dest);
                        while (ValidMove(p,dest2+=m2))
                            total++;
                    }
                }
            }
            else
            {
                total++;
            }
        }
    }
    return total;
}

Pos Amphipod::GetMove(const PuzzleState& p, int i)const
{
    for(Vec m:moves)
    {
        Pos dest(pos);
        while (ValidMove(p,dest+=m))
        {
            if (::MustMove(dest))
            {
                for(Vec m2:moves)
                {
                    if (m2!=-m)
                    {
                        Pos dest2(dest);
                        while (ValidMove(p,dest2+=m2))
                        {
                            if (i==0)
                                return dest2;
                            --i;
                        }
                    }
                }
            }
            else
            {
                if (i==0)
                    return dest;
                --i;
            }
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
    cout << "###########" << endl;
    for (int y=0;y!=map_h;++y)
    {
        char c;
        for (int x=0;(c=GetChar(Vec(x,y)));++x)
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

#if !defined(PART2)
    const char * three_moves_left[] = {
    "...A.......",
    "##A#B#C#D##",
    " #.#B#C#D# ",
    " ######### "};
    PuzzleState c = ReadPuzzle(three_moves_left);
    assert(c.MinStepsToGoal()<=3);
    
    const char * five_moves_left[] = {
    ".A.A.......",
    "##.#B#C#D##",
    " #.#B#C#D# ",
    " ######### "};
    PuzzleState d = ReadPuzzle(five_moves_left);
    assert(d.MinStepsToGoal()<=5);
#endif

};

void ReportProgress(const PuzzleState& puzzle)
{
    puzzle.Print();
    cout << "At least " << puzzle.MinStepsToGoal() << " moves to go." << endl;
    cout << "Minimum energy cost " << puzzle.CostToGoal() << "." << endl;
    cout << puzzle.CountMoves() << " possible moves." << endl;
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

#if defined(PART2)    
    cout << "-- PART 2 --" << endl;
#else
    cout << "-- PART 1 --" << endl;
#endif

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

            // expected part 1 cost = 15322
            // 284.44s user
            cout << "Nodes opened: " << cfg.result_nodes_opened << endl;
        }
        else
        {
            cout << "No solution found!" << endl;
            cout << "Nodes opened: " << cfg.result_nodes_opened << endl;
            cout << "Nodes pending: " << cfg.result_nodes_pending << endl;
            cout << "Best found: " << endl;
            ReportProgress(results.back());
        }
    }
}
