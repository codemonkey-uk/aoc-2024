#include <iostream>
#include <vector>

using namespace std;

void One_A()
{
    int a,b,c=0;
    cin >> a;
    while(cin>>b)
    {
        if (b>a) c++;
        a = b;
    }
    cout << c << endl;
}

void One_B()
{
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
}