// eighteen.cpp
// Snailfish


#include <iostream>
#include <vector>

using namespace std;

struct SnailfishNumber
{
    SnailfishNumber* left;
    SnailfishNumber* right;

    SnailfishNumber* get(int i) {
        if (i==0) return left; 
        else return right; 
    }
    
    void set(int i, SnailfishNumber* p) {
        if (i==0) left=p; 
        else right=p;
    }
    
    static bool IsRegularNumber(SnailfishNumber* p)
    {
        return (reinterpret_cast<intptr_t>(p) & 1);
    }

    static intptr_t ReadRegularNumber(SnailfishNumber* p)
    {
        assert (reinterpret_cast<intptr_t>(p) & 1);
        return reinterpret_cast<intptr_t>(p) >> 1;
    }
    
    static SnailfishNumber* EncodeRegularNumber(intptr_t i)
    {
        return reinterpret_cast<SnailfishNumber*>( (i<<1) | 1);
    }

    SnailfishNumber( intptr_t l, intptr_t r )
        : left(EncodeRegularNumber(l))
        , right (EncodeRegularNumber(r))
    { }
    
    SnailfishNumber( SnailfishNumber* l, SnailfishNumber* r)
        : left(l)
        , right(r)
    { 
        Reduce();
    }
    
    void Read(const string& source, int& itr, SnailfishNumber** dest)
    {
        if (source[itr]=='[')
        {   
            *dest = new SnailfishNumber(source,itr);
            assert( !IsRegularNumber(*dest) );
        }
        else
        {
            intptr_t v = 0;
            
            while (source[itr]>='0' && source[itr]<='9')
                v = v*10 + source[itr++]-'0';
            
            *dest = EncodeRegularNumber( v );
        }
    }
    
    SnailfishNumber(const string& source, int& itr)
    {
        assert( source[itr]=='[' );
        itr++;
        
        left=nullptr;
        right=nullptr;
        
        Read(source,itr, &left);
        
        assert( source[itr]==',' );
        itr++;
        
        Read(source,itr, &right);
        
        if (source[itr]!=']')
            cerr << "parse error in " << source << " at column " << itr << endl;
            
        assert( source[itr]==']' );
        
        itr++;
    }
    
    string ToString(SnailfishNumber* p)const
    {
        if (!IsRegularNumber(p))
            return p->ToString();
        else 
            return to_string( ReadRegularNumber(p) );
    }
    
    string ToString()const
    {
        string result;
        result.reserve(8);
        result+='[';
        result+=ToString(left);
        result+=',';
        result+=ToString(right);
        result+=']';
        return result;
    }
    
    ~SnailfishNumber()
    {
        if (!IsRegularNumber(left)) delete left;
        if (!IsRegularNumber(right)) delete right;
    }
    
    bool Explode()
    {
        typedef pair< SnailfishNumber*, int> itr;
        vector< itr > stack;
        
        itr current = {this,0};
        
        itr prior_regular = {nullptr,0};
        itr explode = {nullptr,0};
        itr next_regular = {nullptr,0};
        itr parent = {nullptr,0};
        
        while(current.first && next_regular.first==nullptr)
        {
            itr next = {current.first->get(current.second),0};
            if (!IsRegularNumber(next.first))
            {
                stack.push_back(current);
                current = next;
            }
            else 
            {
                if (explode.first==nullptr) prior_regular = current;
                else if (current.first!=explode.first && explode.first!=nullptr)
                {
                    //cout << "found next: " << current.first->ToString() << 
                    //    " (" << current.second << ")" << endl;
                    next_regular = current;
                }
                
                while (current.second==1)
                {
                    if (stack.empty()) {
                        current.first=nullptr;
                        break;
                    }
                    current = stack.back();
                    stack.pop_back();
                }
                current.second++;
            }
            
            //for (unsigned int i=0;i!=stack.size();++i) cout << '-';
            //cout << current.first->ToString() << endl;
            
            if (stack.size()==4 && explode.first==nullptr)
            {
                explode = current;
                parent = stack.back();
                
                //cout << "explode: " << current.first->ToString() << endl;
                //cout << "within: " << parent.first->ToString() << 
                //    (parent.second ? " (right)" : " (left)") << endl;
            }
        }
        
        if (explode.first!=nullptr)
        {
            auto left = explode.first->get(0);
            assert( IsRegularNumber(left) );
            auto right = explode.first->get(1);
            assert( IsRegularNumber(right) );
            
            auto right_i = ReadRegularNumber( right );
            auto left_i = ReadRegularNumber( left );
            
            // the pair's left value is added to the first regular number 
            // to the left of the exploding pair (if any)
            if (prior_regular.first)
            {
                auto prior_i = ReadRegularNumber( prior_regular.first->get(prior_regular.second) );
                
                //cout << "<- " << prior_i << " + " << left_i << " = " << prior_i+left_i << endl;
                
                prior_regular.first->set(
                    prior_regular.second,
                    EncodeRegularNumber(
                        prior_i + left_i
                    )
                );
            }
            else
            {
                //cout << "<- (none) + " << left_i << endl;
            }
            
            // the pair's left value is added to the first regular number 
            // to the left of the exploding pair (if any)
            if (next_regular.first)
            {
                auto next_i = ReadRegularNumber( next_regular.first->get(next_regular.second) );
                //cout << "-> " << right_i << " + " << next_i << " = " << next_i+right_i << endl;

                next_regular.first->set(
                    next_regular.second,
                    EncodeRegularNumber(
                        next_i+right_i
                    )
                );
            }
            else
            {
                //cout << "-> " << right_i << "+ (none)" << endl;
            }
            
            // Then, the entire exploding pair is replaced with the regular number 0.
            assert( !IsRegularNumber(parent.first->get(parent.second)) );
            delete parent.first->get(parent.second);
            parent.first->set(parent.second, EncodeRegularNumber(0));
            
            return true;
        }
        
        return false;
    }
    
    bool Split()
    {
        if (IsRegularNumber(left))
        {
            auto v = ReadRegularNumber(left);
            if (v>=10)
            {
                auto l = v/2;
                auto r = v-l;
                left = new SnailfishNumber(l,r);
                return true;
            }
        }
        else if (left->Split())
        {
            return true;
        }
        
        if (IsRegularNumber(right))
        {
            auto v = ReadRegularNumber(right);
            if (v>=10)
            {
                auto l = v/2;
                auto r = v-l;
                right = new SnailfishNumber(l,r);
                return true;
            }
        }
        else if (right->Split())
        {
            return true;
        }
        
        return false;
    }
    
    void Reduce()
    {
        bool b;
        do
        {
            b = Explode();
            if (!b) b = Split();
        }while (b);
    }
    
    intptr_t Magnitude() const
    {
        intptr_t l=0;
        intptr_t r=0;
        
        if (IsRegularNumber(left))
            l = ReadRegularNumber(left);
        else
            l = left->Magnitude();
            
        if (IsRegularNumber(right))
            r = ReadRegularNumber(right);
        else
            r = right->Magnitude();
            
        return l*3 + 2*r;
    }

};


SnailfishNumber* ReadLine(const string& line)
{
    int itr = 0;
    return new SnailfishNumber(line,itr);
}

void CheckReadWrite(const char* str)
{
    auto a = ReadLine(str);
    assert( a->ToString()==str );
    delete a;
}

void CheckExplode(const char* str, const char* expected)
{
    auto r = ReadLine(str);
    r->Explode();
    auto o = r->ToString();
    if (o != expected)
    {
        cout << "FAILED!" << endl;
        cout << "exploded: " << str << endl;
        cout << "expected: " << expected << endl;
        cout << "result  : " << o << endl;
    }
    else
    {
        cout << "PASSED: " << str << " -> " << o << endl;
    }
    delete r;
}

void CheckSplit(const char* str, const char* expected)
{
    auto r = ReadLine(str);
    r->Split();
    auto o = r->ToString();
    if (o != expected)
    {
        cout << "FAILED!" << endl;
        cout << "split   : " << str << endl;
        cout << "expected: " << expected << endl;
        cout << "result  : " << o << endl;
    }
    else
    {
        cout << "PASSED: " << str << " -> " << o << endl;
    }
    delete r;
}

void CheckReduce(const char* str, const char* expected)
{
    auto r = ReadLine(str);
    r->Reduce();
    auto o = r->ToString();
    if (o != expected)
    {
        cout << "FAILED!" << endl;
        cout << "reduce   : " << str << endl;
        cout << "expected: " << expected << endl;
        cout << "result  : " << o << endl;
    }
    else
    {
        cout << "PASSED: " << str << " -> " << o << endl;
    }
    delete r;
}

void CheckMagnatude(const char* str, intptr_t expected)
{
    auto r = ReadLine(str);
    auto m = r->Magnitude();
    if (m!=expected)
    {
        cout << "FAILED!" << endl;
        cout << "magnitude: " << str << endl;
        cout << "expected : " << expected << endl;
        cout << "result   : " << m << endl;
    }
    else
    {
        cout << "PASSED: " << str << " -> " << m << endl;    
    }
    delete r;
}

void Eighteen()
{
    CheckReadWrite("[1,2]");
    CheckReadWrite("[[3,4],5]");
    CheckReadWrite("[[1,2],[[3,4],5]]");
    
    CheckExplode("[[[[[9,8],1],2],3],4]", "[[[[0,9],2],3],4]");
    CheckExplode("[7,[6,[5,[4,[3,2]]]]]", "[7,[6,[5,[7,0]]]]");
    CheckExplode("[[6,[5,[4,[3,2]]]],1]", "[[6,[5,[7,0]]],3]");
    CheckExplode(
        "[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]", 
        "[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]"
    );
    CheckExplode(
        "[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]", 
        "[[3,[2,[8,0]]],[9,[5,[7,0]]]]"
    );
    
    CheckSplit("[1,10]","[1,[5,5]]");
    CheckSplit("[11,1]","[[5,6],1]");
    
    CheckReduce("[[[[[4,3],4],4],[7,[[8,4],9]]],[1,1]]", "[[[[0,7],4],[[7,8],[6,0]]],[8,1]]");

    CheckMagnatude("[9,1]", 29);
    CheckMagnatude("[1,9]", 21);
    CheckMagnatude("[[9,1],[1,9]]", 129);
    CheckMagnatude("[[1,2],[[3,4],5]]", 143);
    CheckMagnatude("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]", 1384);
    CheckMagnatude("[[[[1,1],[2,2]],[3,3]],[4,4]]", 445);
    CheckMagnatude("[[[[3,0],[5,3]],[4,4]],[5,5]]", 791);
    CheckMagnatude("[[[[5,0],[7,4]],[5,5]],[6,6]]", 1137);
    CheckMagnatude("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]", 3488);

    string line;
    if (getline(cin,line))
    {
        SnailfishNumber* sum = ReadLine(line);
        while (getline(cin,line))
        {
            sum = new SnailfishNumber(sum, ReadLine(line));
        }
        cout << sum->ToString() << endl;
        cout << sum->Magnitude() << endl;
    }
    
}



