// ten.cpp

#include <iostream>
#include <vector>

using namespace std;

char match(char c)
{
    switch (c){
        case '(': return ')';
        case '[': return ']';
        case '<': return '>';
        case '{': return '}';
    }
    
    return 0;
}

size_t score(char c)
{
    switch (c){
        case ')': return 3;
        case ']': return 57;
        case '}': return 1197;
        case '>': return 25137;
    }
    return 0;
}

size_t score_2(char c)
{
    switch (c){
        case ')': return 1;
        case ']': return 2;
        case '}': return 3;
        case '>': return 4;
    }
    return 0;
}

void Ten()
{
    size_t sum = 0;
    string line;

    while(getline(cin,line))
    {
        vector<char> stack;
        for (size_t i=0;i!=line.size();++i)
        {
            if (stack.size()>0 && line[i]==match(stack.back())) {
                stack.pop_back();
            } 
            else if (match(line[i])) {
                stack.push_back(line[i]);
            }
            else {
                cout << line << "Expected '" << match(stack.back()) 
                     << "' but found '" << line[i]
                     << "' on column " << i << endl;
                sum+=score(line[i]);
                break;
            }
        }
    }
    cout << sum << endl;
}