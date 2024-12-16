#include <iostream>
#include <vector>
#include <cmath>

#include "util.h"

using namespace std;

string example = "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";

long long int DoThree(string in)
{
    long long int a=0, b=0, x=0;
    char m='m';

    size_t i=0;

    while (i<in.size())
    {
       switch (m) 
       {
        case 'm':
            if (in[i]=='m') m='u';
            break;
        case 'u':
            if (in[i]=='u') m='l';
            else m='m';
            break;
        case 'l':
            if (in[i]=='l') m='(';
            break;
        case '(':
            if (in[i]=='(') { m='a'; a=0; b=0; }
            break;
        case 'a':
            if (isdigit(in[i]) && a<=99) a=a*10 + (in[i]-'0');
            else if (in[i]==',') m='b';
            else m='m';
            break;
        case 'b':
            if (isdigit(in[i]) && b<=99) b=b*10 + (in[i]-'0');
            else if (in[i]==')') {
                assert(a>0 && a<1000);
                assert(b>0 && b<1000);
                cout << "mul("<<a<<','<<b<<')'<<endl;
                x += a*b;
                m='m';
            }
            else m='m';
            break;            
       }
       i++;
    }
    cout << "Total: " << x << endl;
    return x;
}

void Three()
{   
    // example WORKS
    DoThree(example);

    // part 1 result TO LARGE ?!?!
    long long int tx=0;
    std::string line; 
    while (std::getline(std::cin, line))
    {
        tx+=DoThree(line);
    }
    cout << "P1: " << tx << endl;
}

