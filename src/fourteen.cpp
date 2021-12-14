// fourteen.cpp

#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <cassert>

using namespace std;

void ReportFreq(const map< char, size_t >& freq)
{
    size_t most_common = 0;
    size_t least_common = numeric_limits<size_t>::max();
    for (auto f : freq)
    {
        size_t i = f.second;
        cout << f.first << " : " << i << endl;
        if (i<least_common) least_common = i;
        if (i>most_common) most_common = i;
    }
    size_t diff = most_common-least_common;
    cout << most_common << " - " << least_common << " = " << diff << endl;
}

void Fourteen()
{
    string polymer_template;
    getline(cin, polymer_template);
    
    string line;
    getline(cin, line);
    assert (line.size()==0);
    
    map< pair<char,char>, char > insertions;
    
    while (getline(cin, line))
    {
        assert(line.size()==7);
        char a=line[0];
        char b=line[1];
        char c=line[6];
        insertions[{a,b}]=c;
    }
    
    int steps = 40;
    
    cout << "Template: " << polymer_template << endl;
    map< char, size_t > freq;
    for (unsigned char c : polymer_template)
        freq[c]++;
    
    map< pair<char,char>, size_t > pairs;
    for (size_t c=0;c!=polymer_template.size()-1;++c)
        pairs[{polymer_template[c], polymer_template[c+1]}]+=1;
    
    for (int i = 0;i!=steps;++i)
    {
        map< pair<char,char>, size_t > result;
        for (auto kvp : pairs)
        {
            auto itr = insertions.find( kvp.first );
            if (itr!=insertions.end())
            {
                pair<char, char> left = {kvp.first.first,itr->second};
                pair<char, char> right = {itr->second,kvp.first.second};
                result[left] += kvp.second;
                result[right] += kvp.second;
                freq[itr->second] += kvp.second;
            }
            else
            {
                result[kvp.first] += kvp.second;
            }
        }
        
        if (i==11) 
        {
            cout << "--- Part 1 (10 steps) ---" << endl;
            ReportFreq(freq);
        }
        
        pairs = result;
    }
    
    cout << "--- Part 2 (40 steps) ---" << endl;    
    ReportFreq(freq);
}