// twentyfour.cpp
// ALU

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <map>
#include <stdlib.h>

#include <atomic>
#include <thread>

#include "util.h"

using namespace std;
typedef int Integer;

struct Registers
{
    Integer d[4];
    
    Integer& operator[] (int i) {return d[i];}
    const Integer& operator[] (int i) const {return d[i];}
    bool operator<(Registers rhs)const{
        for (int i=0;i!=4;++i)
            if (d[i]<rhs.d[i]) return true;
            else if (d[i]>rhs.d[i]) return false;
        return false;
    };
    
    bool operator==(Registers rhs)const{
        for (int i=0;i!=4;++i)
            if (d[i]!=rhs.d[i]) return false;
        return true;
    };

    int get(char c)const{return d[c-'w'];}
    int& get(char c){return d[c-'w'];}
    size_t hash() const { 
        size_t result = d[0];
        for (int i=1;i!=4;++i)
            result = result * 7919 + d[i];
        return result;
    }
};

enum Op : short {
    inp,
    add,mul,div,mod,eql,
    addc,mulc,divc,modc,eqlc,
    nop
};

struct Instruction
{
    Op op;
    short a;
    short b;
    
    bool operator==(Instruction rhs)const{
        return op==rhs.op && a==rhs.a && b==rhs.b;
    }
};

typedef vector< Instruction > Program;

typedef vector< pair<int,int> > Input;

vector<int> current;
struct ScopePush
{
    vector<int>* t;
    ScopePush( vector<int>* target, int value ) : t(target)
    {
        t->push_back(value);
    }
    ~ScopePush() { t->pop_back(); }
};

char last_report = 0;
int report_f = 2;
bool Emit(Registers r)
{
    if (r.get('z')==0){
        cout << "VALID: ";
        for (auto c:current) cout<<c;
        cout << endl;
        return true;
    }
    else
    {
        if (current.size()==14 && current[report_f]!=last_report)
        {
            last_report=current[report_f];
            cout << "Progress: ";
            for (auto c:current) cout<<c;
                cout << endl;
        }
    }
    
    return false;
};

struct CachedState
{
    CachedState(const Registers& rx)
        : r(rx.get('w'), rx.get('z'))
    { }
    
    // just w & z
    pair<Integer,Integer> r;
    
    size_t hash() const { 
        return r.first * 7919 + r.second;
    }
    bool operator==(const CachedState& rhs) const{
        return r==rhs.r;
    }
};

namespace std
{
    template<>
    struct hash< CachedState >
    {
        std::size_t operator()(CachedState const& state) const noexcept
        {
            return state.hash();
        }
    };
}


unordered_map< CachedState,int > known_z[14];

size_t hits=0;
size_t misses=0;

Registers Run(
    Registers r,
    Program::const_iterator pi, 
    Program::const_iterator pe, 
    Input::const_iterator ii,
    Input::const_iterator ie
)
{
    static int depth=0;
    for(;pi!=pe;)
    {
        auto i = *pi++;

        switch (i.op)
        {
            case inp:
            // let me run on limited input set for a partial result
            if (ii==ie) return r;

            // inp a - Read an input value and write it to variable a.
            {
                Registers rx;
                auto range = *ii++;
                int dv = range.first < range.second ? 1 : -1;
                Integer min_z = numeric_limits<Integer>::max();
                for (int v=range.first;v!=range.second+dv;v+=dv)
                {
                    ScopePush p(&current, v);
                    r[i.a] = v;
                    
                    auto cached = known_z[depth].find(r);
                    if (cached!=known_z[depth].end()) 
                    {
                        hits++;
                        rx.get('z')=cached->second;
                    }
                    else
                    {
                        misses++;
                        depth++;
                        rx = Run(r,pi,pe,ii,ie);
                        depth--;
                        min_z = min(min_z,rx.get('z'));
                        known_z[depth].insert(cached, {r,min_z});
                    }
                    if (Emit(rx)) return rx;
                }
                return rx;
            }
            
            case add:
            // add a b - Add the value of a to the value of b, then store the result in variable a.
            r[i.a]+=r[i.b];
            break;
            case addc:
            // add a b - Add the value of a to the value of b, then store the result in variable a.
            r[i.a]+=i.b;
            break;
            
            case mul:
            // mul a b - Multiply the value of a by the value of b, then store the result in variable a.
            r[i.a]*=r[i.b];
            break;
            case mulc:
            // mul a b - Multiply the value of a by the value of b, then store the result in variable a.
            r[i.a]*=i.b;
            break;
            
            case Op::div:
            // div a b - Divide the value of a by the value of b, truncate the result to an integer, then store the result in variable a. (Here, "truncate" means to round the value toward zero.)
            r[i.a]/=r[i.b];
            break;
            case divc:
            // div a b - Divide the value of a by the value of b, truncate the result to an integer, then store the result in variable a. (Here, "truncate" means to round the value toward zero.)
            r[i.a]/=i.b;
            break;
            
            case mod:
            // mod a b - Divide the value of a by the value of b, then store the remainder in variable a. (This is also called the modulo operation.)
            r[i.a]%=r[i.b];
            break;
            case modc:
            // mod a b - Divide the value of a by the value of b, then store the remainder in variable a. (This is also called the modulo operation.)
            r[i.a]%=i.b;
            break;
            
            case eql:
            // eql a b - If the value of a and b are equal, then store the value 1 in variable a. Otherwise, store the value 0 in variable a.    
            r[i.a] = (r[i.a]==r[i.b]) ? 1 : 0;
            break;
            case eqlc:
            // eql a b - If the value of a and b are equal, then store the value 1 in variable a. Otherwise, store the value 0 in variable a.    
            r[i.a] = (r[i.a]==i.b) ? 1 : 0;
            break;
            
            case nop:
            break;

        }
    }
    
    return r;
}

Registers Run(const Program& p, const Input& input)
{
    Registers r{0};
    return Run( 
        r, 
        p.begin(), p.end(), 
        input.begin(), input.end()
    );
}

Registers Run(const Program& p, const vector<int>& input)
{
    Registers r{0};
    Input ix;
    ix.reserve(input.size());
    for(auto i:input) ix.push_back({i,i});
    
    return Run( 
        r, 
        p.begin(), p.end(), 
        ix.begin(), ix.end()
    );
}

static const map<string,Op> register_ops = 
{
    {"add",add},
    {"mul",mul},
    {"div",Op::div},
    {"mod",mod},
    {"eql",eql}
};

static const map<string,Op> const_ops = 
{
    {"add",addc},
    {"mul",mulc},
    {"div",divc},
    {"mod",modc},
    {"eql",eqlc}
};

static const map<Op,Op> to_const_op = 
{
    {add,addc},
    {mul,mulc},
    {Op::div,divc},
    {mod,modc},
    {eql,eqlc}
};

int RegisterIndex(char c)
{
    assert( c>='w' && c<='z' );
    return c-'w';
}

Instruction Compile(const string& line)
{
    Instruction result{inp,0,0};
    auto s = Split(line, ' ');

    // let me add blank lines and comment stuff out
    if (s.size() == 0 || (s[0].size()>3 && s[0].front()=='#')) 
        return {nop,0,0};

    if (s.size()==2)
    {
        assert(s[0]=="inp");
        assert(s[1].size()==1);
        result.a = RegisterIndex( s[1].front() );
    }
    else
    {
        assert(s.size()==3);
        assert(s[0].size()==3);
        assert(s[1].size()==1);
        result.a = RegisterIndex( s[1].front() );
        
        bool optype_const = false;
        if (s[2].front()>='w' && s[2].front()<='z')
        {
            result.b = RegisterIndex( s[2].front() );
        }
        else
        {
            result.b = atoi(s[2].c_str());
            optype_const=true;
        }
        
        const auto& lookup = (optype_const) ? const_ops : register_ops;
        const auto itr = lookup.find(s[0]);
        if (itr==lookup.end())
        {
            cerr << "Invalid opcode: '"<<s[0]<<"' in: " << line << endl;
            return result;
        }
        
        result.op = itr->second;
    }
    
    return result;
}

void FoldConstants(Program& in)
{
    Registers r{0,0,0,0};
    Registers c{1,1,1,1};

    unsigned int idx = 0;
    while(idx<in.size())
    {
        char flag = ' ';
        
        Registers ro = r;
        
        auto& i = in[idx++];
        switch (i.op)
        {
            // reads overwrite a register
            case inp:
                // input marks register as non-constant
                r[i.a]=1;
                c[i.a]=0;
                break;
            
            //
            // two register ops, target is constant if both inputs are constant
            // 
            
            case eql:
                r[i.a] = (r[i.a]==r[i.b]) ? 1 : 0;
                c[i.a] = (c[i.a] && c[i.b]) || (i.a==i.b);
            break;
            case add:
                r[i.a]+=r[i.b];
                c[i.a] = c[i.a] && c[i.b];
            break;
            case mul:
                r[i.a]*=r[i.b];
                c[i.a] = c[i.b] && (c[i.a] || r[i.b]==0);
            break;
            case Op::div:
                r[i.a]/=r[i.b];
                c[i.a] = c[i.a] && c[i.b];
            break;
            case mod:
                r[i.a]%=r[i.b];
                c[i.a] = c[i.a] && c[i.b];
            break;
            
            //
            // constant (one register ops), 
            // target is constant if input is constant
            // 
            
            case eqlc:
                r[i.a] = (r[i.a]==i.b) ? 1 : 0;
            break;
            case addc:
                r[i.a]+=i.b;
            break;
            case mulc:
                r[i.a]*=i.b;
                // multiply by zero resets to zero, constant
                if (i.b==0) c[i.a]=1;
            break;
            case Op::divc:
                r[i.a]/=i.b;
            break;
            case modc:
                r[i.a]%=i.b;
            break;

            case nop:
            break;
        }
        
        switch (i.op)
        {
            // equal to self, and divide by self,
            // both reset A to a constant value (1)
            case Op::div:
            case eql:
                if (i.a==i.b)
                {
                    c[i.a]=1;
                    r[i.a]=1;
                    break;
                }
            
            case mul:
            case add:
            case mod:
                // B is known constant, covert op to constant 
                if (c[i.b])
                {
                    auto conversion = to_const_op.find(i.op);
                    if (conversion!=to_const_op.end())
                    {
                        flag = 'c';
                        i.op = conversion->second;
                        i.b = c[i.b];
                    }
                    
                    // no break;
                    // fall through to the no-changes test for constants
                }
            
            case mulc:
            case addc:
            case Op::divc:
            case modc:
            case eqlc:
                // constant tracing means we know when an instruction
                // never changes the register value
                if (ro[i.a] == r[i.a] && c[i.a])
                {
                    flag = 'x';
                    i.op = nop;
                }
                break;
            
            case nop:
            case inp:
                break;
        }
        
        //for (int r=0;r!=4;++r)
        //    cout << c[r];
        //cout << flag << endl;

    }
}

// works, but useless for my problem 
void DependancyAnalysis(Program& in, Registers ofInterest)
{
    Registers hot = ofInterest;
    Registers phot{0};
        
    // vector< pair< Registers, bool > > hot_result;
    
    // from last instruction, backwards
    int idx = in.size()-1;
    while(idx>=0)
    {
        phot = hot;
        
        auto i = in[idx];
        switch (i.op)
        {
            // reads overwrite a register
            case inp:
                hot[i.a]=0;

                // from this discovered: all use: inp - 1001
                // so only w & z need using, xy are irrelevant for state caching
                //cout << "inp - ";
                //for (int r=0;r!=4;++r)
                //    cout << phot[r];
                //cout << endl;
                
                break;
                
            // equal to self, and divide by self,
            // both reset A to a constant value
            case Op::div:
            case eql:
                if (i.a==i.b)
                {
                    hot[i.a]=0;
                    break;
                }
                
            case add:
            case mul:
            case mod:
                // we make B hot ONLY if we write to a hot register
                if (hot[i.a])
                    hot[i.b]=1;
                break;
            
            case mulc:
                // multiply by 0 resets a to constant value 0, 
                if (i.b==0)
                {
                    hot[i.a]=0;
                    break;
                }
                
            case addc:
            case Op::divc:
            case modc:
            case eqlc:
                // constant ops no not create register dependancies
                break;
            
            case nop:
                break;
                
        }
        
        // op writes a value to a register that wont be used
        if (i.op!=inp && phot[i.a]==0)
        {
            in.erase(in.begin()+idx);
        }
        
        idx--;
    }
}

void DependancyAnalysis(Program& in, const string& ofInterest)
{
    Registers r={0};
    for (auto c:ofInterest)
        r[RegisterIndex(c)]=1;
    DependancyAnalysis(in, r);
}

void Optimise(Program& in, const string& ofInterest)
{
    int before = in.size();
    
    FoldConstants(in);
    
    // no-op removal
    auto itr = remove_if(in.begin(),in.end(),[](auto ins){
        return ins.op==nop;}
    );
    
    // multiply/divide by 1 - removal
    itr = remove_if(in.begin(),itr,[](auto ins){
        return (ins.op==divc || ins.op==mulc) && ins.b==1;}
    );
    
    // add/subtract zero
    itr = remove_if(in.begin(),itr,[](auto ins){
        return ins.op==addc && ins.b==0;}
    );

    in.erase(itr,in.end());
    
    // removes instructions that write values to registers that are then not used
    DependancyAnalysis(in, ofInterest);


    int after = in.size();
    cout<<"Optimise removed " << before-after << " instructions"<<endl;
}

Program Compile(const vector<string>& listing, const string& ofInterest=string("wxyz"))
{
    Program result;
    result.reserve(listing.size());
    for(const string& line : listing)
        result.push_back( Compile(line) );

    Optimise(result,ofInterest);
    return result;
}

namespace
{
    void TestSample1()
    {
        auto p = Compile(vector<string>{"inp x","mul x -1"});
        auto r = Run(p,{1});
        assert(r.get('x')==-1);
    }
    
    void TestSample2()
    {
        auto p = Compile(vector<string>{
            "inp z",
            "inp x",
            "mul z 3",
            "eql z x"
        });
        auto r1 = Run(p,{3,9});
        assert(r1.get('z')==1);
        auto r2 = Run(p,{4,9});
        assert(r2.get('z')==0);
    }
    
    void TestOptimise()
    {
        auto p = Compile(vector<string>{
            "inp z",
            "inp x",
            "#inp y",
            "mul z 3",
            "add y 5", 
            "eql z x",
            "#eql z x",
        },"z");
        
        assert(p.size()==4);
        
        auto r1 = Run(p,{3,9});
        assert(r1.get('z')==1);
        auto r2 = Run(p,{4,9});
        assert(r2.get('z')==0);
    }
    
    void SelfTest()
    {
        assert( Compile("inp w")==(Instruction{inp,0,0}) );
        assert( Compile("mul x 0")==(Instruction{mulc,1,0}) );
        assert( Compile("mul x w")==(Instruction{mul,1,0}) );
        
        TestSample1();
        TestSample2();
        TestOptimise();
    }
    /*
    void InputTest(const  Program& program)
    {
        vector<int> t={0,0,0};
        map< Registers, int > count;
        
        for (int a=1;a<=9;++a)
        {
            for (int b=1;b<=9;++b)
            {
                for (int c=1;c<=9;++c)
                {
            
                    t[0]=a;
                    t[1]=b;
                    t[2]=c;
                    auto r = Run(program, t);
                    
                    // w is input, z is accumulator
                    r[0]=0;//x
                    // r[1]=0;//x
                    r[2]=0;//y
                    r[3]=0;//z
                    
                    count[r]++;
                }
            }
        }
        cout << count.size() << endl;
        //for (auto kvp : count )
        //    cout << kvp.
    }
    */
}

bool Test(const  Program& monad, const Input& input)
{
    //if (find(input.begin(),input.end(),0)!=input.end())
    //    return false;

    auto r = Run(monad,input);
    return (r.get('z')==0);
}

void TwentyFour()
{
    SelfTest();

    Program monad;
    monad.reserve(255);

    std::ifstream file("data/monad24.txt");
    string line;
    while(getline(file,line))
    {
        // no clue why std io is behaving incorrectly 
        // but this hack fixes my excess carriage-returns from stdin
        while (isspace(line.back())) line.pop_back();
        
        monad.push_back(Compile(line));
    }
    
    Optimise(monad,"z");
    
    //InputTest(monad);
    // exit(0);
    
    // const char* example="13579246899999";
    cout << "Part 1:" << endl;    
    Input highest;
    highest.resize(14);
    for(int i=0;i!=14;++i)
        highest[i]={9,1};

    Run(monad,highest);
    
    cout << "Part 2:" << endl;
    Input lowest;
    lowest.resize(14);
    for(int i=0;i!=14;++i)
        lowest[i]={1,9};
    
    Run(monad,lowest);

    cout << hits << " / " << (hits+misses) << " = " << (100.0 * hits) / (hits+misses) << "%" << endl;
    /*
    const size_t nthreads = std::thread::hardware_concurrency();
    atomic<long int> top = 99962836000001;

    std::mutex progress_lock;
    atomic<char> report='-';
    const int reportf=6;
    
    auto fn = [&top,&report,&monad,&progress_lock]()
    {
        char buffer[15]={0};
        
        vector<int> candidate;
        candidate.resize(14);
        for(int i=0;i!=14;++i)
            candidate[i]=1;

        do{
            const long int l = top--;
            
            sprintf(buffer,"%ld", l);
            if (report.load()!=buffer[reportf])
            {
                if (progress_lock.try_lock())
                {
                    if (report!=buffer[reportf])
                    {
                        cout << buffer << " and counting." << endl;
                        report.store(buffer[reportf]);
                    }
                    progress_lock.unlock();
                }
            }
            
            for(int i=0;i!=14;++i)
                candidate[i] = candidate[i]-'0';
        
            if (Test(monad,candidate))
            {
                cout << "FOUND MAX VALID:" << endl;
                for (int i:candidate) cout<<i;
                cout << endl;
                top.store(0);
                break;
            }

        }while(top.load()>0);
    };
    
    std::thread threads[nthreads-1];
    for(unsigned int t=0;t<nthreads-1;++t)
    {
        threads[t] = std::thread(fn);
    }
    
    // wait for all the other threads to complete
    for(auto& x : threads)
    {   
        x.join();
    }
    */
    
}
