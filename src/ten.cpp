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
    vector< size_t > points;
    while(getline(cin,line))
    {
        // part 1
        bool syntax_error=false;
        vector<char> stack;
        for (size_t i=0;i!=line.size();++i)
        {
            if (stack.empty()==false && line[i]==match(stack.back())) {
                stack.pop_back();
            } 
            else if (match(line[i])) {
                stack.push_back(line[i]);
            }
            else {
                cout << line << " Expected '" << match(stack.back()) 
                     << "' but found '" << line[i]
                     << "' on column " << i << endl;
                sum+=score(line[i]);
                syntax_error=true;
                break;
            }
        }
        // part 2
        if (stack.empty()==false && syntax_error==false)
        {
            size_t completion_score = 0;
            while(stack.empty()==false)
            {
                completion_score *= 5;
                char m = match(stack.back());
                completion_score += score_2(m);
                cout << m;
                stack.pop_back();
            }
            cout << " - " << completion_score << " total points." << endl;
            points.push_back( completion_score );
        }
    }
    cout << "part 1: " << sum << endl;
    
    sort(points.rbegin(),points.rend());
    cout << "part 2: " << points[points.size()/2] << endl;
}