#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "cFifteen.h"
using namespace std;

int main( int argc, char* argv[] )
{
    cFifteen F;
    if( argc == 1 )
    {
        /* generate and solve random puzzles */
        srand (time(NULL));
        int count = 0;
        int success = 0;
        for( int k=0; k < 100; k++ )
        {
            // genrate a random solveable 15 puzzle initial arrangement
            F.Random();

            // display
            F.Text();

            // solve
            if( F.Solve() )
                success++;
            count++;
        }
        cout <<"success " << success << " / " << count << "\n";
    }
    else
    {
        // read initial arrangement from file
        F.Read( argv[1] );

        // display board as we go along
        F.Animate();

        //F.Instrument();

        // display initial position
        F.Text();

        // Solve
        F.Solve();
    }

    return 0;
}
