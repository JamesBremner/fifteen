#include <iostream>
#include <fstream>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iomanip>
#include <algorithm>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace std;

class cVertex
{
public:
    int myTile;
};
class cEdge
{
public:
    int myCost;
};
class cFifteen
{
    // represent the fifteen puzzle box and tiles with a graph
    typedef boost::adjacency_list
    <
    boost::listS,
          boost::vecS,
          boost::bidirectionalS,
          cVertex,
          cEdge > graph_t;

    graph_t myGB;

    vector<int> mySolution;

    bool myfAnimate;
    bool myfInstrument;

public:

    /// CTOR, link spots that tile can slide between ( orthogonal )
    cFifteen();

    /// Assign tiles randomly
    void Random();

    /// Assign tiles from file
    void Read( const string& fname );

    /// Display puzzle
    void Text();

    /** Solve puzzle using method described in http://www.chessandpoker.com/fifteen-puzzle-solution.html
        @return true if solved
    */
    bool Solve();

    /** Click on tile, slide to adjacent empty spot
        param[in] index of tile spot, 0 to 15
    */
    void Click( int jc );

    void Animate()
    {
        myfAnimate = true;
    }
    void Instrument()
    {
        myfInstrument = true;
    }
private:

    /** spot index from row, column
        @param[in] r row
        @param[in] c col
        @return spot index, -1 if not valid
    */
    int NodeFromColRow( int r, int c )
    {
        int ret;
        if( r < 0 || r > 3 || c < 0 || c > 3 )
            ret = -1;
        else
            ret = r*4+c;
        //cout << "NCR  "<< r <<" "<< c <<" " << ret << "\n";
        return ret;
    }

    /// spot index from row, column pair
    int NodeFromColRow( std::pair<int,int> cr )
    {
        return NodeFromColRow( cr.second, cr.first );
    }

    /// spot index where tile is at
    int NodeFromTile( int t );

    /// row, col from spot index
    std::pair<int,int> ColRowFromNode( int j )
    {
        return make_pair( j % 4, j / 4 );
    }

    /// set all sliding costs to 1
    void CostInit();

    /// set sliding costs so tile at spot index will not be moved
    void Fix( int j );

    /// path from src to dst, using dijsktra algorithm
    vector<int> Path( int src, int dst );

    /** Move tile
        @param[in] tile
        @param[in] dst spot index where tile should move to
        @param[in/out] dnd spots that must not be disturbed containing previouly arranged tiles
    */
    void Move( int tile, int dst,
               vector<int>& dnd );
};

cFifteen::cFifteen()
{
    myfAnimate = false;
    myfInstrument = false;

    for( int kr = 0; kr<4; kr++ )
    {
        for( int kc =0; kc < 4; kc++ )
        {
            int i = NodeFromColRow( kc, kr );
            int j = NodeFromColRow( kc-1, kr );
            if( j >= 0 )
            {
                add_edge( i, j, myGB );
            }
            j = NodeFromColRow( kc+1, kr );
            if( j >= 0 )
            {
                add_edge( i, j, myGB );
            }
            j = NodeFromColRow( kc, kr-1 );
            if( j >= 0 )
            {
                add_edge( i, j, myGB );
            }
            j = NodeFromColRow( kc, kr+1 );
            if( j >= 0 )
            {
                add_edge( i, j, myGB );
            }
        }
    }
}

void cFifteen::Move( int tile, int dst,
                     vector<int>& dnd )
{
    cout << "Move tile " << tile << " to spot " << dst << ", ";

    try
    {
        // tile location
        int jt = NodeFromTile( tile );

        // while not at destination
        int count = 0;
        while( jt != dst )
        {
            if( count++ >= 50 )
                throw 1;

            // do not disturb tiles that are already in position
            CostInit();
            for( int f : dnd )
                Fix( f );

            // fix tile we are moving so that space path does not move it
            Fix( jt );

            // path to move tile
            vector<int> TilePath = Path( jt, dst );

            // path to move space
            vector<int> SpacePath = Path( NodeFromTile( 0 ), TilePath[0] );

            // move space
            for( int j : SpacePath )
                Click( j );

            // move tile
            Click( jt );

            // new tile location
            jt = NodeFromTile( tile );
        }

        // the tile is now in position
        // prevent moving it again
        dnd.push_back( jt );

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

    mySolution.clear();

    try
    {
        vector<int> dnd;
        Move( 1, 0, dnd );
        Move( 2, 1, dnd );
        Move( 3, 3, dnd );
        Move( 4, 7, dnd );

        CostInit();
        Fix( 0 );
        Fix( 1 );
        Fix( 3 );
        Fix( 7 );
        for( int j : Path( NodeFromTile( 0 ), 2 ) )
            Click( j );
        Click( 3 );
        Click( 7 );
        dnd[ dnd.size()-2 ] = 2;
        dnd[ dnd.size()-1 ] = 3;

        Move( 5, 4, dnd );
        Move( 6, 5, dnd );
        Move( 7, 7, dnd );
        Move( 8, 11, dnd );

        CostInit();
        Fix( 0 );
        Fix( 1 );
        Fix( 2 );
        Fix( 3 );
        Fix( 4 );
        Fix( 5 );
        Fix( 7 );
        Fix( 11 );
        for( int j : Path( NodeFromTile( 0 ), 6 ) )
            Click( j );
        Click( 7 );
        Click( 11 );
        dnd[ dnd.size()-2 ] = 6;
        dnd[ dnd.size()-1 ] = 7;

        Move( 9, 12, dnd );
        Move( 13, 13, dnd );

        CostInit();
        Fix( 0 );
        Fix( 1 );
        Fix( 2 );
        Fix( 3 );
        Fix( 4 );
        Fix( 5 );
        Fix( 6 );
        Fix( 7 );
        Fix( 12 );
        Fix( 13 );
        for( int j : Path( NodeFromTile( 0 ), 8 ) )
            Click( j );
        Click( 12 );
        Click( 13 );
        dnd[ dnd.size()-2 ] = 8;
        dnd[ dnd.size()-1 ] = 12;

        Move( 10, 13, dnd );
        Move( 14, 14, dnd );

        CostInit();
        Fix( 0 );
        Fix( 1 );
        Fix( 2 );
        Fix( 3 );
        Fix( 4 );
        Fix( 5 );
        Fix( 6 );
        Fix( 7 );
        Fix( 8 );
        Fix( 12 );
        Fix( 13 );
        Fix( 14 );
        for( int j : Path( NodeFromTile( 0 ), 9 ) )
            Click( j );
        Click( 13 );
        Click( 14 );
        dnd[ dnd.size()-2 ] = 9;
        dnd[ dnd.size()-1 ] = 13;

        Move( 11, 10, dnd );

        int loc12 = NodeFromTile( 12 );
        cout << "loc12 " << loc12 << "\n";

        if( loc12 == 15 )
        {
            Click( 15 );
        }
        if( loc12 == 14 )
            throw runtime_error("\nUnsolveable");

        cout << "\n";
        Text();
    }
    catch( runtime_error& e )
    {
        cout << e.what() << "\n";
        return false;
    }

}

vector<int> cFifteen::Path( int src, int dst )
{
    //cout << "Path " << src <<" "<< dst << "\n";
    vector<int> path;
    if( src == dst )
        return path;

    auto weights = boost::get(&cEdge::myCost, myGB );
    vector<graph_t::vertex_descriptor> predecessors(boost::num_vertices(myGB));
    boost::dijkstra_shortest_paths(
        myGB, src,
        boost::weight_map(boost::get(&cEdge::myCost, myGB))
        .predecessor_map(boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index,myGB)))
    );

    graph_t::vertex_descriptor v = dst;
    for( auto u = predecessors[v]; u != v; v=u, u=predecessors[v])
    {
        auto edge_pair = boost::edge(u,v,myGB);
        path.push_back( boost::target(edge_pair.first, myGB) );
    }
    reverse( path.begin(), path.end() );

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

void cFifteen::CostInit()
{
    boost::graph_traits<graph_t>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(myGB); ei != ei_end; ++ei)
    {
        //cout <<  boost::source(*ei, myGB ) <<" "<< boost::target(*ei, myGB ) << " set cost 1\n";

        if(  ColRowFromNode(boost::source(*ei, myGB)).second == 0 ||
          ColRowFromNode(boost::target(*ei, myGB)).second == 0 )
        myGB[*ei].myCost = 10;
        else
        myGB[*ei].myCost = 1;
    }
}

void cFifteen::Fix( int j )
{
    if( myfInstrument)
        cout << "Fix" << j << " ";

    boost::graph_traits<graph_t>::out_edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = out_edges(j, myGB);
            ei != ei_end; ++ei)
    {
        //cout <<  boost::source(*ei, myGB ) <<" "<< boost::target(*ei, myGB ) << " set cost 100\n";
        myGB[*ei].myCost = 100;
    }
}

void cFifteen::Click( int jc )
{
    std::pair<int,int> cr = ColRowFromNode( jc );
    int j0 = NodeFromTile( 0 );
    std::pair<int,int> cr0 = ColRowFromNode( j0 );
    int dx = cr0.first - cr.first;
    int dy = cr0.second - cr.second;
    if( ((dx == 1 || dx == -1 ) && dy == 0 ) ||
            (( dy == 1 || dy == -1) && dx == 0 ) )
    {
        myGB[ j0 ].myTile = myGB[ jc ].myTile;
        myGB[ jc ].myTile = 0;
    }
    if( myfAnimate ) {
        cout << "Click " << jc << "\n";
        Text();
    }
    mySolution.push_back( jc );
}

int cFifteen::NodeFromTile( int t )
{
    for ( auto vp = vertices(myGB); vp.first != vp.second; ++vp.first)
        if( myGB[*vp.first].myTile == t )
            return (int) *vp.first;
}

void cFifteen::Text()
{
    for( int y = 0; y < 4; y++ )
    {
        for( int x = 0; x < 4; x++  )
        {
            int t = myGB[ NodeFromColRow( y, x ) ].myTile;
            if( t )
                cout <<setw(3) << t;
            else
                cout << "   ";
        }
        cout << "\n";
    }
    cout << "\n";

}

void cFifteen::Random()
{
    /* initialize random seed: */
    srand (time(NULL));

    for( int n = 0; n < 16; n++ )
        myGB[ n ].myTile = 0;

    for( int tile = 1; tile <= 15; tile++ )
    {
        while( 1 )
        {
            int n = rand() % 16;
            if( myGB[ n ].myTile == 0 )
            {
                myGB[ n ].myTile = tile;
                break;
            }
        }
    }
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
        myGB[ k ].myTile = tile;
    }

}


int main( int argc, char* argv[] )
{
    cFifteen F;
    if( argc == 1 )
        F.Random();
    else
    {
        F.Read( argv[1] );
        F.Animate();
        F.Instrument();
    }
    F.Text();
    F.Solve();
    return 0;
}
