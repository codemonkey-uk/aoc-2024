#include <iostream>
#include <numeric>

#include "util.h"

using namespace std;

typedef int64_t BigInt;

bool verbose=true;
bool part2=false;

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

// Chinese Remainder Theorem
std::pair<BigInt, BigInt> CRT(BigInt r1, BigInt m1, BigInt r2, BigInt m2)
{
    // Step 1: Compute GCD and coefficients
    ExtendedEuclideanResult e = ExtendedEuclidean(m1, m2);
    BigInt d = e.gcd;

    // Step 2: Check for solvability
    if ((r2 - r1) % d != 0)
    {
        throw std::runtime_error("No solution exists for the given congruences");
    }

    // Step 3: Compute the solution
    BigInt lcm = (m1 / d) * m2; // Least common multiple of m1 and m2
    BigInt offset = ((r2 - r1) / d) * e.x % (m2 / d);
    BigInt x = (r1 + offset * m1) % lcm;

    // Ensure x is non-negative
    if (x < 0) x += lcm;

    return {x, lcm};
}

class Solution
{
    public:
    Solution(BigInt a_, BigInt b_, BigInt t)
    : a(a_)
    , b(b_)
    , r(ExtendedEuclidean(a_, b_))
    {
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
        cout << "k_min " << k_min << " k_max " << k_max << endl;
        assert(k_min <= k_max);
    }

    int Scale() const
    {
        return ma/r.x;
    }
    int T() const
    {   
        return r.gcd * Scale();
    }

    // create a combined solutiom
    Solution(const Solution& s1, const Solution& s2)
    : a(gcd(s1.a, s2.a))
    , b(gcd(s1.b, s2.b))
    , r(ExtendedEuclidean(a,b))
    {
        // Step 1: Calculate steps for both solutions
        BigInt step1 = s1.b / s1.r.gcd;
        BigInt step2 = s2.b / s2.r.gcd;

        // Step 2: Align ma using modular arithmetic
        BigInt mod1 = step1;
        BigInt mod2 = step2;
        BigInt res1 = (s1.ma % mod1 + mod1) % mod1; // Normalize to [0, mod1)
        BigInt res2 = (s2.ma % mod2 + mod2) % mod2; // Normalize to [0, mod2)

        // Step 3: Solve for combined k using CRT
        auto crt = CRT(res1, mod1, res2, mod2);
        BigInt combined_k = crt.first;   // Starting k value aligned to both solutions
        BigInt combined_step = crt.second; // Step size of the combined solution

        // Step 4: Calculate combined ma and mb
        ma = s1.ma + combined_k * step1;
        mb = s1.mb - combined_k * (s1.a / s1.r.gcd);

        // Step 5: Align k ranges
        // Translate s1's k range to the combined system
        BigInt s1_k_min_aligned = (s1.k_min * step1 - combined_k) / combined_step;
        BigInt s1_k_max_aligned = (s1.k_max * step1 - combined_k) / combined_step;

        // Translate s2's k range to the combined system
        BigInt s2_k_min_aligned = (s2.k_min * step2 - combined_k) / combined_step;
        BigInt s2_k_max_aligned = (s2.k_max * step2 - combined_k) / combined_step;

        // Compute the intersection of the ranges
        k_min = std::max(s1_k_min_aligned, s2_k_min_aligned);
        k_max = std::min(s1_k_max_aligned, s2_k_max_aligned);

        // Step 6: Sanity check for valid range
        if (k_min > k_max) {
            std::cerr << "No valid solutions exist in the combined range.\n";
            k_min = k_max = 0; // Mark as invalid
        }
    }

    struct iterator 
    {
        using value_type = std::pair<BigInt, BigInt>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::forward_iterator_tag;

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

    const BigInt a;
    const BigInt b;
    const ExtendedEuclideanResult r;

private:
    BigInt ma;
    BigInt mb;
    BigInt k_min;
    BigInt k_max;        

};

// Define the comparison operator for std::pair<BigInt, BigInt>
bool operator<(const std::pair<BigInt, BigInt>& lhs, const std::pair<BigInt, BigInt>& rhs) {
    return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
}   

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
    auto combo = Solution(xsolutions, ysolutions);

    cout << xsolutions.size() << " solutions for x with gcd = ";
    cout << xsolutions.r.gcd << "*" << xsolutions.Scale() << endl;
    if (verbose && !part2)
    {
        for (auto x: xsolutions)
        {
            cout << " " << x.first << " x " << m.a.dx << " + ";
            cout << " " << x.second << " x " << m.b.dx << " = ";
            cout << " " << x.first * m.a.dx + x.second * m.b.dx;
            cout << " = " << m.prize.x << endl;
        }
    }
    
    cout << ysolutions.size() << " solutions for y with gcd = ";
    cout << ysolutions.r.gcd << "*" << ysolutions.Scale() << endl;
    if (verbose && !part2)
    {
        for (auto y: ysolutions)
        {
            cout << " " << y.first << " x " << m.a.dy << " + ";
            cout << " " << y.second << " x " << m.b.dy << " = ";
            cout << " " << y.first * m.a.dy + y.second * m.b.dy;
            cout << " = " << m.prize.y << endl;
        }
    }

    cout << combo.size() << " shared solutions: with gcd = ";
    cout << combo.r.gcd << "*" << combo.Scale() << endl;
    if (verbose && !part2)
    {
        int max = 5;
        for (auto c: combo)
        {
            cout << " " << c.first << " x " << m.a.dx << " + ";
            cout << " " << c.second << " x " << m.b.dx << " = ";
            cout << " " << c.first * m.a.dx + c.second * m.b.dx;
            cout << " = " << m.prize.x << endl;

            cout << " " << c.first << " x " << m.a.dy << " + ";
            cout << " " << c.second << " x " << m.b.dy << " = ";
            cout << " " << c.first * m.a.dy + c.second * m.b.dy;
            cout << " = " << m.prize.y << endl;
            if (max-- == 0) { cout << "..." << endl; break; }
        }
    }

    if (xsolutions.size() && ysolutions.size())
    {
        if (verbose) cout << "looking for " << (*ysolutions.begin()).first << " " << (*ysolutions.begin()).second << " in xsolutions" << endl;
        auto yitr = lower_bound(ysolutions.begin(), ysolutions.end(), *xsolutions.begin());
        auto xitr = lower_bound(xsolutions.begin(), xsolutions.end(), *ysolutions.begin());
        for (;xitr!=xsolutions.end();)
        {
            auto x = *xitr;

            if (verbose) cout << "looking for " << x.first << " " << x.second << " in ysolutions" << endl;
            yitr = lower_bound(yitr, ysolutions.end(), x);

            if (yitr==ysolutions.end()) break;

            auto y = *yitr;
            if (x.first == y.first && x.second == y.second)
            {
                BigInt cost = x.first*3 + x.second*1;
                // because solitions are in sorted order, assending, we can break here
                cout << "Solution: " << x.first << " " << x.second << " cost " << cost << endl;
                return cost;
            }
            
            if (verbose) cout << "looking for " << y.first << " " << y.second << " in xsolutions" << endl;
            xitr = lower_bound(xitr, xsolutions.end(), y);
        }
    }
    
    cout << "No solution found." << endl;
    return INT_MAX;
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