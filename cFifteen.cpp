#include <iostream>
#include <fstream>
#include <stdio.h>      /* printf, scanf, puts, NULL */

#include <iomanip>
#include <algorithm>

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "cFifteen.h"

using namespace std;

cFifteen::cFifteen()
{
    myfAnimate = false;
    myfInstrument = false;
}
cBox::cBox() {
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
                     vector<int>& dnd )
{
    //cout << "Move tile " << tile << " to spot " << dst << ", ";

    // do not disturb tiles that are already in position
    CostInit();
    for( int f : dnd )
        Fix( f );

    // path to move tile
    if( myfInstrument )
        cout << "\nTile Path: ";
    vector<int> TilePath = Path( NodeFromTile( tile ), dst );

    try
    {
        // tile location
        int jt = NodeFromTile( tile );
        int jstart = jt;

        // while not at destination
        int count = -1;
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

            // path to move space
            if( myfInstrument )
                cout << "\nSpace Path: ";
            vector<int> SpacePath = Path( NodeFromTile( 0 ), TilePath[count] );

            // move space
            for( int j : SpacePath )
                Click( j );

            // move tile
            Click( jt );

            // new tile location
            jt = NodeFromTile( tile );

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

    mySolution.clear();

    try
    {
        vector<int> dnd;
        Move( 1, 0, dnd );
        Move( 2, 1, dnd );

        MoveOut( 4, dnd );
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

        MoveOut( 8, dnd );
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

        MoveOut( 13, dnd );
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

        MoveOut( 14, dnd );
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

        if( loc12 == 15 )
        {
            Click( 15 );
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

void cFifteen::MoveOut( int tile, vector<int>& dnd )
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

    myBox.Path( src, dst );

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

void cFifteen::CostInit()
{
    boost::graph_traits<graph_t>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    {
        //cout <<  boost::source(*ei, G ) <<" "<< boost::target(*ei, G ) << " set cost 1\n";

        if(  ColRowFromNode(boost::source(*ei, G)).r == 0 ||
                ColRowFromNode(boost::target(*ei, G)).r == 0 )
            G[*ei].myCost = 10;
        else
            G[*ei].myCost = 1;
    }
}

void cFifteen::Fix( int j )
{
    if( myfInstrument)
        cout << "Fix" << j << " ";

    boost::graph_traits<graph_t>::out_edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = out_edges(j, G);
            ei != ei_end; ++ei)
    {
        //cout <<  boost::source(*ei, G ) <<" "<< boost::target(*ei, G ) << " set cost 100\n";
        G[*ei].myCost = 100;
    }
}

void cFifteen::Click( int jc )
{
    myBox.Click( jc );

    if( myfAnimate )
    {
        cout << "Click " << jc << "\n";
        Text();
    }
    mySolution.push_back( jc );
}

void cBox::Click( int spot )
{
    cSpot cr = ColRowFromNode( spot );
    int j0 = NodeFromTile( 0 );
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
    int zr = ColRowFromNode( NodeFromTile( 0 ) ).r;
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

