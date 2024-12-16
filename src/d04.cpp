#include <iostream>
#include <vector>
#include <cmath>

#include "util.h"

using namespace std;

bool Match(const string& word, const vector<string>& in, size_t row, size_t column, int dr, int dc)
{
    size_t i = 0;

    while(word[i]==in[row][column])
    {
        i++;
        row += dr;
        column += dc;

        if (i==word.size()) return true;
        if (row==in.size()) return false;
        if (column==in.size()) return false;
    }

    return false;
}

size_t CountMatches(const string& word, const vector<string>& in, size_t row, size_t column)
{
    int c = 0;
    if (word[0]==in[row][column])
    {
        // four axis aligned
        c += Match(word, in, row, column,  1, 0);
        c += Match(word, in, row, column,  0, 1);
        c += Match(word, in, row, column, -1, 0);
        c += Match(word, in, row, column,  0,-1);

        // four diagnal
        c += Match(word, in, row, column,  1, 1);
        c += Match(word, in, row, column, -1, 1);
        c += Match(word, in, row, column,  1,-1);
        c += Match(word, in, row, column, -1,-1);  
    }

    return c;
}

size_t CountAllMatches(const string& word, const vector<string>& in)
{
    int total = 0;
    for (size_t row=0;row!=in.size();row++)
    {
        for (size_t column=0;column!=in.size();column++)
        {
            total += CountMatches(word, in, row, column);
        }
    }

    return total;
}

void Four()
{   
    vector<string> grid;
    
    string line; 
    while (std::getline(std::cin, line))
    {
        grid.push_back(line);
    }


    cout << "P1: " << CountAllMatches("XMAS", grid) << endl;
}

