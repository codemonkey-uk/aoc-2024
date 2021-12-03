#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

//bool BeginsWith(const string& a, const char* b);

int data_width = 5;


// Each bit in the gamma rate can be determined by 
//   finding the most common bit in the corresponding position 
//   of all numbers in the diagnostic report. 
string Gamma(const vector< string >& data)
{
    // 01010
    int counts[data_width];
    for (int i=0;i!=data_width;++i)
    {
        counts[i]=0;
    }
    for (const string& line : data)
    {
        for (int i=0;i!=data_width;++i)
        {
            if (line[i]=='1')
                counts[i]++;
        }
    }
    string result;
    const int threshold = data.size()/2;
    for (int i=0;i!=data_width;++i)
    {
        char c = (counts[i]>threshold) ? '1' : '0';
        result += c;
    }
    return result;
}

string Epsilon(const string& gamma)
{
    string result;
    for (int i=0;i!=data_width;++i)
    {
        char c = (gamma[i]=='0') ? '1' : '0';
        result += c;
    }
    return result;
}

int BString2Int(const string& b)
{
    int result=0;
    for (int i=0;i!=data_width;++i)
    {
        result = (result << 1) + (b[i]=='1');
    }
    return result;
}

void Three_A()
{
    string line;
    vector< string > data;
    while(getline(cin,line))
    {
        data.push_back(line);
    }
    
    data_width = data.front().size();
    
    string gamma = Gamma(data);
    const int gamma_int = BString2Int(gamma);
    cout << "gamma = " << gamma << " = " << gamma_int << endl;
    string epsilon = Epsilon(gamma);
    const int epsilon_int = BString2Int(epsilon);
    cout << "epsilon = " << epsilon << " = " << BString2Int(epsilon) << endl;
    const int consumption = gamma_int*epsilon_int;
    cout << "consumption = " << consumption << endl;
}

void Three_B()
{
    
}