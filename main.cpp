#include <iostream>
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

public:

    /// CTOR, link spots that tile can slide between ( orthogonal )
    cFifteen();

    /// Assign tiles randomly
    void Random();

    /// Display puzzle
    void Text();

    /// Solve puzzle
    void Solve();

    /** Click on tile, slide to adjacent empty spot
        param[in] index of tile spot, 0 to 15
    */
    void Click( int jc );

private:

    /// spot index from row, column
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

    /// path between spots, using dijsktra algorithm
    vector<int> Path(int src, int dst);
};

cFifteen::cFifteen()
{
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

void cFifteen::Solve()
{
    cout << "Solve\n";
    Text();

    int j1 = NodeFromTile( 1 );
    while( 1 )
    {
        std::pair<int,int> cr = ColRowFromNode( j1 );
        cout << j1 <<" "<< cr.first <<" "<< cr.second << "\n";
        if( cr.first )
            cr.first -= 1;
        else if( cr.second )
            cr.second -= 1;
        else
            break;
        int jtarget = NodeFromColRow( cr );
        cout << "target " << jtarget << "\n";
        CostInit();
        Fix( j1 );
        int j0 = NodeFromTile( 0 );

        vector<int> path = Path( j0, jtarget );
        for( int i : path )
        {
            Click( i );
        }
        Click( j1 );
        if( jtarget == 0 )
            break;
        j1 = jtarget;
    }

    cout << " *** 2 ***\n";
    j1 = NodeFromTile( 2 );
    while( 1 )
    {
        std::pair<int,int> cr = ColRowFromNode( j1 );
        cout << j1 <<" "<< cr.first <<" "<< cr.second << "\n";
        if( cr.first == 1 && cr.second == 0 )
            break;
        if( cr.first != 1 )
            if( cr.first == 0 )
                cr.first = 1;
            else
                cr.first -= 1;
        else
            cr.second -= 1;
                int jtarget = NodeFromColRow( cr );
        cout << "target " << jtarget << "\n";
        CostInit();
        Fix( 0 );
        Fix( j1 );
        int j0 = NodeFromTile( 0 );

        vector<int> path = Path( j0, jtarget );
        for( int i : path )
        {
            Click( i );
        }
        Click( j1 );
        if( jtarget == 0 )
            return;
        j1 = jtarget;
    }
}

vector<int> cFifteen::Path( int src, int dst )
{
    cout << "Path " << src <<" "<< dst << "\n";
    auto weights = boost::get(&cEdge::myCost, myGB );
    vector<graph_t::vertex_descriptor> predecessors(boost::num_vertices(myGB));
    boost::dijkstra_shortest_paths(myGB, src,
                                   boost::weight_map(boost::get(&cEdge::myCost, myGB))
                                   .predecessor_map(boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index,myGB)))
                                  );

    vector<int> path;
    graph_t::vertex_descriptor v = dst;
    for( auto u = predecessors[v]; u != v; v=u, u=predecessors[v])
    {
        auto edge_pair = boost::edge(u,v,myGB);
        path.push_back( boost::target(edge_pair.first, myGB) );
    }
    reverse( path.begin(), path.end() );

    if( ! path.size() )
        cout << "no path\n";
    else
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
        myGB[*ei].myCost = 1;
    }
}

void cFifteen::Fix( int j )
{
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
    Text();
}
//void cFifteen::MoveSpace( int tx, int ty )
//{
//    cout << "MoveSpace " << tx <<" " << ty << "\n";
//
//    std::pair<int,int> l0 = find( 0 );
//    int dx = l0.first - tx;
//    while( dx != 0 )
//    {
//        int d1 = 1;
//        if( dx > 0 )
//            d1 = -1;
//        Click( l0.first+d1, l0.second );
//        l0 = find( 0 );
//        dx = l0.first - tx;
//    }
//    int dy = l0.second - ty;
//    while( dy != 0 )
//    {
//        int d1 = 1;
//        if( dy > 0 )
//            d1 = -1;
//        Click( l0.first, l0.second+d1 );
//        l0 = find( 0 );
//        dy = l0.second - ty;
//    }
//}

//std::pair<int,int> cFifteen::find( int t )
//{
//    for( int y = 0; y < 4; y++ )
//    {
//        for( int x = 0; x < 4; x++  )
//        {
//            if( myBoard[x][y] == t )
//            {
//                return make_pair( x, y );
//            }
//        }
//    }
//}

int cFifteen::NodeFromTile( int t )
{
    for ( auto vp = vertices(myGB); vp.first != vp.second; ++vp.first)
        if( myGB[*vp.first].myTile == t )
            return (int) *vp.first;
}

void cFifteen::Text()
{
//    for( int y = 0; y < 4; y++ )
//    {
//        for( int x = 0; x < 4; x++  )
//        {
//            int t = myBoard[x][y];
//            if( t )
//                cout <<setw(3) << t;
//            else
//                cout << "   ";
//        }
//        cout << "\n";
//    }
//    cout << "\n";

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


int main()
{
    cFifteen F;
    F.Random();
    F.Text();
    F.Solve();
    return 0;
}
