#include <iostream>
#include <deque>

#include "util.h"

using namespace std;

typedef int64_t BigInt;

// DFS recursive solution
bool FindSolution(BigInt target, const vector<BigInt>& operands, BigInt subTotal, BigInt operandIndex, deque<char>& solutionOut)
{
    // all operands have been consumed, if subTotal matches Target we have found a solution
    if (operandIndex>=operands.size()) return subTotal == target;

    // if using + next is the solution
    if (FindSolution(target, operands, subTotal+operands[operandIndex], operandIndex+1, solutionOut)) 
    {
        solutionOut.push_front('+');
        return true;
    }

    // if using * next is the solution
    if (FindSolution(target, operands, subTotal*operands[operandIndex], operandIndex+1, solutionOut)) 
    {
        solutionOut.push_front('*');
        return true;
    }
    return false;
}

bool FindSolution(int target, const vector<BigInt>& operands, deque<char>& solutionOut)
{
    BigInt subTotal = operands[0];
    BigInt operandIndex = 1;
    return FindSolution(target, operands, subTotal, operandIndex, solutionOut);
}

void Seven()
{   
    string line;
    BigInt total=0;

    while (std::getline(std::cin, line))
    {
        vector<string> tokens = Split(line, ' ');
        BigInt target = atoll(tokens.front().c_str());
        vector<BigInt> operands;
        operands.reserve(tokens.size()-1);
        for(size_t o=1;o<tokens.size();++o)
        {
            operands.push_back(atoll(tokens[o].c_str()));
        }
        deque<char> solutionOut;
        if (FindSolution(target, operands, solutionOut))
        {
            int c=0;
            cout << target << ": ";
            for (BigInt i : operands)
                cout << i << solutionOut[c++];
            cout << endl;

            BigInt subTotal = total + target;
            //cerr << total << " + " << target << " = " << subTotal << endl;
            if (subTotal < target) cerr << "OVERFLOW DETECTED" << endl;
            total = subTotal;
        }
        else
        {
            cout << line << " UNSOLVED " << endl;
            //cout << target << ": ";
            //for (BigInt i : operands)
            //    cout << i << " ";
            //cout << endl;
        }
    }

    cout << "P1: " << total << endl;
    if (total<=17894274561) cerr << "KNOWN TOO SMALL" << endl;
    //         17394003452
}

