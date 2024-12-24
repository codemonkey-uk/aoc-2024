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

static void DebugPrint(const vector<int>& content)
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

void Defrag_V2(vector<int>& memory)
{
    auto fileEnd = find_if(memory.rbegin(), memory.rend(), [](int i) { return i!=-1; });
    if (fileEnd==memory.rend()) return;
    int id=*fileEnd;

    while (id>0)
    {
        auto nextFree = find(memory.begin(), memory.end(), -1);
        auto fileBegin = find_if(fileEnd+1, memory.rend(), [id](int i) { return i!=id; });
        int fileLen = distance(fileEnd, fileBegin);
        //cout << id << " = " << fileLen << endl;
        if (fileLen>0)
        {
            auto freeEnd = find_if(nextFree, memory.end(), [](int i) { return i!=-1; });
            int freeSpace = distance(nextFree, freeEnd);
            //cout << " free " << freeSpace << endl;
            while(freeSpace<fileLen && nextFree!=memory.end())
            {
                nextFree = find(freeEnd, memory.end(), -1);
                freeEnd = find_if(nextFree, memory.end(), [](int i) { return i!=-1; });
                freeSpace = distance(nextFree, freeEnd);
                //cout << " free " << freeSpace << endl;
            }
            if(freeSpace>=fileLen)
            {
                if (nextFree<fileEnd.base())
                {
                    do
                    {
                        //swap(*fileBegin, *nextFree);
                        *fileEnd = -1;
                        *nextFree = id;
                        fileEnd++;
                        nextFree++;
                    }while(fileEnd!=fileBegin);
                }
            }
        }
        
        //DebugPrint(memory);
        id--;
        
        fileEnd = find_if(memory.rbegin(), memory.rend(), [id](int i) { return i==id; });
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
        cout << "P1: " << Checksum(map) << endl;

        map = Expand(dense);
        Defrag_V2(map);
        cout << "P2: " << Checksum(map) << endl;

    }

    cout << endl;
}