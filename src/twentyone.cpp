// twentyone.cpp
// Dirac Dice
#include <iostream>
#include <vector>
using namespace std;

namespace Part1
{
    static int deterministic_dice = 0;
    static int deterministic_rolls = 0;
    int roll()
    {
        int r = deterministic_dice+1;
        deterministic_rolls++;
        deterministic_dice = r%100;
        return r;
    }

    struct player
    {
        player(int start):pos(start-1),score(0){}
        int pos;
        int score;
        void turn(){
            int move = roll()+roll()+roll();
            pos = (pos + move) % 10;
            score = score + pos + 1;
        }
    };

    void TwentyOne(int p1, int p2)
    {
        deterministic_dice = 0;
        deterministic_rolls = 0;
        
        player one(p1);
        player two(p2);
    
        do{
            one.turn();
            if (one.score >= 1000) break;
            two.turn();
            if (two.score >= 1000) break;
        }while(true);
        cout << "game over after "<< deterministic_rolls<<endl;
        int worst_score = one.score<two.score ? one.score : two.score;
        cout << "loosing player score "<< worst_score<<endl;
        cout << "answer: " << worst_score*deterministic_rolls<<endl;
    }
    
    void TwentyOne()
    {
        cout << "--- Part 1 (Sample) ---" << endl;
        TwentyOne(4,8);
        cout << "--- Part 1 ---" << endl;
        TwentyOne(4,1);
    }
}

namespace Part2
{
    struct Player
    {
        int pos=0;
        int score=0;
        
        bool operator==(const Player& rhs)const{
            return pos==rhs.pos && score==rhs.score;
        }
        
        int encode() const 
        {
            return (score << pos_bits) | pos;
        }
        
        static Player decode(int i) 
        {
            Player r;
            r.score = i >> pos_bits;
            r.pos = i & pos_mask;
            return r;
        }
        
        Player Advance(int distance) const
        {
            Player result;
            result.pos = (this->pos + distance) % 10;
            result.score = this->score + result.pos + 1;
            return result;
        }
        
        static constexpr int pos_bits = 4;
        static constexpr int score_bits = 5; // 0...21 
        static constexpr int bits = pos_bits+score_bits;
        static constexpr int pos_mask = (1<<pos_bits)-1;
    };
    
    // whos turn is encoded outside the gamestate
    struct GameState
    {
        GameState(){}
        GameState(int p1, int p2)
        {
            player[0].pos=p1-1;
            player[1].pos=p2-1;
        }
        
        Player player[2];
        bool operator==(const GameState& rhs)const{
            return player[0]==rhs.player[0] && player[1]==rhs.player[1];
        }
        bool operator!=(const GameState& rhs)const{
            return !(*this==rhs);
        }

        int encode() const
        {
            return (player[0].encode() << Player::bits) | player[1].encode();
        }
        
        static GameState decode(int i)
        {
            GameState g;
            g.player[0] = Player::decode( i >> Player::bits );
            g.player[1] = Player::decode( i & player_mask );
            return g;
        }
        
        GameState Advance(int distance, int turn) const
        {
            GameState next = *this;
            next.player[turn]=player[turn].Advance(distance);
            return next;
        }
        
        void DebugPrint() const 
        {
            cout << 'p' << (player[0].pos+1) << ":s=" << player[0].score << " & ";
            cout << 'p' << (player[1].pos+1) << ":s=" << player[1].score << endl;
        }
        
        bool Finished(int turn) const
        {
            return player[turn].score >= 21;
        }
        
        static constexpr int bits = Player::bits*2;
        static constexpr int player_mask = (1<<Player::bits)-1;
    };
    
    class QuantumCalculator
    {
        public:
        QuantumCalculator(int p1, int p2)
        {
            InitMoves();
            universes[0].resize( size );
            universes[1].resize( size );

            GameState g(p1,p2);
            int j = g.encode();
            universes[turn][j]=1;
        }
        
        int Step(bool verbose=false)
        {
            int count=0;
            const int next = 1-turn;
            fill(universes[next].begin(),universes[next].end(),0);
            for (int i=0;i!=size;++i)
            {
                const long long ux = universes[turn][i];
                if (ux>0)
                {
                    const GameState g=GameState::decode(i);
                    
                    if (verbose) cout << ux << " x ";
                    if (verbose) g.DebugPrint();
                    
                    for (int j=0;j!=threeRollOutcomeCount;++j)
                    {
                        int dist = j+threeOutcomeFirst;
                        GameState n = g.Advance(dist, turn);
                        
                        if (verbose) cout << "+ " << dist << " = ";
                        if (verbose) n.DebugPrint();
                        
                        long long nc = ux*movesHistogram[j];
                        if (n.Finished(turn))
                        {
                            wins[turn] += nc;
                        }
                        else
                        {
                            if(GameState::decode( n.encode() ) != n)
                            {
                                cerr<<"ENCODE ERROR:"<<endl;
                                n.DebugPrint();
                                GameState::decode( n.encode() ).DebugPrint();
                                exit(1);
                            }
                            
                            universes[next][n.encode()] += nc;
                            count ++;
                        }
                    }
                }
            }
            
            turn = next;
            return count;
        }
        
        void DebugPrint()const
        {
            for (int i=0;i!=size;++i)
            {
                if (universes[turn][i]>0)
                {
                    GameState g=GameState::decode(i);
                    cout << universes[turn][i] << " x ";
                    g.DebugPrint();
                }
            }
            PrintScores();
        }
        
        void PrintScores()const
        {
            cout << "Player 1 wins: " << wins[0] << endl;
            cout << "Player 2 wins: " << wins[1] << endl;
        }
        
        private:
        static constexpr int size = 1<<GameState::bits;
        long long wins[2]={0,0};
        vector<long long> universes[2];
        int turn=0;
        
        void InitMoves()
        {
            for (int a=0;a<3;a++)
                for (int b=0;b<3;b++)
                    for (int c=0;c<3;c++)
                        movesHistogram[a+b+c]++;

            for (int i=0;i<threeRollOutcomeCount;++i)
            {
                cout << i+threeOutcomeFirst << ":" << string(movesHistogram[i],'#')<<endl;
            }   
        }
        static constexpr int threeRollOutcomeCount = 7;
        static const int threeOutcomeFirst = 3;
        int movesHistogram[threeRollOutcomeCount]={0};
    };
    
    void TwentyOne()
    {
        assert( Player::decode(1).encode()==1 );
        assert( Player::decode(0).encode()==0 );
        assert( GameState::decode(1).encode()==1 );
        assert( GameState::decode(2).encode()==2 );

        cout << GameState::bits << " bits of gamestate per turn" << endl;


        //QuantumCalculator c(4,8);
        QuantumCalculator c(4,1);
        int n=c.Step(true);
        c.DebugPrint();

        while(n)
        {
            n=c.Step();
            c.PrintScores();
        }
        
        // cout << "Finished after " << steps << " steps." << endl;
        c.DebugPrint();
    }
}

void TwentyOne()
{
    Part1::TwentyOne();
    Part2::TwentyOne();
}