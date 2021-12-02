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
    /*
    int a=0;
    std::vector<int> data;
    data.reserve( 2048 );

    while(cin>>a)
    {
        data.push_back(a);
    }

    int c = 0;
    for (size_t i = 3;i<data.size();++i)
    {
        int ps = data[i-1] + data[i-2] + data[i-3];        
        int is = data[i-0] + data[i-1] + data[i-2];        
        if (is>ps)
            c++;
    }
    cout << c << endl;
    */
}