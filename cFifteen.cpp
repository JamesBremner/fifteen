#include <iostream>
#include <fstream>
#include <stdio.h>      /* printf, scanf, puts, NULL */

#include <iomanip>
#include <algorithm>

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/astar_search.hpp>

#include "cRunWatch.h"
#include "cFifteen.h"

using namespace std;

cFifteen::cFifteen()
{
    myfAnimate = false;
    myfInstrument = false;
}
cBox::cBox()
    : myfAStar( false )
{
    for( int kr = 0; kr<4; kr++ )
    {
        for( int kc =0; kc < 4; kc++ )
        {
            int i = NodeFromColRow( kc, kr );
            int j = NodeFromColRow( kc-1, kr );
            if( j >= 0 )
            {
                add_edge( i, j, G );
            }
            j = NodeFromColRow( kc+1, kr );
            if( j >= 0 )
            {
                add_edge( i, j, G );
            }
            j = NodeFromColRow( kc, kr-1 );
            if( j >= 0 )
            {
                add_edge( i, j, G );
            }
            j = NodeFromColRow( kc, kr+1 );
            if( j >= 0 )
            {
                add_edge( i, j, G );
            }
        }
    }
    for( int kr = 0; kr<4; kr++ )
    {
        for( int kc =0; kc < 4; kc++ )
        {
            G[ NodeFromColRow( kc, kr ) ].c = kc;
            G[ NodeFromColRow( kc, kr ) ].r = kr;
        }
    }
}

void cFifteen::Fix()
{
    myBox.CostInit();
    for( int f : myDND )
    {
        if( myfInstrument )
            cout << " Fix" << f <<" ";
        myBox.Fix( f );
    }
}

void cFifteen::Move( int tile, int dst )
{
    //cout << "Move tile " << tile << " to spot " << dst << ", ";

    // do not disturb tiles that are already in position
    Fix();

    // path to move tile
    if( myfInstrument )
        cout << "\nTile Path: ";
    vector<int> TilePath = Path( myBox.SpotFromTile( tile ), dst );

    try
    {
        // tile location
        int jt = myBox.SpotFromTile( tile );
        int jstart = jt;

        // while not at destination
        int count = -1;
        while( jt != dst )
        {
            if( count++ >= 50 )
                throw 1;

            // do not disturb tiles that are already in position
            Fix();

            // fix tile we are moving so that space path does not move it
            myBox.Fix( jt );

            // path to move space
            if( myfInstrument )
                cout << "\nSpace Path: ";
            vector<int> SpacePath = Path( myBox.SpotFromTile( 0 ), TilePath[count] );

            // move space
            for( int j : SpacePath )
                Slide( j );

            // move tile
            Slide( jt );

            // new tile location
            jt = myBox.SpotFromTile( tile );

            // check that we are making progress
            if( jt == jstart )
                throw 2;
        }

        // the tile is now in position
        // prevent moving it again
        myDND.push_back( jt );

    }
    catch( int e )
    {
        cout << "\n";
        Text();
        stringstream ss;
        ss << "Move tile " << tile << " to spot " << dst << " failed";
        throw std::runtime_error( ss.str());
    }
}

bool cFifteen::Solve()
{
    cout << "Solve\n";

    raven::set::cRunWatch aWatcher("Solve");

    mySolution.clear();
    myDND.clear();

    try
    {
        // tiles 1-2
        Move( 1, 0 );
        Move( 2, 1 );

        // tiles 3-4
        MoveOut( 4 );
        Move( 3, 3 );
        Move( 4, 7 );
        Rotate( 2, 3, 7 );

        // tiles 5-6
        Move( 5, 4 );
        Move( 6, 5 );

        // tiles 7-8
        MoveOut( 8 );
        Move( 7, 7 );
        Move( 8, 11 );
        Rotate( 6, 7, 11 );

        // tiles 9, 13
        MoveOut( 13 );
        Move( 9, 12 );
        Move( 13, 13 );
        Rotate( 8, 12, 13 );

        // tiles 10-14
        MoveOut( 14 );
        Move( 10, 13 );
        Move( 14, 14 );
        Rotate( 9, 13, 14 );

        // remaining tiles
        Move( 11, 10 );

        int loc12 = myBox.SpotFromTile( 12 );

        if( loc12 == 15 )
        {
            Slide( 15 );
        }
        if( loc12 == 14 )
            throw runtime_error("\nUnsolveable");

        if( myfAnimate || myfInstrument)
        {
            cout << "\n";
            Text();
            cout << "================================================\n";
        }
        return true;
    }
    catch( runtime_error& e )
    {
        cout << e.what() << "\n";
        cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
        return false;
    }

}

void cFifteen::Rotate( int dst, int s1, int s2 )
{
    for( int f = 0; f < dst; f++ )
        myBox.Fix( f );
    myBox.Fix( s1 );
    myBox.Fix( s2 );
    for( int j : Path( myBox.SpotFromTile( 0 ), dst ) )
        Slide( j );
    Slide( s1 );
    Slide( s2 );
    myDND[ myDND.size()-2 ] = dst;
    myDND[ myDND.size()-1 ] = s1;
}

void cFifteen::MoveOut( int tile )
{
    vector<int> tdnd = myDND;
    Move( tile, 15 );
    myDND = tdnd;
}

vector<int> cFifteen::Path( int src, int dst )
{
    //cout << "Path " << src <<" "<< dst << "\n";
    vector<int> path;
    if( src == dst )
        return path;

    path = myBox.Path( src, dst );

    if( ! path.size() )
        throw std::runtime_error( "no path" );
    else if( myfInstrument )
    {
        for( int i : path )
            cout << i << "->";
        cout << "\n";
    }

    return path;
}
vector<int> cBox::Path( int src, int dst )
{
    if( myfAStar )
        return PathAStar( src, dst );
    return PathDijsktra( src, dst );
}
vector<int> cBox::PathDijsktra( int src, int dst )
{

    // construct vector to store path from src to dst
    vector<int> path;

    // construct storage for path from src to every spot
    // which is what the boost dijskstra algorithm reurns
    vector<graph_t::vertex_descriptor> predecessors(boost::num_vertices(G));
    auto it = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index,G));

    // construct storage for edge costs
    // ( prevents previously arranged tiles from moving )
    auto W = boost::weight_map(boost::get(&cEdge::myCost, G));

    // run dijsktra algorithm
    boost::dijkstra_shortest_paths(
        G, src,
        W.predecessor_map(it)
    );

    // pick out path to dst
    int v = dst;
    for( auto u = predecessors[v]; u != v; v=u, u=predecessors[v])
    {
        path.push_back( boost::target(boost::edge(u,v,G).first, G) );
    }
    reverse( path.begin(), path.end() );

    return path;
}






vector<int> cBox::PathAStar( int src, int dst )
{
    // construct vector to store path from src to dst
    vector<int> path;

    // construct storage for path from src to every spot
    // which is what the boost dijskstra algorithm returns
    vector<graph_t::vertex_descriptor> predecessors(boost::num_vertices(G));
    auto it = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index,G));

    // distance heuristic
    class distance_heuristic : public boost::astar_heuristic<graph_t, int>
    {
    public:
        distance_heuristic(graph_t& g, int goal)
            : G(g), m_goal(goal) {}
        int operator()(int u)
        {
            return abs(G[m_goal].c - G[u].c) + abs(G[m_goal].r - G[u].r);
        }
    private:
        graph_t& G;
        Vertex m_goal;
    };

    // construct storage for edge costs
    // ( prevents previously arranged tiles from moving )
    auto W = boost::weight_map(boost::get(&cEdge::myCost, G));

    // visitor that terminates search when we find the goal
    struct found_goal {}; // exception for termination
    class astar_goal_visitor : public boost::default_astar_visitor
    {
    public:
        astar_goal_visitor(int goal )
            : m_goal(goal)
        {}
        void examine_vertex(int u, const graph_t& g)
        {
            if(u == m_goal)
                throw found_goal();
        }
    private:
        int m_goal;
    };
    astar_goal_visitor V( dst );

    try
    {
        // call astar named parameter interface
        boost::astar_search_tree(
            G, src,
            distance_heuristic(G, dst),
            W.
            predecessor_map(it).
            visitor( V ));
    }
    catch(found_goal fg)     // found a path to the goal
    {
        // pick out path to dst
        int v = dst;
        for( auto u = predecessors[v]; u != v; v=u, u=predecessors[v])
        {
            path.push_back( boost::target(boost::edge(u,v,G).first, G) );
        }
        reverse( path.begin(), path.end() );

        return path;
    }
}
void cBox::CostInit()
{
    boost::graph_traits<graph_t>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    {
        //cout <<  boost::source(*ei, G ) <<" "<< boost::target(*ei, G ) << " set cost 1\n";

        G[*ei].myCost = 1;
    }
}

void cBox::Fix( int j )
{
    boost::graph_traits<graph_t>::out_edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = out_edges(j, G);
            ei != ei_end; ++ei)
    {
        //cout <<  boost::source(*ei, G ) <<" "<< boost::target(*ei, G ) << " set cost 100\n";
        G[*ei].myCost = 100;
    }
}

void cFifteen::Slide( int jc )
{
    myBox.Slide( jc );

    if( myfAnimate )
    {
        cout << "Slide " << jc << "\n";
        Text();
    }
    mySolution.push_back( jc );
}

void cBox::Slide( int spot )
{
    cSpot cr = ColRowFromNode( spot );
    int j0 = SpotFromTile( 0 );
    cSpot cr0 = ColRowFromNode( j0 );
    int dx = cr0.r - cr.r;
    int dy = cr0.c - cr.c;
    if( ((dx == 1 || dx == -1 ) && dy == 0 ) ||
            (( dy == 1 || dy == -1) && dx == 0 ) )
    {
        G[ j0 ].myTile = G[ spot ].myTile;
        G[ spot ].myTile = 0;
    }
}

int cBox::SpotFromTile( int t )
{
    for ( auto vp = vertices(G); vp.first != vp.second; ++vp.first)
        if( G[*vp.first].myTile == t )
            return (int) *vp.first;
}

int cBox::NodeFromColRow( int r, int c )
{
    int ret;
    if( r < 0 || r > 3 || c < 0 || c > 3 )
        ret = -1;
    else
        ret = r*4+c;
    //cout << "NCR  "<< r <<" "<< c <<" " << ret << "\n";
    return ret;
}

string cBox::Text()
{
    stringstream ss;
    for( int y = 0; y < 4; y++ )
    {
        for( int x = 0; x < 4; x++  )
        {
            int t = G[ NodeFromColRow( y, x ) ].myTile;
            if( t )
                ss <<setw(3) << t;
            else
                ss << "   ";
        }
        ss << "\n";
    }
    ss << "\n";
    return ss.str();
}

bool cFifteen::IsSolveable()
{
    //Text();
    int zr = myBox.ColRowFromNode( myBox.SpotFromTile( 0 ) ).r;
    int ic = InvCount();
    // cout << "IsSolveable "<< ic << " " << zr << "\n";
    if( ic % 2 )
    {
        if( zr == 2 || zr == 0 )
            return true;
    }
    else
    {
        if( zr == 3 || zr == 1 )
            return true;
    }
    return false;
}

int cFifteen::InvCount()
{
    int inv_count = 0;
    for (int i = 0; i < 16; i++)
    {
        for (int j = i + 1; j < 16; j++)
        {
            int ti = myBox.Tile( i );
            int tj = myBox.Tile( j );
            if( ti == 0 || tj == 0 )
                continue;
            if( ti > tj )
                inv_count++;
            //cout << i <<" "<< j <<" "<< ti <<" "<< tj <<" "<< inv_count << "\n";
        }
    }
    return inv_count;
}

void cFifteen::Random()
{
    bool Solveable = false;
    while( ! Solveable )
    {
        for( int n = 0; n < 16; n++ )
            myBox.Tile( n, 0);
        for( int tile = 1; tile <= 15; tile++ )
        {
            while( 1 )
            {
                int n = rand() % 16;
                if( myBox.Tile( n ) == 0 )
                {
                    myBox.Tile( n, tile );
                    break;
                }
            }
        }
        Solveable = IsSolveable();
    }
//    cout << "Random generated\n";
//    Text();
}

void cFifteen::Read( const string& fname )
{
    ifstream f( fname );
    if( ! f.is_open() )
    {
        cout << "Cannot read " << fname << "\n";
        exit(1);
    }

    int tile;
    for( int k = 0; k < 16; k++ )
    {
        f >> tile;
        myBox.Tile( k, tile );
    }
    if( ! IsSolveable() )
        throw runtime_error("Not solveable");
}

