#include <iostream>
#include <vector>
#include <cmath>

#include "util.h"

using namespace std;

int D(const string& a, const string& b)
{
    return atoi(a.c_str()) - atoi(b.c_str());
}

bool Safe(const vector< string >& report, int *a=nullptr, int *b=nullptr)
{
    const int d = D(report[0], report[1]);

    for (size_t i=1;i!=report.size();++i)
    {
        int d2 = D(report[i-1], report[i]);
        if (d2 == 0 || d2>3 || d2<-3 || signbit(d)!=signbit(d2))
        {
            if (a) *a = i-1;
            if (b) *b = i;

            std::cout << "Unsafe @" << i << ": " << Join(report) << " [" << report[i-1] << " " << report[i] << "] " << endl;            

            return false;
        }
    }

    return true;
}

void Two()
{
    vector<int> a,b;
    int safeC = 0;
    int safeD = 0;

    for (std::string line; std::getline(std::cin, line);) 
    {
        vector< string > report = Split(line, ' ');
        
        int a, b;
        bool safe = Safe(report, &a, &b);
        if (!safe)
        {
            auto ra = report;
            ra.erase(ra.begin()+a);            

            bool safeA = Safe(ra);
            if (safeA)
            {
                std::cout << "\tSafe (a): " << line << " removed: " << report[a] << endl;
                safeD++;
                continue;
            }

            auto rb = report;
            rb.erase(rb.begin()+b);

            bool safeB = Safe(rb);
            if (safeB)
            {
                std::cout << "\tSafe (b): " << line << " removed: " << report[b] << endl;
                safeD++;
                continue;
            }

            if (a>0)
            {
                auto rx = report;
                rx.erase(rx.begin());

                bool safeX = Safe(rx);
                if (safeX)
                {
                    std::cout << "\tSafe (x): " << line << " removed: " << report[0] << endl;
                    safeD++;
                    continue;
                }
            }

        }
        else
        {
            safeC ++;
            std::cout << "Safe: " << line << endl;
        }
    }

    cout << "1) " << safeC << endl;
    cout << "2) " << safeC + safeD << endl;
}
