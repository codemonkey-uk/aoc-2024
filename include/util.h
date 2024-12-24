#pragma once

#include <string>
#include <vector>
#include <algorithm>

template< typename pred >
std::vector< std::string > Split( const std::string& in, pred fn )
{
    std::vector< std::string > result;

    auto last = in.end();
    auto a = in.begin();

    result.reserve( std::count_if( a, last, fn ) );

    while(a!=last) {
        auto b = std::find_if(a, last, fn);
        if (b==last)
            break;
            
        result.push_back( std::string(a, b) );
        a = ++b;
    }
    
    result.push_back( std::string(a, last) );

    return result;
}

inline std::vector< std::string > Split( const std::string& in, char c )
{
    return Split(in, [c](char c2){return c==c2;});
}

inline std::string Join( const std::vector< std::string >& in, char c=' ')
{
    std::string result;
    for( const auto& s : in)
    {
        if (!result.empty()) result += c;
        result += s;
    }
    return result;
}

template< typename pred >
std::vector< std::string > Filter( std::vector< std::string > data, pred fn )
{
    auto e = std::remove_if( data.begin(),data.end(),fn );
    data.erase(e,data.end());
    return data;
}

struct Pos
{
    int col;
    int row;

    bool operator<(const Pos& rhs)const{return col<rhs.col || (col==rhs.col && row<rhs.row);}    
    bool operator==(const Pos& p) const { return col==p.col && row==p.row; }
    bool operator!=(const Pos& p) const { return col!=p.col || row!=p.row; }
    Pos& operator+=(const Pos& p) { col+=p.col; row+=p.row; return *this; }
    Pos& operator-=(const Pos& p) { col-=p.col; row-=p.row; return *this; }
    Pos operator+(const Pos& p) const { return {col+p.col,row+p.row}; }
};

template< typename T >
class Grid
{
    public:
    // width = x, height = y
    Grid(int w, int h) : width(w), height(h)
    {
        data.resize(w*h);
    }

    Grid() = default;
    Grid(const Grid& g) = default;

    const T& get(int x, int y) const {
        return data[x+y*width];
    }
    
    T& get(int x, int y) {
        return data[x+y*width];
    }
    
    void fill(T c) { for(T& d:data)d=c; }
  
    int width=0;
    int height=0;
    std::vector< T > data;
};

extern bool debug;