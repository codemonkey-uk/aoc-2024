#include <iostream>
#include <vector>
#include <list>
#include <cstring>
#include <cctype>

#include "util.h"

using namespace std;

// moved to Util

const size_t expected_size = 5;
typedef vector< string > RowSrc;
typedef vector< pair<int,bool> > Row;
class Board : public vector< Row > 
{
    public:
    void reset(){resize(0);}
    void add(RowSrc src){
        Row r;
        r.reserve(src.size());
        for(auto s : src){
            r.push_back({
                atoi(s.c_str()),
                false
            });
        }
        push_back(move(r));
    }
    
    bool mark(int i)
    {
        for (auto & row : *this)
        {
            for (auto & kvp : row)
            {
                if (kvp.first == i)
                    kvp.second = true;
            }
        }
        
        return win();
    }
    
    bool win() const 
    {
        int column[expected_size]={0};
        for (auto & row : *this)
        {
            int n = 0;
            for (int i = 0;i!=expected_size;++i)
            {
                if (row[i].second)
                {
                    if (++column[i]==expected_size) 
                        return true;
                    n++;
                }
            }
            if (n==expected_size) return true;
        }
        
        return false;
    }
    
    int score(int n) const 
    {
        int sum=0;
        for (auto & row : *this)
        {
            for (int i = 0;i!=expected_size;++i)
            {
                if (!row[i].second)
                    sum += row[i].first;
            }
        }
        
        return n*sum;
    }    
};

void Four()
{
    string line;
    getline(cin,line);
    auto sequence = Split(line, [](auto c){return c==',';});
    
    getline(cin,line);
    assert(line.empty());
    
    list< Board > data;
    Board current;
    while (getline(cin,line))
    {
        if (line.empty())
        {
            assert(current.size()==expected_size);
            data.push_back( move(current) );
            current.reset();
            current.reserve(expected_size);
        }
        else
        {
            current.add( 
                Filter(
                    Split(line,[](auto c){return c==' ';}),
                    [](const auto& str){return str.empty();}
                )
            );
            if (current.back().size()!=expected_size)
                cerr << "error: '" << line << "' split into " << current.back().size() << endl;
        }
    }
    
    if(current.size()==expected_size)
    {
        data.push_back( move(current) );
        current.reset();
        current.reserve(expected_size);
    }

    cout << sequence.size() << " numbers."<< endl;
    cout << data.size()<< " boards."<< endl;
    int winners = 0;
    for (size_t s=0;s!=sequence.size();++s)
    {
        int i = atoi(sequence[s].c_str());
        for ( auto itr = data.begin(); itr!=data.end() ; )
        {
            if (itr->mark(i))
            {
                if (data.size()==1 || winners==0)
                {
                    cout << "winner after " << s << endl;
                    cout << "score = " << itr->score(i) << endl;
                }
                winners++;
                itr = data.erase( itr );
            }
            else
            {
                ++itr;
            }
        }
    }
}


