#include <boost/graph/adjacency_list.hpp>
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
    /// represent the fifteen puzzle box and tiles with a boost graph
    typedef boost::adjacency_list
    <
    boost::listS,
          boost::vecS,
          boost::bidirectionalS,
          cVertex,
          cEdge > graph_t;

    graph_t myGB;

    std::vector<int> mySolution;     /// The clicks needed to solve the puzzle

    bool myfAnimate;        /// true if puzzled should be displayed at every step
    bool myfInstrument;     /// true if debugging instrumentation should be displayed

public:

    /// CTOR, link spots that tile can slide between ( orthogonal )
    cFifteen();

    /// Generate a random solveable initial tile arrangement
    void Random();

    /// Assign tiles from file
    void Read( const std::string& fname );

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
        return std::make_pair( j % 4, j / 4 );
    }

    /// set all sliding costs to 1
    void CostInit();

    /// set sliding costs so tile at spot index will not be moved
    void Fix( int j );

    /** path from src to dst, using dijsktra algorithm
        @param[in] src
        @param[in] dst
        @return vector of spots to be visited

        Uses the dijsktra algorithm implemented by boost::graph

        An A* would be faster for large puzzles,
        but dijsktra is plenty fast for the 15 puzzle.
    */
    std::vector<int> Path( int src, int dst );

    /** Move tile
        @param[in] tile
        @param[in] dst spot index where tile should move to
        @param[in/out] dnd spots that must not be disturbed containing previouly arranged tiles
    */
    void Move( int tile, int dst,
               std::vector<int>& dnd );

    /** Move tile temporarily out of the way
        @param[in] tile
        @param[in] dnd spots that must not be disturbed containing previouly arranged tiles
    */
    void MoveOut( int tile, std::vector<int>& dnd );

    /// true if solveable https://www.geeksforgeeks.org/check-instance-15-puzzle-solvable/
    bool IsSolveable();
    int InvCount();
};

