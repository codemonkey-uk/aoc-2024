#include <iostream>
#include <deque>
#include <map>
#include <set>

#include "util.h"

using namespace std;

size_t Measure(const string& d)
{
    size_t total=0;
    for(char c : d)
        total += c-'0';
    return total;
}

void DebugPrint(const vector<int>& content)
{
    for (int i : content)
        if (i==-1) cout << ".";
        else cout << i;

    cout << endl;
}

vector<int> Expand(const string& d)
{
    vector<int> result;
    result.reserve( Measure(d) );

    int id = 0;
    int i=0;
    while(i<d.size())
    {
        int n = d[i]-'0';
        while (n--) result.push_back(id);
        i++;
        id++;
        if (i==d.size()) break;

        n = d[i]-'0';
        while (n--) result.push_back(-1);
        i++;
    }

    if (id<=10)
    {
        DebugPrint(result);
    }
    else 
    {
        cout << id << " IDs"<<endl;
    }

    return result;
}

void Defrag(vector<int>& memory)
{
    auto nextFree = find(memory.begin(), memory.end(), -1);
    auto end = find_if(memory.rbegin(), memory.rend(), [](int i) { return i!=-1; });
    while (nextFree!=memory.end() && end!=memory.rend() && nextFree<end.base())
    {
        swap(*end, *nextFree);
        //DebugPrint(memory);
        nextFree = find(nextFree, memory.end(), -1);
        end = find_if(end, memory.rend(), [](int i) { return i!=-1; });        
    }
}

size_t Checksum(const vector<int>& memory)
{
    size_t total=0;
    for (size_t i=0;i!=memory.size();++i)
        if (memory[i]!=-1)
            total = total + i*memory[i];

    return total;
}

void Nine()
{
    string line;
    while (std::getline(std::cin, line))
    {
        string dense = line;
        auto map = Expand(dense);
        Defrag(map);
        cout << Checksum(map) << endl;
    }

    cout << endl;
}