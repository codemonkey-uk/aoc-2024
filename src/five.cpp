#include <iostream>
#include <vector>
#include <list>
#include <cstring>
#include <cctype>

using namespace std;

//template< typename pred >
//vector< string > Split( const string& in, pred fn )

//template< typename pred >
//vector< string > Filter( vector< string > data, pred fn )

template< typename T >
class Grid
{
    public:
    // width = x, height = y
    Grid(int w, int h) : width(w), height(h)
    {
        data.resize(w*h);
    }
    
    T& get(int x, int y) {
        return data[x+y*width];
    }
    
    void doColumn(int x, int y1, int y2)
    {
        if (y1>y2)
            swap(y1,y2);
        // inclusive loop
        for (int y = y1;y<=y2;++y)
            get(x,y)++;
    }
    void doRow(int y, int x1, int x2)
    {
        if (x1>x2)
            swap(x1,x2);
        // inclusive loop            
        for (int x = x1;x<=x2;++x)
            get(x,y)++;
    }
    
    int count() const
    {
        int r=0;
        for(int i : data)
            if (i>1) r++;
        return r;
    }
        
    private:
    
    int width=0;
    int height=0;
    vector< T > data;
};

struct Line{
    int x1,y1;
    int x2,y2;
    void Parse(const string& in)
    {
        sscanf(in.c_str(), "%i,%i -> %i,%i", &x1,&y1, &x2,&y2);
    }
    
    void Print()
    {
        printf("%i,%i -> %i,%i\n", x1,y1, x2,y2);
    }
    
    void Grow(Line l)
    {
        x1 = min(x1,l.x1);
        x1 = min(x1,l.x2);
        y1 = min(y1,l.y1);
        y1 = min(y1,l.y2);
        x2 = max(x2,l.x1);
        x2 = max(x2,l.x2);
        y2 = max(y2,l.y1);
        y2 = max(y2,l.y2);
    }
};

void Five()
{
    string line;
    
    Line maxExtent;
    getline(cin,line);
    maxExtent.Parse(line);
    
    std::vector< Line > data;
    data.push_back(maxExtent);
    if (maxExtent.x1>maxExtent.x2)
        swap(maxExtent.x1,maxExtent.x2);
    if (maxExtent.y1>maxExtent.y2)
        swap(maxExtent.y1,maxExtent.y2);
        
    while (getline(cin,line))
    {
        Line l;
        l.Parse(line);
        data.push_back(l);
        maxExtent.Grow(l);
    }
    
    cout << data.size() << " lines, " 
         << "from " << maxExtent.x1 << "," << maxExtent.y1 
         << " to " << maxExtent.x2 << "," << maxExtent.y2 << endl;
    
    Grid<int> grid(maxExtent.x2+1, maxExtent.y2+1);
    for (auto line : data)
    {
        if (line.x1==line.x2)
            grid.doColumn(line.x1,line.y1,line.y2);
        else if (line.y1==line.y2)
            grid.doRow(line.y1,line.x1,line.x2);
        else
            line.Print();
    }
    
    cout << "result: " << grid.count() << endl;
}


