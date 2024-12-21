#include <iostream>

#include "util.h"

using namespace std;

struct Prize
{
    int x;
    int y;
};

struct Button
{
    int dx;
    int dy;
};

struct Machine
{
    Button a;
    Button b;
    Prize prize;
};

int GreatestCommonDivisor(int a, int b)
{
    while (b != 0)
    {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

struct ExtendedEuclideanResult
{
    int gcd;
    int x;
    int y;
};

ExtendedEuclideanResult ExtendedEuclidean(int a, int b)
{
    if (b == 0)
    {
        return {a, 1, 0};
    }

    ExtendedEuclideanResult r = ExtendedEuclidean(b, a % b);
    return {r.gcd, r.y, r.x - (a / b) * r.y};
}

// returns ma,mb when solving ma*a + mb*b == t
vector< pair<int,int> > Solve(int a, int b, int t)
{
    ExtendedEuclideanResult r = ExtendedEuclidean(a, b);
    if (t % r.gcd != 0)
    {
        return {};
    }

    int scale = t / r.gcd;
    int ma = r.x * scale;
    int mb = r.y * scale;
    vector< pair<int,int> > result;

    // Calculate the range of k
    int k_min = std::ceil(-static_cast<double>(ma * r.gcd) / b);
    int k_max = std::floor(static_cast<double>(mb * r.gcd) / a);

    for (int i = k_min; i <= k_max; i++)
    {
        result.push_back({
            ma + i * (b / r.gcd), 
            mb - i * (a / r.gcd)
        });
    }
    
    return result;
}

int Solve(Machine m)
{
    static int N=0;
    cout << "-- Machine " << N++ << " --" << endl;
    cout << "Button A: X+" << m.a.dx << " Y+" << m.a.dy << endl;
    cout << "Button B: X+" << m.b.dx << " Y+" << m.b.dy << endl;
    cout << "Prize: X+" << m.prize.x << " Y+" << m.prize.y << endl;

    auto xsolutions = Solve(m.a.dx, m.b.dx, m.prize.x);
    auto ysolutions = Solve(m.a.dy, m.b.dy, m.prize.y);

    cout << xsolutions.size() << " solutions for x: " << endl;
    for (auto x: xsolutions)
    {
        cout << " " << x.first << " x " << m.a.dx << " + ";
        cout << " " << x.second << " x " << m.b.dx << " = ";
        cout << " " << x.first * m.a.dx + x.second * m.b.dx;
        cout << " = " << m.prize.x << endl;
    }
    cout << ysolutions.size() << " solutions for y: " << endl;
    for (auto y: ysolutions)
    {
        cout << " " << y.first << " x " << m.a.dy << " + ";
        cout << " " << y.second << " x " << m.b.dy << " = ";
        cout << " " << y.first * m.a.dy + y.second * m.b.dy;
        cout << " = " << m.prize.y << endl;
    }

    vector< pair<int,int> > solutions;

    int tokens=INT_MAX;
    pair<int,int> bestSolution;

    // find the intersection of the two sets of solutions  
    for (auto x : xsolutions)
    {
        for (auto y : ysolutions)
        {
            if (x.first == y.first && x.second == y.second)
            {
                solutions.push_back(x);
                
                int cost = x.first*3 + x.second*1;
                if (cost < tokens)
                {
                    tokens = cost;
                    bestSolution = x;

                    cout << "Solution: " << x.first << " " << x.second << " cost " << tokens << endl;
                }
            }
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

        Machine m;
        sscanf(line.c_str(), "Button A: X+%d, Y+%d", &m.a.dx, &m.a.dy);
        
        getline(cin, line);
        sscanf(line.c_str(), "Button B: X+%d, Y+%d", &m.b.dx, &m.b.dy);
        
        getline(cin, line);
        sscanf(line.c_str(), "Prize: X=%d, Y=%d", &m.prize.x, &m.prize.y);

        machines.push_back(m);
    }
    cout << machines.size() << " machines to solve." << endl;

    int sum=0;
    for (auto m : machines)
    {
        int cost = Solve(m);
        if (cost != INT_MAX)
        {
            sum += cost;
        }
    }
    cout << "P1: " << sum << endl;
}