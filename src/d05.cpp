#include <iostream>
#include <map>
#include <set>

#include "util.h"

using namespace std;

bool CheckLine(const vector<int>& pages, map< int, set<int> >& rules)
{    
    for (size_t i=0;i!=pages.size(); ++i)
    {
        // a rule exists that P must come before pages[0]
        for (int p : rules[pages[i]])
        {
            // check P follows
            if (find(pages.begin()+i+1, pages.end(), p)!=pages.end()) 
            {
                cerr << "Percursor rule " << p << "|" << pages[0] << " is violated." << endl;
                return false;
            }
        }
    
    }

    return true;
}

bool SortLine(vector<int>& pages, map< int, set<int> >& rules)
{    
    for (size_t i=0;i!=pages.size(); ++i)
    {
        // a rule exists that P must come before pages[0]
        for (int p : rules[pages[i]])
        {
            // check P follows
            auto v = find(pages.begin()+i+1, pages.end(), p);
            if (v!=pages.end()) 
            {
                pages.erase(v);
                pages.insert(pages.begin()+i, p);
                i=-1;
                break;
            }
        }    
    }

    cout << "Fixed: ";
    for (int p : pages)
        cout << p << " ";
    cout << endl;

    return true;
}

void Five()
{   
    map< int, set<int> > rules;
    
    size_t sumTotal = 0, fixedTotal = 0;

    int input_phase = 0;

    int lc=0;
    string line; 
    while (std::getline(std::cin, line))
    {
        lc++;

        if (line.empty())
        {
            input_phase++;
        }
        else if (input_phase==0)
        {
            // reading rules
            auto pair = Split(line,'|');
            if (pair.size()!=2)
            {
                cerr << "Unexpected format on line " << lc << ": " << line << endl;
                continue;
            }
            
            int a = atoi(pair[0].c_str());
            int b = atoi(pair[1].c_str());

            // all the numbers that MUST come before B
            rules[b].insert(a);
        }
        else
        {
            // update phase
            
            auto parsedLine = Split(line,',');
            vector<int> pages;
            pages.reserve(parsedLine.size());
            for(const auto& s : parsedLine)
            {
                pages.push_back( atoi(s.c_str()) );
            }

            bool ok = CheckLine(pages, rules);
            cout << line << (ok ? " - Okay" : " - FAILED");
            if (ok)
            {
                int middle = pages[pages.size()/2];
                cout << " - middle page is " << middle;
                sumTotal += middle;
            }
            else 
            {
                cout << endl;
                SortLine(pages, rules);
                int middle = pages[pages.size()/2];
                cout << " - middle page is " << middle;
                fixedTotal += middle;
            }
            cout << endl;
        } 
    }


    cout << "P1: " << sumTotal << endl;
    cout << "P2: " << fixedTotal << endl;
}

