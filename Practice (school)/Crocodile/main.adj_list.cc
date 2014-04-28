/// NOTES: 1) If there is one exit, we can never be guaranteed
///           of reaching it.
///        2) There must be a node with two exits attached in
///           order to succeed.

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>

int
cmp( void const* a,
     void const* b )
{
   return ((unsigned*)a)[0] - ((unsigned*)b)[1];
}

int
main()
{
   unsigned N, M, K;
   std::cin >> N >> M >> K;
   std::vector<uint16_t> displs( N + 1 ), adj( 2*M );
   std::vector<uint32_t> cost( M );
   std::vector<bool> exits( K );
   std::fill( displs.begin(), displs.end(), 0 );
   for( unsigned ii = 0; ii < M; ++ii )
   {
      uint16_t u, v;
      uint32_t l;
      std::cin >> u >> v >> l;
      ++displs[u + 1];
      ++displs[v + 1];
      adj[2*ii + 0] = u;
      adj[2*ii + 1] = v;
   }
   for( unsigned ii = 2; ii <= N; ++ii )
      displs[ii] += displs[ii - 1];
   std::qsort( adj.data(), M, 2*sizeof(uint16_t), cmp );
   {
      std::vector<uint32_t> cnts( N );
      std::fill( cnts.begin(), cnts.end(), 0 );
      unsigned pos = 0;
      for( unsigned ii = 0; ii < N; ++ii )
      {
         while( adj[2*pos + 0] == ii )
            adj[displs[ii] + cnts[ii]++] = adj[2*pos++ + 1];
      }
      std::vector<uint32_t> cnts2( N );
      std::fill( cnts2.begin(), cnts2.end(), 0 );
      for( unsigned ii = 0; ii < N; ++ii )
      {
         for( unsigned jj = displs[ii]; jj < displs[ii] + cnts[ii]; ++jj )
            adj[displs[adj[jj]] + cnts[adj[jj]] + cnts2[adj[jj]]++] = ii;
      }
   }

   for( unsigned ii = 0; ii < N; ++ii )
   {
      std::cout << ii << ":";
      for( unsigned jj = displs[ii]; jj < displs[ii + 1]; ++jj )
         std::cout << " " << adj[jj];
      std::cout << "\n";
   }
}
