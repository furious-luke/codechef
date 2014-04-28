#include <cstdlib>
#include <cassert>
#include <iostream>

static const unsigned max_N = 1000;
static const unsigned mod = 1000000007;

void
make_table( unsigned* tbl )
{
   // Only need to store one level of the permutations
   // at max due to the nature of the recursion.
   unsigned long long L[max_N + 1];

   // Prepare lower two levels.
   tbl[1] = 1;
   tbl[2] = 2;
   L[1] = 1;
   L[2] = 1;

   // Calculate remaining levels.
   for( int ll = 3; ll <= max_N; ++ll )
   {
      L[ll] = 1;
      L[1] = 1;
      tbl[ll] = 2;
      for( int rr = ll - 1; rr > 1; --rr )
      {
         L[rr] = rr*L[rr] + L[rr - 1];
         if( L[rr] >= mod )
            L[rr] %= mod;
         tbl[ll] += L[rr];
         if( tbl[ll] >= mod )
            tbl[ll] %= mod;
      }
   }
}

int
main()
{
   // Store table between test cases for speed.
   unsigned tbl[max_N + 1];
   make_table( tbl );

   int T;
   std::cin >> T;
   for( int ii = 0; ii < T; ++ii )
   {
      int N;
      std::cin >> N;
      std::cout << tbl[N] << "\n";
   }

   return EXIT_SUCCESS;
}
