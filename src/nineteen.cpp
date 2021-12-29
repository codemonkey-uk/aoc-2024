// nineteen.cpp
// Beacon Scanner

#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include "geometry/vector3d.h"
#include "geometry/aabb3d.h"
#include "geometry/matrix4.h"
#include "geometry/geometry_constants.h"

#include "util.h"

using namespace std;
using namespace Geometry;

typedef double ScalarT;
typedef Vector3d< ScalarT > Vec;
typedef Vector2d< ScalarT > Vec2;
typedef Vector3d< ScalarT > Beacon;
typedef AxisAlignedBoundingBox3d< ScalarT > Aabb;
typedef Matrix4< ScalarT > Transform;

namespace std
{
    template<>
    struct hash< Transform >
    {
        std::size_t operator()(Transform const& t) const noexcept
        {
            size_t result = 0;
            const auto s = t.GetSize();
            for(int r=0;r!=s[0];++r)
                for(int c=0;c!=s[1];++c)
                    result = result * 31 + t[r][c];
                    
            return result;
        }
    };
}

// both 16
bool gluInvertMatrix(const double* m, double* invOut)
{
    double inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}

Transform Invert(const Transform& matrix)
{
    double result[16];
    gluInvertMatrix(&matrix.mData[0], &result[0]);
    
    Transform rm(result);

    return  rm;
}

std::ostream &operator <<(std::ostream &out, const Transform &t)
{
    cout.setf(ios::fixed, ios::floatfield); // set fixed floating format cout.precision(2);

    
    const auto s = t.GetSize();
    for(int r=0;r!=s[0];++r)
    {
        for(int c=0;c!=s[1];++c)
            out <<setw(5)<<setprecision(2)<<setfill(' ') << t[r][c] << ", ";
        cout << endl;
    }

    return out;
}

std::ostream &operator <<(std::ostream &out, const Beacon::BaseType &b)
{
    out << b[0] << ',' << b[1] << ',' << b[2];
    return out;
}

struct IndexPair 
{
    unsigned int a;
    unsigned int b;
};

struct DistancePair : public IndexPair
{
    ScalarT d;
};

constexpr ScalarT Precision = 0.1;

bool Equal(Beacon::BaseType a, Beacon::BaseType b)
{
    return a.DistanceSquare(b)<Precision;
}

struct Scanner
{
    vector< Beacon > all;
    vector< DistancePair > distances;
    Transform origin;

    bool merged{false};
    vector< Beacon > merged_scanners;    
    
    Aabb Bounds() const
    {
        Aabb result({0,0,0},{1,1,1});
        result.ExpandToContain( Beacon(origin*Vec(0,0,0)) );
        
        for(auto p : all){
            result.ExpandToContain( Beacon(origin*p) );
        }
        return result;
    }
    
    void Draw2dXY() const 
    {
        Aabb b = Bounds();
        Beacon o( origin*Beacon({0,0}) );
        
        for(int y=b.GetMaxBound().GetY();y>=b.GetMinBound().GetY();--y)
        {
            for(int x=b.GetMinBound().GetX();x<b.GetMaxBound().GetX();++x)
            {
                if (x==o[0] && y==o[1])
                {
                    cout << 'S';
                }
                else if (find_if(all.begin(),all.end(),
                    [x,y,this](auto b){
                        b=origin*b;
                        return b.Vector2dXY().DistanceSquare(Vec2(x,y))<Precision;}
                    )==all.end())
                {
                    cout << '.';
                }
                else 
                {
                    cout << 'B';
                }
            }
            cout << endl;
        }
    }
    
    void Report() const 
    {
        cout << "--- scanner X ---" << endl;
        cout << origin;
        for (auto b : all)
        {
            Beacon c(origin*b);
            cout << c << " <- " << b << endl;
        }
        cout << endl;
    }
    
    int CountMatches( const Scanner& reference )
    {
        int score=0;
        for (auto b : all)
        {
            Beacon c(origin*b);
            if (find_if(reference.all.begin(),reference.all.end(),
                [c](auto refb){
                    return refb.DistanceSquare(c)<Precision;
                }
                )!=reference.all.end()) score++;
        }
        return score;
    }
    
    void Merge( const Scanner& from )
    {
        all.reserve(all.size() + from.all.size());
        
        for (auto b : from.all)
        {
            Beacon c(from.origin*b);
            if (find_if(all.begin(),all.end(),
                [c](auto refb){
                    return refb.DistanceSquare(c)<Precision;
                }
                )==all.end())
            {
                all.push_back(c);
            }
        }
        distances.clear();
        CalculateDistances();
        
        merged_scanners.reserve( 
            merged_scanners.size() + from.merged_scanners.size() + 1
        );

        for (auto scanner : from.merged_scanners)
            merged_scanners.push_back( Vec(from.origin * scanner) );
            
        merged_scanners.push_back( Vec(from.origin * Vec{0,0,0}) );
    }
    
    vector< DistancePair > CalculateDistances() 
    {
        assert(distances.empty());
        distances.reserve( all.size()*all.size() );
        for (unsigned int a=0;a!=all.size()-1;++a)
        {
            // for (unsigned int b=a+1;b!=all.size();++b)
            for (unsigned int b=0;b!=all.size();++b)
            {
                if (a!=b)
                {
                    // distance between two probes
                    auto d = all[a].DistanceSquare(all[b]);
                
                    distances.push_back({
                        {a,b},d
                    });
                }
            }
        }
        sort(
            distances.begin(),distances.end(),
            [](const auto& a, const auto& b){
                return a.d > b.d;
            }
        );
        return distances;
    }
};

Beacon ReadBeacon(const string& line)
{
    Beacon result(0,0,0);
    
    auto c = Split(line,',');
    switch (c.size()) {
        case 3:
        result[2]=atoi(c[2].c_str());
        case 2:
        result[1]=atoi(c[1].c_str());
        case 1:
        result[0]=atoi(c[0].c_str());
    }
    
    return result;
}

struct DistancePairMatch
{
    Scanner* scanner;
    DistancePair pair;
};

// given two pairs of beacons, 
// assuming a = b in different coordinate spaces, 
// determine the transform to map b coordinates into a-space
bool DetermineOrigin(
    Beacon a1, Beacon a2,
    Beacon b1, Beacon b2,
    Transform* result
)
{
    Vec va(a2-a1);
    
    // apply any thing currently in the transform to B
    Vec lb1(*result * b1);
    Vec lb2(*result * b2);
    Vec vb(lb2-lb1);
    
    // bool rotation = false;
    
    if (Equal(va,vb)) 
    {
        // cout << "Translation:" << endl;
        *result += Transform::Translation(
            a1 - lb1, 0
        );
    }
    else if (Equal(va,-vb))
    {
        // cout << "Translation (invert):" << endl;
        swap(lb2,lb1);
        *result += Transform::Translation(
            a1 - lb1, 0
        );
    }
    else
    {
        // cout << "Rotation:" << endl;
        va.Normalise();
        vb.Normalise();

        result->Transform::BecomeRotationAlign(va,vb);
        // result->Transpose();
        
        // cout << *result;

        DetermineOrigin(
            a1,a2,
            b1,
            b2, result);
    }
    
    // cout << *result; 
    
    Vec r1(*result * b1);
    Vec r2(*result * b2);

    // cout << "Moves B to:\n" << r1 << " -> " << r2 << endl;
    
    bool sucess = ( Equal(r1,a1) && Equal(r2,a2) );
    
    // cout << "DetermineOrigin " << (sucess ? "success!" "failure". << endl;
    
    return sucess;
}

bool DetermineOrigin(
    Beacon a1, Beacon a2, Beacon a3,
    Beacon b1, Beacon b2, Beacon b3,
    Transform* result
)
{
    // Basis vectors for A space:
    
    Vec va_f(a2-a1);
    va_f.Normalise();
    Vec va_r(a3-a1);
    va_r.Normalise();
    Vec va_u = CrossProduct(va_f,va_r);
    va_r = CrossProduct(va_f,va_u);
    
    Transform a_t( va_f, va_r, va_u );

    // Basis vectors for B space:
    
    Vec vb_f(b2-b1);
    vb_f.Normalise();
    Vec vb_r(b3-b1);
    vb_r.Normalise();
    Vec vb_u = CrossProduct(vb_f,vb_r);
    vb_r = CrossProduct(vb_f,vb_u);
    
    Transform b_t( vb_f, vb_r, vb_u );
    
    // invert b_t
    Transform b_ti = Invert(b_t);
    assert( Transform::Identity().Equals(b_t * b_ti, 0.001) );
    
    *result = b_ti * a_t;

    //*result += translation( a2-a1 )
    *result += Transform::Translation(
        a1 - (*result * b1), 0
    );

    //cout << *result; 
    
    Vec r1(*result * b1);
    Vec r2(*result * b2);

    //cout << "Moves B to:\n" << r1 << " -> " << r2 << endl;
    
    bool sucess = ( Equal(r1,a1) && Equal(r2,a2) );
    
    // cout << "DetermineOrigin " << (sucess ? "success!" "failure". << endl;
    
    return sucess;
}


// find a beacon in A and its probable match in B,
// based on the known distances from the pairings indexed ia and ib 
vector<IndexPair> FindCommonThirds(
    const Scanner& a, const Scanner& b,
    const unsigned int ia, const unsigned int ib
    )
{
    unsigned int ja = ia+1;
    unsigned int jb = ib+1;

    vector<IndexPair> result;
    
    // same dual iteration loop
    while(ja<a.distances.size() && jb<b.distances.size())
    {
        if (abs(a.distances[ja].d-b.distances[jb].d)<Precision)
        {
            unsigned int beacon_a;
            unsigned int beacon_aa=a.distances[ja].a;
            unsigned int beacon_ab=a.distances[ja].b;
            
            if (beacon_aa != a.distances[ia].a && 
                beacon_aa != a.distances[ia].b)
            {
                // beacon A is good 
                beacon_a=beacon_aa;
            }
            else if (beacon_ab != a.distances[ia].a && 
                     beacon_ab != a.distances[ia].b)
            {
                beacon_a=beacon_ab;
            }
            else
            {
                ja++;
                continue;
            }
            
            unsigned int beacon_b;
            unsigned int beacon_ba=b.distances[jb].a;
            unsigned int beacon_bb=b.distances[jb].b;
            
            if (beacon_ba != b.distances[ib].a && 
                beacon_ba != b.distances[ib].b)
            {
                // beacon B is good 
                beacon_b=beacon_ba;
            }
            else if (beacon_bb != b.distances[ib].a && 
                     beacon_bb != b.distances[ib].b)
            {
                beacon_b=beacon_bb;
            }
            else
            {
                jb++;
                continue;
            }
                
            result.push_back( IndexPair{beacon_a,beacon_b} );
            ja++;
            jb++;
        }
        else if (a.distances[ja].d > b.distances[jb].d)
        {
            ja++;
        }
        else 
        {
            jb++;
        }
    }
    
    return result;
}

bool FindViaMatchingDistances(const Scanner& a, Scanner& b, int goal)
{
    unsigned int ia=0,ib=0;
    unordered_map< Transform, unsigned int > candidates;
    
    unsigned int target = min((size_t)goal,min(a.all.size(),b.all.size()));
    
    while(ia<a.distances.size() && ib<b.distances.size())
    {
        if (fabs(a.distances[ia].d-b.distances[ib].d)<Precision)
        {
            bool sucess=false;
            
            Beacon a1 = a.all[a.distances[ia].a];
            Beacon a2 = a.all[a.distances[ia].b];
            Beacon b1 = b.all[b.distances[ib].a];
            Beacon b2 = b.all[b.distances[ib].b];

            Vec va(a2-a1);
            Vec vb(b2-b1);
            
            //cout << a1 << " -> " << a2 << " = " << va << endl;
            //cout << "matches (d= " << Sqrt(a.distances[ia].d) << ")" << endl;
            //cout << b1 << " -> " << b2 << " = " << vb << endl;

            auto vj = FindCommonThirds(a,b,ia,ib);
            if (!vj.empty())
            {
                //if (vj.size()>1) 
                //    cout << vj.size() << " triangulation candidates" << endl;
                    
                for (auto j : vj)
                {
                    Beacon a3{0,0,0},b3{0,0,0};
                    a3 = a.all[ j.a ];
                    b3 = b.all[ j.b ];
                    assert(a3!=a1 && a3!=a2);
                    assert(b3!=b1 && b3!=b2);
                
                    //cout << "Can be triangulated with: " << endl;
                    //cout << a3 << " & " << b3 << endl;
                
                    //cout << a1 << " -> " << a3 << "(d= " << a1.Distance(a3)<< ")" << endl;
                    //cout << b1 << " -> " << b3 << "(d= " << b1.Distance(b3) << ")" << endl;
                
                    // try with just 2, works for trivial / 2d test cases
                    b.origin.BecomeIdentity();
                    sucess = (DetermineOrigin(a1,a2,a3,b1,b2,b3, &b.origin));
                    
                    // check if we've tried this transform,
                    if (candidates.find(b.origin)==candidates.end())
                    {
                        // if not see how good it is
                        candidates[b.origin]=b.CountMatches(a);
                        if (candidates[b.origin]>=target)
                            return true;
                    }
                }
            }
            else
            {
                // try with just 2, works for trivial / 2d test cases
                b.origin.BecomeIdentity();
                sucess = (DetermineOrigin(a1,a2,b1,b2, &b.origin));
                if (sucess)
                {
                    // check if we've tried this transform,
                    if (candidates.find(b.origin)==candidates.end())
                    {
                        // if not see how good it is
                        candidates[b.origin]=b.CountMatches(a);
                        if (candidates[b.origin]>=target)
                            return true;
                    }
                }
            }
            
            ia++;
            ib++;
        }
        else if (a.distances[ia].d > b.distances[ib].d)
        {
            ia++;
        }        
        else
        {
            ib++;
        }
    };
    
    cout << candidates.size() << " candidate transforms, best:";

    for (const auto& kvp:candidates)
    {
        if (kvp.second > candidates[b.origin])
            b.origin = kvp.first;
    }

    cout << candidates[b.origin] << " / " << target << " matches." << endl;
    
    return candidates[b.origin]>=target;
}

void Nineteen()
{
    string line;
    
    vector< Scanner > scanners;
    Scanner current;
    while (getline(cin,line))
    {
        if (line.rfind("---",0)==0)
        {
            if (!current.all.empty()) 
            {   
                scanners.push_back( std::move(current) );
                current.all.clear();
            }
        }
        else if (!line.empty())
        {
            current.all.push_back( ReadBeacon(line) );
        }
    }
    if (!current.all.empty()) scanners.push_back(current);
    
    cout << "Found " << scanners.size() << " beacons:\n\t";
    int count = 0;
    for (Scanner& s:scanners)
    {
        count += s.all.size();
        cout << s.all.size() << ' ';
        s.CalculateDistances();
    }
    cout << " = " << count << " scanners" << endl;


    
    int remaining = scanners.size();
    while (remaining>1)
    {
        int mc = 0;
        
        for (unsigned int i=0;i!=scanners.size()-1;++i)
        {
            for (unsigned int j=i+1;j!=scanners.size();++j)
            {
                if (scanners[i].merged==false && scanners[j].merged==false)
                {
                    if (FindViaMatchingDistances(scanners[i], scanners[j], 12))
                    {
                        cout << "Merged " << j << " into " << i << endl;
                        scanners[i].Merge( scanners[j] );
                        remaining--;
                        scanners[j].merged = true;
                        mc++;
                    }
                }
            }
        }
        
        if (mc==0)
        {
            cout << "No further merges found, with " << remaining << " to go." << endl;
            break;
        }
    }

    ScalarT maxd=0;
    int total = 0;
    for (auto& r : scanners)
    {
        if (r.merged==false)
        {
            total += r.all.size();
            cout << r.all.size() << " beacons." << endl;
            
            // put yourself in the scanners list:
            r.merged_scanners.push_back(Vec{0,0,0});
            
            // check all scanner distances
            for (unsigned int i=0;i!=r.merged_scanners.size()-1;++i)
            {
                Vec vi(r.merged_scanners[i]);
                for (unsigned int j=i+1;j!=r.merged_scanners.size();++j)
                {
                    Vec vj(r.merged_scanners[j]);
                    Vec dv(vj - vi);

                    // Manhattan distance
                    auto d = abs(dv[0])+abs(dv[1])+abs(dv[2]);
                    if (d>maxd) maxd=d;
                }
            }
        }
    }
    
    cout << total << " total (part 1)" << endl;
    cout << "distance (part 2): " << maxd << endl;
}


