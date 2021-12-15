// -------------------------------------------------------------------------
// Filename:	astar.h
// Version:		2
// Date:		2002/03/08
// Purpose:		Provide template for a* algorythm
// (c) T.Frogley 1999-2021
// Updated: 2021 to better support behaviour controls 
// -------------------------------------------------------------------------

#ifndef ASTAR_H
#define ASTAR_H

#ifdef _MSC_VER
    //identifier was truncated to '255' characters in the browser information
    #pragma warning (disable : 4786)
#endif

//include standard library code
#include <vector>
#include <set>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <limits>

// #define ASTAR_STATS
#ifdef ASTAR_STATS
    #include <time.h>
    #include <iostream>
#endif

namespace astar
{
    //from <vector>
    using std::vector;

    //from <set>
    using std::set;
    using std::unordered_set;

    //from <functional>
    using std::binary_function;
    using std::greater;

    //from <algorithm>
    using std::pop_heap;
    using std::push_heap;

    //max should be in <algorithm>
    //see: http://www.sgi.com/Technology/STL/max.html

    //unfortunatly the Microsoft compiler\headers arn't standard compliant
    //see: http://x42.deja.com/getdoc.xp?AN=520752890

    //thus:	
    #ifdef _MSC_VER

        #ifdef max 
        #undef max
        #endif

        template<class T> inline
        const T& max(const T& a, const T& b)
        { return (a>b)?a:b; }
    #else
        using std::max;
    #endif

    //node = class encapsulating a location on the graph

    /* example node class for use with astar,
       minimal interface - 
       note it is recomended that in most cases nodes are implemented as pointers to data,

    struct example_node{

        //default, & copy constructor, &
        //assignment operator should be available
    
        //example_node::iterator
        //for fetching connected nodes, and costs

        struct iterator{

            //copy constructor and assignment operator should be available

            typedef double cost_type;	//typedef required, must be scalar type

            example_node value()const;	//node
            cost_type cost()const;		//cost/distance to node
        
            iterator& operator++();		//next node
        
            bool operator!=(iterator v);//used by search

        };

        //Get first, and past-end iterators
        iterator begin()const;
        iterator end()const;

        //equality operator, required
        //note: fuzzy equality may be useful
        bool operator==(const xynode b);
    };

    */

    //heuristic examples, estimates of cost from node A to node B

    //use this heuristic when costs don't apply
    template<class T>
    struct no_heuristic{
        //heuristic();
        typename T::iterator::cost_type operator()(const T, const T)
        { return 1; }
    };

    //some useful template functions for creating heuristics for movement on a 2d plane
    //reference: http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html

    //The standard heuristic is the Manhattan distance. 
    //Look at your cost function and see what the least cost is 
    //for moving from one space to another. 
    //The heuristic should be cost times manhattan distance:
    template<class T> inline
    const T manhattan_distance(const T& x1, const T& y1, const  T& x2, const T& y2)
    {
        return (fabs(x1-x2)+fabs(y1-y2));
    }

    //If on your map you allow diagonal movement, then you need a different heuristic. 
    //The Manhattan distance for (4 east, 4 north) will be 8. 
    //However, you could simply move (4 northeast) instead, so the heuristic should be 4. 
    //This function handles diagonals:
    template<class T> inline
    const T diagonal_distance(const T& x1, const T& y1, const  T& x2, const T& y2)
    {
        return (max(fabs(x1-x2),fabs(y1-y2)));
    }

    //If your units can move at any angle (instead of grid directions), 
    //then you should probably use a straight line distance:
    template<class T> inline
    const T straight_distance(const T& x1, const T& y1, const T& x2, const T& y2)
    {
        T dx = (x1-x2);
        T dy = (y1-y2);
        return ( sqrt(dx*dx + dy*dy) );
    }

    //One thing that can lead to poor performance is ties in the heuristic. 
    //When several paths have the same f value, they are all explored, even 
    //though we only need to explore one of them. To solve this problem, we 
    //can add a small tie-breaker to the heuristic. 
    //This tie breaker also can give us nicer looking paths:
    //x1,y1 = start position
    //x2,y2 = current position
    //x3,y3 = target position
    template<class T> inline
    const T amits_modifier(const T& x1, const T& y1, const T& x2, const T& y2, const T& x3, const T& y3)
    {
        T dx1 = x2 - x3;
        T dy1 = y2 - y3;
        T dx2 = x1 - x3;
        T dy2 = y1 - y3;
        T cross = dx1*dy2 - dx2*dy1;
        if( cross<0 ) cross = -cross;
        return cross;
    }

    //node_link (astar implementation helper class)
    //wraps up a node with movement cost / heuristic info 
    //and an index to its parent node in the nodes list
    template<class T>
    struct node_link{
        typedef T value_type;
        typedef typename T::iterator::cost_type scalar;
        typedef std::set<node_link> container;
    
        node_link(){}

        node_link(T n, scalar h, typename container::iterator p):
            myNode(n),
            myH(h),
            myStatus(eNodeNotListed),
            myG(), 
            myParent(&*p)
            { }
            
        node_link(T n, scalar g, scalar h, typename container::iterator p):
            myNode(n),
            myH(h),
            myStatus(eNodeNotListed),
            myG(g),
            myParent(&*p)
            { }

        // for use by the node-set
        inline bool operator<(const node_link<T>& rhs)const noexcept
            { return myNode.hash() < rhs.myNode.hash(); }
        inline bool operator==(const node_link<T>& rhs)const noexcept
            { return myNode == rhs.myNode; }
            
        // for sorting in the pending queue
        inline bool costs_more(const node_link<T> &b)const noexcept
            { return (myG+myH > b.myG+b.myH); }

        inline bool has_parent(typename container::iterator orphanValue) const noexcept
            { return myParent!=&*orphanValue; }

        T myNode;
        scalar myH;
        
        enum NodeList { eNodePending, eNodeDone, eNodeNotListed };
        mutable NodeList myStatus;
        mutable scalar myG;

        // mutable typename container::iterator myParent;
        mutable const node_link* myParent;

        static const size_t sOrphan = static_cast<size_t>(-1);
    };

	//binary_lookup functor (astar implemetatoin helper class)
	//	key	: key to container
	template<class key>
	class binary_lookup : public binary_function<key, key, bool> {
		
		public:
			//constructor, take a copy of functor, 
			//and keep a reference to the container
			binary_lookup()
			{ }
		
			//look up two values in contianer from keys a and b,
			//pass values to binary functor, and return result
			bool operator()(
				const key& a, 
				const key& b	)
			//{ return fn(c[a], c[b]); }
            {return a->costs_more(*b); }
	};

	//configuration info for astar algorythm
	//also used to return some additional information about the finished search
	template<class nodeType>
	struct config{
		typedef typename nodeType::iterator::cost_type scalar;
		
		//construct with sensable defaults / empty results
		config():
		node_limit(std::numeric_limits<size_t>::max()),
		cost_limit(std::numeric_limits<scalar>::max()),
		result_nodes_examined(0),
		result_nodes_pending(0),
		result_nodes_opened(0),
		route_length(0),
		route_cost()
		{ }
		
		//configuration variables

		//node_limit
		//set this to restrict the number of nodes astar will open
		//has the effect of limiting the amount of time spent searching
		size_t node_limit;	

		//cost limit
		//set this to restrict the maximum distance considered 
		//acceptable for a route
		//if astar cannot find a shorter route than this it will fail
		scalar cost_limit;
		
		//result variables
		
		//result_nodes_examined
		//astart sets the to the total number of nodes it 
		//'looked at'  when the search terminated
		size_t result_nodes_examined;

		//result_nodes_pending
		//astar sets this to the number of nodes still waiting 
		//to be examined when the search terminated
		size_t result_nodes_pending;

		//result_nodes_opened
		//astar sets this to the total number of nodes it fetched
		//should equal pending + examined
		size_t result_nodes_opened;

		//route_length
		//astar sets this to equal the total number of nodes
		//used to construct the returned route
		unsigned int route_length;

		//route_cost
		//astart sets the to equal the total "cost" of 
		//the returned route
		scalar route_cost;
	};

	//astar algorithm, as template, 
	//find a path from a to b, 
	//using the given behaviour (providing heuristic), 
	//places results in container [ any class that can push_front( nodeType ) ]
	//returns flase if no route exists
	//returns true if a complete route is found
	//returns true if it exceeds node_limit, but a partial route is found
	//returns false (aborts with a partial route) if it exceeds cost_limit
	template<class behaviourType, class nodeType, class container>
	bool astar(const nodeType a, const nodeType b, container &results, behaviourType behaviour, config<nodeType> &cfg)
	{
		#ifdef ASTAR_STATS
		clock_t time = clock();
		#endif

        typedef node_link<nodeType> node;
        typedef typename node::container node_container;
        typedef vector<typename node_container::iterator> index_container;
        typedef typename nodeType::iterator node_iterator;
        typedef typename nodeType::iterator::cost_type scalar;

        node_container nodes;		//all nodes opened
        index_container pending;	//sorted index to pending nodes
        index_container done;		//unsorted index to nodes already explored
        typename node_container::iterator index;
        bool complete = false;
    
        //reserve space in index vectors to avoid reallocation
        if (cfg.node_limit != std::numeric_limits<size_t>::max()){
            pending.reserve( cfg.node_limit / 2 );
            done.reserve( cfg.node_limit / 2 );
        }

        //create the indirect comparison object
        binary_lookup<
            typename node_container::iterator 
        > sort_index_object;

        //tempory storage for 'working' node data
        node current;
        nodeType next;

        //stick the fist node into the list, 
        //and its index into the pending list
        index = nodes.insert( nodes.begin(), node( a, behaviour.heuristic(a,b), nodes.end() ));
        pending.push_back( index );
        index->myStatus = node::eNodePending;

        do{
        
            //get top rated node
            index = pending.front();
            current = *index;
        
            //remove it from pending list
            pop_heap( pending.begin(), pending.end(), sort_index_object );
            pending.pop_back();

            //stick it in the list of examined nodes
            done.push_back(index);
            index->myStatus = node::eNodeDone; 

            //found target?
            complete = current.myNode==b;
            if (complete) break;

            //failed (based on distance)
            if (current.myG+current.myH>cfg.cost_limit) break;
        
            //for each node connected to the current one
            node_iterator i(current.myNode.begin());
            const node_iterator end(current.myNode.end());
            for (;i!=end;++i){
                next = i.value();
                if (behaviour.filter(next)) continue;

                typename node_container::iterator inserted_at;
                scalar g = i.cost()+current.myG;
                inserted_at = nodes.insert( nodes.begin(), node(next, g, behaviour.heuristic(next, b), index ) );                

                //not in the pending list
                if (inserted_at->myStatus != node::eNodePending){

                    //not in done list (or pending list)
                    if (inserted_at->myStatus != node::eNodeDone){

                        //add to pending list						
                        pending.push_back( inserted_at );
                        inserted_at->myStatus = node::eNodePending;
                        push_heap( pending.begin(), pending.end(), sort_index_object );

                    }
                }
                else{
                
                    //its in the pending list, but is this a better version ?
                    if (g<(inserted_at)->myG){

                        // This is allowed but it's not obvious why:

                        //replace node cost with new path to this node
                        (inserted_at)->myG = g;
                        (inserted_at)->myParent = &*index;
                    
                        // see: http://theory.stanford.edu/~amitp/GameProgramming/path.cpp
                        push_heap( 
                            pending.begin(), 
                            std::find(pending.begin(), pending.end(), inserted_at)+1, 
                            sort_index_object 
                        );

                    }
                }
            }
        }while (!pending.empty() && nodes.size()<cfg.node_limit);

        #ifdef ASTAR_STATS
        clock_t elapsed1 = clock() - time;
        #endif

        cfg.route_length = 0;
    
        //did not exit because a route was found
        if (!complete){
        
            //ran out of time
            if (nodes.size()>=cfg.node_limit && !pending.empty()){

                //best potential
                current = *pending.front();

                //search list of already explored nodes for "better" compromise
                typename index_container::const_iterator j=done.begin();
                typename index_container::const_iterator k=done.end();
                for(;j!=k;++j){
                    if (current.myH>(*j)->myH){
                        current = **j;
                    }
                }

                //partial routes should not be considered a failure
                complete = true;
            }
        }

        #ifdef ASTAR_STATS
        elapsed1 = clock() - time;
        #endif

        //store route length 
        //(including estimate of remaining distance for partial routes)
        cfg.route_cost = current.myG+current.myH;

        //store results of search in "container"
        ++cfg.route_length;
        results.push_front(current.myNode);

        while(current.has_parent(nodes.end())){
            current = *current.myParent;
            results.push_front(current.myNode);

            ++cfg.route_length;
        }

        //store stats for calling code
        cfg.result_nodes_opened = nodes.size();
        cfg.result_nodes_pending = pending.size();
        cfg.result_nodes_examined = done.size();

        //report stats
        #ifdef ASTAR_STATS
        clock_t elapsed2 = clock()-time;
        std::cout << "astar stats\n";
        std::cout << "ticks:\t\t" << elapsed2 << " (" << elapsed1 << ", " << elapsed2-elapsed1 << ")\n";
        std::cout << "(seconds):\t" << (float)elapsed2/CLOCKS_PER_SEC << "\n";		
        std::cout << "route length:\t" << cfg.route_length << " nodes (" << cfg.route_cost << " units)\n";
        std::cout << "nodes examined:\t" << cfg.result_nodes_examined << "\n";
        std::cout << "nodes pending:\t" << cfg.result_nodes_pending << "\n";
        std::cout << "(total):\t" << cfg.result_nodes_opened << "\n";	
        #endif

        return complete;
    }//void astar(...);

}//namespace astar

namespace std
{
    template<typename T> struct hash< astar::node_link<T> >
    {
        std::size_t operator()(astar::node_link<T> const& node) const noexcept
        {
            return node.myNode.hash();
        }
    };
}

#endif


