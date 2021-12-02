#include <iostream>
#include <cstring>

using namespace std;

bool BeginsWith(const string& a, const char* b)
{
    return (a.rfind(b,0)==0);
}

void Two_A()
{
    string line;

    //Your horizontal position and depth both start at 0.
    int horizontal_position=0;
    int depth=0;

    const int fl = strlen("forward");
    const int dl = strlen("down");
    const int ul = strlen("ul");

    while(getline(cin,line))
    {
        if (BeginsWith(line,"forward"))
            horizontal_position += atoi(line.c_str()+fl);
        else if (BeginsWith(line,"down"))
            depth += atoi(line.c_str()+dl);
        else if (BeginsWith(line,"up"))
            depth -= atoi(line.c_str()+ul);
    }
    cout << horizontal_position 
         << " * " 
         << depth 
         << " = "
         << horizontal_position*depth
         << endl;
}

void Two_B()
{
    string line;

    //Your horizontal position and depth both start at 0.
    int horizontal_position=0;
    int depth=0;
    int aim=0;

    const int fl = strlen("forward");
    const int dl = strlen("down");
    const int ul = strlen("ul");

    while(getline(cin,line))
    {
        if (BeginsWith(line,"forward"))
        {
            int n = atoi(line.c_str()+fl);
            horizontal_position += n;
            depth += aim*n;
        }
        else if (BeginsWith(line,"down"))
        {
            aim += atoi(line.c_str()+dl);
        }
        else if (BeginsWith(line,"up"))
        {
            aim -= atoi(line.c_str()+ul);
        }
    }
    cout << horizontal_position 
         << " * " 
         << depth 
         << " = "
         << horizontal_position*depth
         << endl;
}