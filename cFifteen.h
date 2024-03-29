#include <boost/graph/adjacency_list.hpp>

/// A spot in the puzzle box which can be occupied by a tile, or not
class cSpot
{
public:
    int c;              // column in grid, 0-based
    int r;              // row in grid, 0-based
    int myTile;         // Tile occupying spot, 1-based with 0 for empty
};

/// The orthogonal connection between two spots along which a tile can be slid
class cEdge
{
public:
    int myCost;
};

/// Puzzle box containing sliding tiles
class cBox
{
public:

    cBox();

    void UseAStar( bool f = true )
    {
        myfAStar = f;
    }

    /// set all sliding costs to 1
    void CostInit();

    /// set sliding costs so tile at spot index will not be moved
    void Fix( int spot );

    /// slide tile in spot to adjacent space
    void Slide( int spot );

    /** spot index from row, column
    @param[in] r row
    @param[in] c col
    @return spot index, -1 if not valid
    */
    int NodeFromColRow( int r, int c );

    /// spot index
    int NodeFromColRow( cSpot& s )
    {
        return NodeFromColRow( s.r, s.c );
    }

//    /// row, col from spot index
//    cSpot ColRowFromNode( int j )
//    {
//        cSpot s;
//        s.c = j%4;
//        s.r = j/4;
////        s.c = G[j].r;
////        s.r = G[j].c;
////        if( s.c != j%4 || s.r != j/4 )
////        {
////            std::cout << "ColRowFromNode " << j <<" "<< s.c <<" "<< s.r << "\n";
////            exit(1);
////        }
//        return s;
//    }

    /// find spot index that holds tile
    int SpotFromTile( int t );

    /// set tile in spot
    void Tile( int spot, int tile )
    {
        G[spot].myTile = tile;
    }

    /// get tile in spot
    int Tile( int spot )
    {
        return G[spot].myTile;
    }

    /** path from src to dst, using specified search algorithm algorithm
        @param[in] src
        @param[in] dst
        @return vector of spots to be visited
    */
    std::vector<int> Path( int src, int dst );


    /// Human readable string to dislay tile layout in box
    std::string Text();

private:
    /// represent the fifteen puzzle box and tiles with a boost graph
    typedef boost::adjacency_list
    <
    boost::listS,
          boost::vecS,
          boost::bidirectionalS,
          cSpot,
          cEdge > graph_t;

    graph_t G;

    bool myfAStar;                  /// true if using AStar algorithm

    /// Find path using Dijsktra
    std::vector<int> PathAStar( int src, int dst );

    /// Find path using AStar
    std::vector<int> PathDijsktra( int src, int dst );
};

/** Solve "Fifteen" puzzles

The World-famous Fifteen Puzzle has stumped curious gamers since the 18th century.
Invented by Noyes Chapman, the Fifteen Puzzle is comprised of 15 numbered puzzle pieces
contained in a small 4x4 box that allows each of the numbers to slide around freely
based on the fact that there is always an empty space somewhere on the puzzle board.

The goal of the Fifteen puzzle is to arrange the pieces so that
the top row reads 1-2-3-4 (from left to right),
the second row 5-6-7-8, the third row 9-10-11-12
and the bottom row 13-14-15 with the space in the lower right-hand corner.

 http://www.chessandpoker.com/fifteen-puzzle-solution.html

 */
class cFifteen
{
public:

    /// CTOR, link spots that tile can slide between ( orthogonal )
    cFifteen();

    void UseAStar( bool f = true )
    {
        myBox.UseAStar( f );
    }

    /// Generate a random solveable initial tile arrangement
    void Random();

    /// Assign initial tile arrangement from file
    void Read( const std::string& fname );

    /** Solve puzzle using method described in http://www.chessandpoker.com/fifteen-puzzle-solution.html
        @return true if solved

        The list of tiles that need to be slid are stored in attribute mySolution.
    */
    bool Solve();

    /** Click on tile, slide to adjacent empty spot
        param[in] index of tile spot, 0 to 15
    */
    void Slide( int jc );

    /// Switch on display of puzzle at each step
    void Animate()
    {
        myfAnimate = true;
    }

    /// Switch on debug instrumentation display
    void Instrument()
    {
        myfInstrument = true;
    }

    /// Display puzzle
    void Text()
    {
        std::cout << myBox.Text();
    }

private:
    cBox myBox;                     /// Puzzle box with sliding tiles
    std::vector<int> mySolution;     /// The slides needed to solve the puzzle
    std::vector<int> myDND;

    bool myfAnimate;        /// true if puzzled should be displayed at every step
    bool myfInstrument;     /// true if debugging instrumentation should be displayed

    /** path from src to dst, using dijsktra algorithm
        @param[in] src
        @param[in] dst
        @return vector of spots to be visited
    */
    std::vector<int> Path( int src, int dst );

    /** Move tile
        @param[in] tile
        @param[in] dst spot index where tile should move to
    */
    void Move( int tile, int dst );

    /** Move tile temporarily out of the way
        @param[in] tile
    */
    void MoveOut( int tile );

    /** Rotate tiles
        @param[in] dst move tile at s1 to dst
        @param[in] s1  move tile at s2 to s1
        @param[in] s2  move to s1

        Move tile at s1 to dst, tile as s2 to s1
    */
    void Rotate( int dst, int s1, int s2 );

    /// true if solveable https://www.geeksforgeeks.org/check-instance-15-puzzle-solvable/
    bool IsSolveable();
    int InvCount();

    /// prevent tiles in DND from moving
    void Fix();
};

