// twelve.cpp

#include <iostream>
#include <vector>
#include <set>

#include "util.h"

using namespace std;

bool debug=false;

struct Node
{
    Node(const string& n):name(n){}
    Node(const char* n):name(n){}
    
    string name;
    mutable int visits=0;
    mutable vector< const Node* > edges;
    bool small()const{return name[0]==tolower(name[0]);}
    
    bool operator< (const Node& rhs) const
    {
        return name<rhs.name;
    }
};

typedef set< Node > Graph;
typedef vector<const Node*> Path;

void Explore( Path& path, const Node* node, int* count )
{
    if (node->small())
    {
        // do not visit a small path more than once
        if(find(path.begin(),path.end(),node)!=path.end())
            return;
    }
    
    if (node->name=="end")
    {
        if (debug)
        {
            for(const Node* step : path)
                cout << step->name << ",";
            cout << node->name << "." << endl;
        }
        (*count)++;
        return;
    }
    
    path.push_back(node);
    for (const Node* next : node->edges)
    {
        Explore(path,next,count);
    }
    path.pop_back();
}

void Explore2( Path& path, const Node* node, int* count )
{
    if (node->small())
    {
        //we can visit ONE small node TWICE
        if (node->visits>0)
        {
            // any nodes with >1 visits on path already?
            if(find_if(path.begin(),path.end(),
                [](auto nop){return nop->small() && nop->visits>1;})!=path.end())
                return;
                
            // also dont return to 1st node
            if (node==path[0])
                return;
        }
    }
    
    if (node->name=="end")
    {
        if (debug)
        {
            for(const Node* step : path)
                cout << step->name << ",";
            cout << node->name << "." << endl;
        }
        (*count)++;
        return;
    }
    
    path.push_back(node);
    node->visits++;
    
    for (const Node* next : node->edges)
    {
        Explore2(path,next,count);
    }
    
    path.pop_back();
    node->visits--;
}

void Twelve()
{
    string line;
    Graph graph;
    
    while(getline(cin,line))
    {
        vector p = Split(line,'-');
        assert(p.size()==2);

        graph.insert(p[0]).first->edges.push_back( 
            &*graph.insert(p[1]).first 
        );
        graph.insert(p[1]).first->edges.push_back( 
            &*graph.insert(p[0]).first 
        );
    }
    
    if (debug)
    {
        for (const auto& node : graph)
        {
            for (const auto& edge : node.edges)
            {
                cout << node.name << "-" << edge->name << endl;
            }
        }
    }
    
    Path path;
    int count = 0;
    Explore(path, &*graph.find("start"), &count);
    cout << "part 1: " << count << endl;
    
    count = 0;
    assert(path.size()==0);
    Explore2(path, &*graph.find("start"), &count);
    cout << "part 2: " << count << endl;
    
}