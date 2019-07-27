# Fifteen
 Solve fifteen puzzle


The World-famous Fifteen Puzzle has stumped curious gamers since the 18th century.
Invented by Noyes Chapman, the Fifteen Puzzle is comprised of 15 numbered puzzle pieces
contained in a small 4x4 box that allows each of the numbers to slide around freely
based on the fact that there is always an empty space somewhere on the puzzle board.

The goal of the Fifteen puzzle is to arrange the pieces so that
the top row reads 1-2-3-4 (from left to right),
the second row 5-6-7-8, the third row 9-10-11-12
and the bottom row 13-14-15 with the space in the lower right-hand corner.

 http://www.chessandpoker.com/fifteen-puzzle-solution.html
 
 ## Performance
 
 The algorithm used to search for paths to slide a tile to a new spot is, optionally, Dijsktra or A*.  The A* gives slightly better performance becasue the path searching is guided by a heuristic and terminates as soon as a path is discovered.
 

 ```
 Mean of solving 100 randomly generated solveable initial tile arrangements on a i5-6400 CPU @ 2.70 GHz
 
 Dijsktra    2.1 millisecs
 
 A*          1.7 millisecs
 ```

