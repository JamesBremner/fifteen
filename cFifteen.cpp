#include <iostream>
#include <fstream>
#include <stdio.h>      /* printf, scanf, puts, NULL */

#include <iomanip>
#include <algorithm>

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "cRunWatch.h"
#include "cFifteen.h"

using namespace std;

cFifteen::cFifteen()
{
    myfAnimate = false;
    myfInstrument = false;
}
cBox::cBox()
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
}

void cFifteen::Move( int tile, int dst,
                     std::vector<int>& dnd )
{
    //cout << "Move tile " << tile << " to spot " << dst << ", ";

    // do not disturb tiles that are already in position
    myBox.CostInit();
    for( int f : dnd )
    {
        if( myfInstrument )
            cout << " Fix" << f <<" ";
        myBox.Fix( f );
    }

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
            myBox. CostInit();
            for( int f : dnd )
                myBox.Fix( f );

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

    raven::set::cRunWatch aWatcher("Solve");

    mySolution.clear();

    try
    {
        vector<int> dnd;

        // tiles 1-2
        Move( 1, 0, dnd );
        Move( 2, 1, dnd );

        // tiles 3-4
        MoveOut( 4, dnd );
        Move( 3, 3, dnd );
        Move( 4, 7, dnd );
        Rotate( 2, 3, 7, dnd );

        // tiles 5-6
        Move( 5, 4, dnd );
        Move( 6, 5, dnd );

        // tiles 7-8
        MoveOut( 8, dnd );
        Move( 7, 7, dnd );
        Move( 8, 11, dnd );
        Rotate( 6, 7, 11, dnd );

        // tiles 9, 13
        MoveOut( 13, dnd );
        Move( 9, 12, dnd );
        Move( 13, 13, dnd );
        Rotate( 8, 12, 13, dnd );

        // tiles 10-14
        MoveOut( 14, dnd );
        Move( 10, 13, dnd );
        Move( 14, 14, dnd );
        Rotate( 9, 13, 14, dnd );

        // remaining tiles
        Move( 11, 10, dnd );

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

void cFifteen::Rotate( int dst, int s1, int s2,
                       std::vector<int>& dnd )
{
    for( int f = 0; f < dst; f++ )
        myBox.Fix( f );
    myBox.Fix( s1 );
    myBox.Fix( s2 );
    for( int j : Path( myBox.SpotFromTile( 0 ), dst ) )
        Slide( j );
    Slide( s1 );
    Slide( s2 );
    dnd[ dnd.size()-2 ] = dst;
    dnd[ dnd.size()-1 ] = s1;
}

void cFifteen::MoveOut( int tile, std::vector<int>& dnd )
{
    vector<int> tdnd = dnd;
    Move( tile, 15, tdnd );
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
    vector<int> path;
    vector<graph_t::vertex_descriptor> predecessors(boost::num_vertices(G));
    boost::dijkstra_shortest_paths(
        G, src,
        boost::weight_map(boost::get(&cEdge::myCost, G))
        .predecessor_map(boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index,G)))
    );

    graph_t::vertex_descriptor v = dst;
    for( auto u = predecessors[v]; u != v; v=u, u=predecessors[v])
    {
        auto edge_pair = boost::edge(u,v,G);
        path.push_back( boost::target(edge_pair.first, G) );
    }
    reverse( path.begin(), path.end() );
    return path;
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

