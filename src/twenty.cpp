// twenty.cpp
// Tench Map


#include <iostream>
#include "util.h"

using namespace std;

void Print(const Grid< char >& grid)
{
    for (int y=0;y!=grid.height;++y)
    {
        for (int x=0;x!=grid.width;++x)
        {
            cout << grid.get(x,y);
        }
        cout << endl;
    }
    cout << endl;
}

int CountLit(const Grid< char >& grid)
{
    int r = 0;
    for (int y=0;y!=grid.height;++y)
    {
        for (int x=0;x!=grid.width;++x)
        {
            r += grid.get(x,y)=='#';
        }
    }
    return r;
}

char SafeGet(const Grid< char >& grid, int x, int y, char d='.')
{
    if (x>=0 && x<grid.width && y>=0 && y<grid.height)
        return grid.get(x,y);
    else 
        return d;
}

unsigned int GetKernel(char d)
{
    if (d=='.') return 0;
    return 0b111111111;
}

unsigned int GetKernel(const Grid< char >& grid, int cx, int cy, char d)
{
    unsigned int result = 0;

    for (int y=cy-1;y<=cy+1;++y)
    {
        for (int x=cx-1;x<=cx+1;++x)
        {
            result <<= 1; 
            char c = (x>=0 && x<grid.width && y>=0 && y<grid.height) 
                ? grid.get(x,y)
                : d;
            if (c=='#')
                result |= 1;
        }
    }
    
    return result;
}
/*
struct DynamicGrid 
{
    Grid< char > grid;
    string algorithm;
    
    unsigned int GetKernel(int cx, int cy, int steps)
    {
        if (steps==0) return ::GetKernel(grid, cx, cy);
        
        unsigned int result=0;
        for (int y=cy-1;y<=cy+1;++y)
        {
            for (int x=cx-1;x<=cx+1;++x)
            {
                result <<= 1; 
                char c =get(x, y, steps-1);
                if (c=='#')
                {
                    result |= 1;
                }
            }
        }
        return result;
    }
    
    char get(int cx, int cy, int steps)
    {
        if (steps==0) return SafeGet(grid,cx,cy);
        return algorithm[ GetKernel(cx, cy, steps) ];
    }
    
    void Print(int steps)
    {
        if (steps==0) ::Print(grid);
        else
        {
            for (int y=-steps;y!=grid.height+steps;++y)
            {
                for (int x=-steps;x!=grid.width+steps;++x)
                {
                    cout << get(x,y,steps);
                }
                cout << endl;
            }
            cout << endl;
        }
    }
    
    int CountLit(int steps)
    {
        int c=0;
        for (int y=-steps;y!=grid.height+steps;++y)
        {
            for (int x=-steps;x!=grid.width+steps;++x)
            {
                c += get(x,y,steps) == '#';
            }
        }
        return c;
    }    
};
*/
Grid<char> Step(const Grid<char>& source, const string& algorithm, char d)
{
    // grow border
    int x1 = - 1;
    int x2 = source.width + 1;
    int y1 = - 1;
    int y2 = source.height + 1;
    Grid<char> result(1+x2-x1, 1+y2-y1);
    
    for (int y=0;y!=result.height;++y)
    {
        for (int x=0;x!=result.width;++x)
        {
            unsigned int i = GetKernel( source, x+x1, y+y1, d );
            assert(i>=0 && i<algorithm.size());
            char c = algorithm[i];
            result.get(x,y)=c;
        }
    }
    return result;
}

void Twenty()
{
    // load the algorithm, 
    // combine split lines to handle split line sample copy-paste
    string line;
    string algorithm;
    while(getline(cin,line))
    {
        if (line.empty()) break;
        algorithm+=line;
    }
    assert(algorithm.size()==512);
    assert(line.size()==0);
    
    // load source image, track width as we go
    unsigned int source_width =0;
    vector< string > source_image;
    while(getline(cin,line))
    {
        source_image.push_back(line);
        if (line.size()>source_width)source_width=line.size();
    }
    
    // copy source image strings into a grid of bool
    Grid<char> image(
        source_width,
        source_image.size()
    );
    for (unsigned int y=0;y!=source_image.size();++y)
        for (unsigned int x=0;x!=source_width;++x)
            image.get(x,y)=source_image[y][x];
    
    // 
    Print(image);

    /* Recursive solution stupid slow at 50 deep
    DynamicGrid dg{image,algorithm};
    const int steps = 2;
    for (int s = 1; s<=steps; ++s){
        dg.Print(s);
    }
    
    cout << "Part 1 (2 steps) = " << dg.CountLit(steps) << endl;
    cout << "Part 2 (50 steps) = " << dg.CountLit(50) << endl;    
    */
    
    const int part1 = 2;
    int s = 1;

    char d = '.';

    for (;s<=part1; ++s){
        image = Step(image, algorithm, d);
        d = algorithm[GetKernel(d)];
        Print(image);
    }
    cout << "Part 1 ("<<part1<<" steps) = " << CountLit(image) << endl;

    // CORRECT - Your puzzle answer was 5437.
    const int part2=50;
    for (;s<=part2; ++s){
        image = Step(image, algorithm, d);
        d = algorithm[GetKernel(d)];
        //Print(image);
    }
    
    // expected (sample) = 3351
    cout << "Part 2 ("<<part2<<" steps) = " << CountLit(image) << endl;
}

