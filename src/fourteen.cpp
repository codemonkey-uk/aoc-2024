// fourteen.cpp

#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <cassert>

using namespace std;

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
    
    int steps = 10;
    
    cout << "Template: " << polymer_template << endl;
    for (int i = 0;i!=steps;++i)
    {
        string result;
        result.reserve( polymer_template.size()*2 );
        for (size_t c=0;c!=polymer_template.size()-1;++c)
        {
            result.push_back(polymer_template[c]);
            auto itr = insertions.find(
                {polymer_template[c], polymer_template[c+1]}
            );
            if (itr!=insertions.end())
                result.push_back(itr->second);
        }
        result.push_back(polymer_template.back());
        
        if (i<4)
        {
            cout << "After step " << i+1 << ": " << result << endl;
        }
        if (i==4 || i==9)
        {
            cout << "After step " << i+1 << " length = " << result.size() << endl;            
        }
        
        polymer_template = result;
    }
    
    int freq[256]={0};
    for (unsigned char c : polymer_template)
        freq[c]++;

    int most_common = 0;
    int least_common = numeric_limits<int>::max();
    for (int c =0;c!=256;++c )
    {
        int i = freq[c];
        if (i>0)
        {
            cout << (char)c << " : " << i << endl;
            if (i<least_common) least_common = i;
            if (i>most_common) most_common = i;
        }
    }
    int diff = most_common-least_common;
    cout << most_common << " - " << least_common << " = " << diff << endl;
    
}