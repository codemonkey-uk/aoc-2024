// eleven.cpp

#include <iostream>
#include <vector>

#include "util.h"

using namespace std;

struct Octopus {
    int energy;
    bool burst;
};

void GainEnergy(Grid< Octopus >& grid)
{
    for (int y=0;y!=grid.height;++y)
    {
        for (int x=0;x!=grid.width;++x)
        {
            if (grid.get(x,y).burst)
                grid.get(x,y).energy=0;
                
            grid.get(x,y).energy++;
            grid.get(x,y).burst=false;
        }
    }  
}

int Burst(Grid< Octopus >& grid)
{
    int count=0;
    for (int y=0;y!=grid.height;++y)
    {
        for (int x=0;x!=grid.width;++x)
        {
            if (grid.get(x,y).energy>9 && grid.get(x,y).burst==false)
            {
                count++;
                grid.get(x,y).burst=true;
                
                int x1=max(0,x-1);
                int y1=max(0,y-1);
                int x2=min(grid.width,x+2);
                int y2=min(grid.height,y+2);
                for (int yn=y1;yn!=y2;++yn)
                {
                    for (int xn=x1;xn!=x2;++xn)
                    {
                        grid.get(xn,yn).energy++;
                    }
                }
            }
            
            if (grid.get(x,y).burst)
                grid.get(x,y).energy=0;

        }
    }
    return count;
}

void Print(Grid< Octopus >& grid)
{
    for (int y=0;y!=grid.height;++y)
    {
        for (int x=0;x!=grid.width;++x)
        {
            cout << grid.get(x,y).energy%10;
        }
        cout << endl;
    }
    cout << endl;    
}

void Eleven()
{
    string line;
    getline(cin,line);
    const size_t w = line.size();
    vector<string> lines;
    do
    {
        assert(line.size()==w);
        lines.push_back(line);
    } while (getline(cin,line));
    const size_t h = lines.size();
    Grid< Octopus > grid( w, h );
    for (size_t x=0;x!=w;++x)
    {
        for (size_t y=0;y!=h;++y)
        {
            int i = lines[y][x]-'0';
            assert(i >= 0 && i < 10);
            grid.get(x,y) = {i, false};
        }
    }
    
    // PART 1
    int sum = 0;
    int n = 0;
    for (;n!=100;++n)
    {
        GainEnergy(grid);
        
        int subtotal = 0;
        while(int f = Burst(grid))
            subtotal+=f;
            
        sum+=subtotal;
    }
    
    Print(grid);
    cout << sum << " flashes after "<< n << " steps" <<endl;

    int subtotal = 0;
    do
    {
        n++;
        GainEnergy(grid);
        
        subtotal = 0;
        while(int f = Burst(grid))
            subtotal+=f;
        
    } while (subtotal != w*h);
    
    cout << n << " step until synchronised" << endl;

}