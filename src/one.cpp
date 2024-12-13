#include <iostream>
#include <vector>

using namespace std;

void One_A()
{
    int ia,ib;
    vector<int> a,b;

    while(cin>>ia>>ib)
    {
        a.push_back(ia);
        b.push_back(ib);
    }

    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());

    int t = 0;
    int t2 = 0;
    for (size_t i=0;i!=a.size();++i)
    {
        // part 1
        int d = a[i]-b[i];
        if (d<0) d=-d;
        cout << a[i] << ' ' << b[i] << ' ' << d;
        t += d;
        
        // part 2
        int tx = 0;
        size_t j=0;
        while (b[j]<a[i]) j++;
        while (b[j]==a[i]) {                
            tx++;
            j++;
        }
        t2 += tx*a[i];
        cout << ' ' << tx << endl;

    }
    cout << "1) " << t << endl;
    cout << "2) " << t2 << endl;

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