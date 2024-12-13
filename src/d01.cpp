#include <iostream>
#include <vector>

using namespace std;

void One()
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
