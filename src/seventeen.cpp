// seventeen.cpp
// "Trick Shot"

#include <iostream>
#include <vector>

using namespace std;

typedef long long Integer;

struct Vec
{
    Integer x, y;
    bool operator==(const Vec& rhs)const{return x==rhs.x && y==rhs.y;}
};

struct Probe
{
    Vec p;
    Vec v;
    
    Probe(Vec v_) : p{0,0},v(v_)
    {
    }
    Probe(Integer x, Integer y) : p{0,0}, v{x,y}
    {
    }
    Probe(Probe& rhs): p(rhs.p),v(rhs.v){}
    
    void Step()
    {
        // The probe's x position increases by its x velocity.
        p.x+=v.x;
        // The probe's y position increases by its y velocity.
        p.y+=v.y;
        // Due to drag, the probe's x velocity changes by 1 toward the value 0; 
        // that is, it decreases by 1 if it is greater than 0, 
        if (v.x>0) v.x-=1;
        // increases by 1 if it is less than 0, or does not change if it is already 0.
        if (v.x<0) v.x+=1;
        // Due to gravity, the probe's y velocity decreases by 1.    
        v.y-=1;
    }
};

struct Box
{
    Vec p1,p2;
    
    bool WithinX(const Vec& p) const {
        return p.x>=p1.x && p.x<=p2.x;
    }
    bool WithinY(const Vec& p) const {
        return p.y>=p1.y && p.y<=p2.y;
    }
    bool Within(const Vec& p) const {
        return WithinX(p) && WithinY(p);
    }
    bool Within(const Probe& p) const {
        return WithinX(p.p) && WithinY(p.p);
    }
};

typedef vector< Vec > History;
Box Bounds(const History& h, Box b={{0,0},{0,0}})
{
    for(auto p : h){
        if (p.x<b.p1.x) b.p1.x=p.x;
        if (p.x>b.p2.x) b.p2.x=p.x;
        if (p.y<b.p1.y) b.p1.y=p.y;
        if (p.y>b.p2.y) b.p2.y=p.y;
    }
    return b;
}

void DebugDraw(const History& h, Box target)
{
    Box bounds = Bounds(h,target);
    for (int y=bounds.p2.y;y>=bounds.p1.y;--y)
    {
        for (int x=bounds.p1.x;x<=bounds.p2.x;++x)
        {
            Vec v{x,y};
            char c = (find(h.begin(),h.end(),v)!=h.end()) 
                ? '#' 
                : target.Within(v) ? 'T' : '.';
            cout << c;
        }
        cout << endl;
    }
    cout << endl;
}

Box Read(const char* str)
{
    int x1,x2,y1,y2;
    
    sscanf(str, "target area: x=%i..%i, y=%i..%i", &x1, &x2, &y1, &y2);
    if (x1>x2) swap(x1,x2);
    if (y1>y2) swap(y1,y2);
    return Box{{x1,y1},{x2,y2}};
}

bool Hit(const Probe& p, const Box& target)
{
    return target.Within(p.p);
}

Integer Sign(Integer i)
{
    return i > 0 ? 1 : -1;
}

// our Progress is:
// 0 if we are ON target.
// 1 if we are heading TOWARDS the target
// -1 if we are NOT moving TOWARDS the target (moving away or not moving)
int ProgressX(const Probe& p, const Box& t)
{
    if (t.WithinX(p.p)) return 0;
    Integer dx = t.p1.x - p.p.x;
    return (Sign(dx)==Sign(p.v.x)) ? 1 : -1;
}

// our Progress is:
// 0 if we are ON target.
// 1 if we are heading UP or are ABOVE the target
// -1 if we are NOT moving TOWARDS the target (moving away or not moving)
int ProgressY(const Probe& p, const Box& t)
{
    if (t.WithinY(p.p)) return 0;
    if (p.v.y>0 || p.p.y > t.p2.y) return 1;
    else return -1;
}


void Seventeen()
{
    History h;

    // const char* input = "target area: x=20..30, y=-10..-5";
    const char* input = "target area: x=70..96, y=-179..-124";
    Box target = Read(input);

    // find a hypothetical min x to reach the box ignoring y
    Integer mindx = 0;
    Integer testx = target.p1.x;
    while (testx>0)
    {
        mindx++;
        testx-=mindx;
    }
    
    Integer mindy=target.p1.y;
    
    Integer dy=mindy-1;
    Integer dx=mindx-1;
    Integer max_apex=0;
    Integer unique_hits=0;
    bool hit;
    do{
        Probe p(dx,dy);
        Integer apex=0;
        while (!Hit(p,target) && (ProgressX(p,target)>-1 && ProgressY(p,target)>-1))
        {
            // h.push_back(p.p);
            if (p.p.y>apex) apex=p.p.y;
            p.Step();
        }
        hit = Hit(p,target);
        if (p.p.y>apex) apex=p.p.y;
        
        if (hit)
        {
            unique_hits++;
            if (max_apex<apex)
            {
                cout << dx << ',' << dy << " = " << apex << endl;
                max_apex = apex;
            }
            dy++;
        }
        else
        {
            dy++;
            if (dy>=-target.p1.y)
            {
                dy=mindy-1;
                dx++;
            }
        }
    }while(dx<=target.p2.x);
    
    assert( max_apex==15931 );
    cout << "unique_hits = " << unique_hits << endl;
    //h.push_back(p.p);
    //DebugDraw(h,target);
}



