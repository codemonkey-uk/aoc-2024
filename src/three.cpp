#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cassert>

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

char oxygen_criteria(const vector< string >& data, int pos)
{
    int ones = 0;
    for (const string& line : data)
    {
        ones += (line[pos]=='1');
    }

    const int zeros = data.size()-ones;
    if (ones >= zeros) return '1';
    else return '0';
}

char scrubber_criteria(const vector< string >& data, int pos)
{
    return oxygen_criteria(data,pos)=='1' ? '0' : '1';
}

string rating( 
    vector< string > data, 
    char(*fn)(const vector< string >&, int) 
)
{
    for (int i=0;i!=data_width;++i)
    {
        char m = fn(data,i);
        
        auto e = remove_if(
            data.begin(),data.end(),
            [i,m](const auto& line){
                return line[i]!=m;
            }
        );
        data.erase(e,data.end());

        if (data.size()==1)
            break;
    }
    assert(data.size()==1);
    return data.front();
}

void Three()
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
    
    string ox = rating( data, oxygen_criteria );
    const int ox_int = BString2Int(ox);
    cout << ox << " = " << ox_int << endl;
    string scrub = rating( data, scrubber_criteria );
    const int scrub_int = BString2Int(scrub);
    cout << scrub << " = " << scrub_int << endl;
    
    const int rating = ox_int*scrub_int;
    cout << "rating = " << rating << endl;
}
