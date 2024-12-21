#include <iostream>
#include <map>
#include <iterator>

#include "util.h"

using namespace std;

typedef uint64_t BigInt;

constexpr int Digits(BigInt num, int base=10) 
{
    BigInt result = 1;
    while(num/=base) result++;
    return result;
}

constexpr bool Odd(BigInt i)
{
    return i&1;
}

constexpr bool Even(BigInt i)
{
    return !Odd(i);
}

pair<BigInt,BigInt> Split(BigInt i)
{
    std::string s = std::to_string(i);
    auto mid = s.begin() + (s.size()/2);
    string l(s.begin(), mid);
    string r(mid, s.end());

    pair<BigInt,BigInt> result;
    result.first = atoll(l.c_str());
    result.second = atoll(r.c_str());

    return result;
}

map<BigInt, BigInt> Blink(const map<BigInt, BigInt>& in)
{
    map<BigInt, BigInt> result;
    for (auto m : in)
    {
        //If the stone is engraved with the number 0, it is replaced by a stone engraved with the number 1.
        if (m.first==0) 
        {
            result[1]+=m.second;
        }
        //If the stone is engraved with a number that has an even number of digits, it is replaced by two stones. 
        else if (Even(Digits(m.first)))
        {
            // The left half of the digits are engraved on the new left stone, and the right half of the digits are engraved on the new right stone. 
            // (The new numbers don't keep extra leading zeroes: 1000 would become stones 10 and 0.)
            auto split = Split(m.first);
            result[split.first] += m.second;
            result[split.second] += m.second;
        }
        else
        //If none of the other rules apply, the stone is replaced by a new stone; the old stone's number multiplied by 2024 is engraved on the new stone.
        {
            result[m.first*2024]+=m.second;
        }
    }
    return result;
}

void Eleven()
{
    //string line= "125 17";
    string line ="8435 234 928434 14 0 7 92446 8992692";

    //if (!std::getline(std::cin, line)) return;

    map<BigInt, BigInt> m;

    for(const auto &n : Split(line,' '))
    {
        m[atoll(n.c_str())]++;
    }

    //cout << Split(1000).first << ":" <<Split(1000).second<< endl;

    int blink=1;
    for (; blink<=75; blink++)
    {
        m = Blink(m);
    }

    BigInt sum = 0;
    for (auto p : m)
    {
        sum += p.second;
    }

    cout << "after " << blink << ", " << sum << " stones " << endl;

    // 192238 is to high
    // 198097 is to high
    // 198097

}