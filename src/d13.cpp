#include <iostream>

#include "util.h"

using namespace std;

typedef int64_t BigInt;

bool verbose=false;
bool part2=true;

struct Prize
{
    BigInt x;
    BigInt y;
};

struct Button
{
    BigInt dx;
    BigInt dy;
};

struct Machine
{
    Button a;
    Button b;
    Prize prize;
};

BigInt GreatestCommonDivisor(BigInt a, BigInt b)
{
    while (b != 0)
    {
        BigInt t = b;
        b = a % b;
        a = t;
    }
    return a;
}

struct ExtendedEuclideanResult
{
    BigInt gcd;
    BigInt x;
    BigInt y;
};

ExtendedEuclideanResult ExtendedEuclidean(BigInt a, BigInt b)
{
    if (b == 0)
    {
        return {a, 1, 0};
    }

    ExtendedEuclideanResult r = ExtendedEuclidean(b, a % b);
    return {r.gcd, r.y, r.x - (a / b) * r.y};
}

class Solution
{
    public:
    Solution(BigInt a_, BigInt b_, BigInt t)
    : a(a_), b(b_)  
    {
        r = ExtendedEuclidean(a, b);
        if (t % r.gcd != 0)
        {
            k_min = k_max = 0;
            return;
        }

        BigInt scale = t / r.gcd;
        ma = r.x * scale;
        mb = r.y * scale;

        // Calculate the range 
        k_min = std::ceil(-static_cast<double>(ma * r.gcd) / b);
        k_max = std::floor(static_cast<double>(mb * r.gcd) / a);        
    }

    struct iterator 
    {
        iterator(const Solution* s_, BigInt i_) : s(s_), i(i_) {}
        pair<BigInt,BigInt> operator*() const { return (*s)[i]; }
        iterator& operator++() { i++; return *this; }
        iterator operator++(int) { iterator copy(*this); i++; return copy; }
        bool operator==(const iterator& other) const { return i == other.i; }
        bool operator!=(const iterator& other) const { return i != other.i; }
        const Solution* s;
        BigInt i;
    };

    iterator begin() const { return iterator(this, k_min); }
    iterator end() const { return iterator(this, k_min+size()); }

    size_t size() const
    {
        return k_max - k_min + 1;
    }

    pair<BigInt,BigInt> operator[](size_t i) const
    {
        return {ma + i * (b / r.gcd), 
                mb - i * (a / r.gcd)};
    }

private:
    BigInt a;
    BigInt b;
    ExtendedEuclideanResult r;
    BigInt ma;
    BigInt mb;
    BigInt k_min;
    BigInt k_max;        

};

// returns ma,mb when solving ma*a + mb*b == t
Solution Solve(BigInt a, BigInt b, BigInt t)
{   
    return Solution(a, b, t);
}

BigInt Solve(Machine m)
{
    static BigInt N=0;
    cout << "-- Machine " << N++ << " --" << endl;
    cout << "Button A: X+" << m.a.dx << " Y+" << m.a.dy << endl;
    cout << "Button B: X+" << m.b.dx << " Y+" << m.b.dy << endl;
    cout << "Prize: X+" << m.prize.x << " Y+" << m.prize.y << endl;

    auto xsolutions = Solve(m.a.dx, m.b.dx, m.prize.x);
    auto ysolutions = Solve(m.a.dy, m.b.dy, m.prize.y);

    cout << xsolutions.size() << " solutions for x: " << endl;
    if (verbose)
    {
        for (auto x: xsolutions)
        {
            cout << " " << x.first << " x " << m.a.dx << " + ";
            cout << " " << x.second << " x " << m.b.dx << " = ";
            cout << " " << x.first * m.a.dx + x.second * m.b.dx;
            cout << " = " << m.prize.x << endl;
        }
    }
    
    cout << ysolutions.size() << " solutions for y: " << endl;
    if (verbose)
    {
        for (auto y: ysolutions)
        {
            cout << " " << y.first << " x " << m.a.dy << " + ";
            cout << " " << y.second << " x " << m.b.dy << " = ";
            cout << " " << y.first * m.a.dy + y.second * m.b.dy;
            cout << " = " << m.prize.y << endl;
        }
    }
    

    BigInt tokens=INT_MAX;
    pair<BigInt,BigInt> bestSolution;

    // find the intersection of the two sets of solutions  
    for (auto x : xsolutions)
    {
        for (auto y : ysolutions)
        {
            if (x.first == y.first && x.second == y.second)
            {
                BigInt cost = x.first*3 + x.second*1;
                if (cost < tokens)
                {
                    tokens = cost;
                    bestSolution = x;

                    cout << "Solution: " << x.first << " " << x.second << " cost " << tokens << endl;
                }
            }
            else if (x.first < y.first) break;
        }
    }

    return tokens;
}

void Thirteen()
{
    // Example 1
    /*
        Button A: X+94, Y+34
        Button B: X+22, Y+67
        Prize: X=8400, Y=5400

        Button A: X+26, Y+66
        Button B: X+67, Y+21
        Prize: X=12748, Y=12176

        Button A: X+17, Y+86
        Button B: X+84, Y+37
        Prize: X=7870, Y=6450

        Button A: X+69, Y+23
        Button B: X+27, Y+71
        Prize: X=18641, Y=10279    
    */

    // Parse input into machines
    vector<Machine> machines;

    string line;
    while (getline(cin, line))
    {
        // skip blanks
        if (line.empty()) getline(cin, line);

        int x, y;
        Machine m;

        sscanf(line.c_str(), "Button A: X+%d, Y+%d", &x, &y);
        m.a.dx=x;
        m.a.dy=y;
        
        getline(cin, line);
        sscanf(line.c_str(), "Button B: X+%d, Y+%d", &x, &y);
        m.b.dx=x;
        m.b.dy=y;        
        
        getline(cin, line);
        sscanf(line.c_str(), "Prize: X=%d, Y=%d", &x, &y);
        m.prize.x=x;
        m.prize.y=y;        

        machines.push_back(m);
    }
    cout << machines.size() << " machines to solve." << endl;

    BigInt sum=0;
    for (auto m : machines)
    {
        BigInt cost = Solve(m);
        if (cost != INT_MAX)
        {
            sum += cost;
        }
    }
    cout << "P1: " << sum << endl;

    // Part 2
    if (part2)
    {
        sum=0;
        for (auto m : machines)
        {
            m.prize.x += 10000000000000;
            m.prize.y += 10000000000000;

            BigInt cost = Solve(m);
            if (cost != INT_MAX)
            {
                sum += cost;
            }
        }
        cout << "P2: " << sum << endl;
    }   
}