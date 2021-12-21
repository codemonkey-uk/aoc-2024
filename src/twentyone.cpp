// twentyone.cpp
// Dirac Dice
#include <iostream>
using namespace std;

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

void TwentyOne()
{
    // sample:
    //player one(4);
    //player two(8);

    //Player 1 starting position: 4
    //Player 2 starting position: 1
    player one(4);
    player two(1);
    
    
    do{
        one.turn();
        if (one.score >= 1000) break;
        two.turn();
        if (two.score >= 1000) break;
    }while(true);
    cout << "game over after "<< deterministic_rolls<<endl;
    int worst_score = one.score<two.score ? one.score : two.score;
    cout << "loosing player score "<< worst_score<<endl;
    cout << "part 1 answer: " << worst_score*deterministic_rolls<<endl;
    
}