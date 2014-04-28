/// NOTES: 1) If there is one exit, we can never be guaranteed
///           of reaching it.
///        2) There must be a node with two exits attached in
///           order to succeed.
///        3) Any non-starting, non-exit node with only 2 edges
///           might as well be removed, as it could be blocked
///           such that passage is impossible.

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>
#include <array>
#include <queue>
#include <limits>

struct Q_cmp
{
   Q_cmp( std::vector<std::array<uint32_t,2> > const& dist )
      : dist( dist )
   {
   }

   bool
   operator()( uint32_t a,
               uint32_t b ) const
   {
      return dist[a][1] > dist[b][1];
   }

   std::vector<std::array<uint32_t,2> > const& dist;
};

void
dijkstra( std::vector<uint32_t> const& exits,
          std::multimap<uint32_t,std::pair<uint32_t,uint32_t> > const& adj,
          std::vector<std::array<uint32_t,2> >& dist )
{
   std::priority_queue<uint32_t,std::vector<uint32_t>,Q_cmp> Q{ Q_cmp( dist ) };
   for( uint32_t ii = 0; ii < dist.size(); ++ii )
      dist[ii][0] = dist[11][1] = std::numeric_limits<uint32_t>::max();
   for( uint32_t ii = 0; ii < exits.size(); ++ii )
   {
      dist[exits[ii]][0] = dist[exits[ii]][1] = 0;
      Q.push( exits[ii] );
   }
   while( !Q.empty() )
   {
      uint32_t u = Q.top();
      Q.pop();
      // std::cout << "u: " << u << "\n";
      auto rng = adj.equal_range( u );
      while( rng.first != rng.second )
      {
         uint32_t v = rng.first->second.first;
         uint32_t w = rng.first->second.second;
         // std::cout << "  v: " << v << "\n";
         // std::cout << "    d[u][1] + w: " << dist[u][1] + w << "\n";
         // std::cout << "    d[v][0]    : " << dist[v][0] << "\n";
         bool reheap = false;
         if( dist[u][1] + w < dist[v][0] )
         {
            dist[v][1] = dist[v][0];
            dist[v][0] = dist[u][0] + w;
            reheap = true;
         }
         else if( dist[u][1] + w < dist[v][1] )
         {
            dist[v][1] = dist[u][1] + w;
            reheap = true;
         }
         if( reheap )
            Q.push( v );
         ++rng.first;
      }
   }
}

int
main()
{
   unsigned N, M, K;
   std::cin >> N >> M >> K;
   std::cout << N << ", " << M << ", " << K << "\n";
   std::multimap<uint32_t,std::pair<uint32_t,uint32_t> > adj;
   std::vector<uint32_t> exits( K );
   std::vector<std::array<uint32_t,2> > dist( N );
   for( uint32_t ii = 0; ii < M; ++ii )
   {
      uint32_t u, v;
      uint32_t l;
      std::cin >> u >> v >> l;
      adj.emplace( u, std::make_pair( v, l ) );
      adj.emplace( v, std::make_pair( u, l ) );
   }
   for( uint32_t ii = 0; ii < K; ++ii )
      std::cin >> exits[ii];
   // std::sort( exits.begin(), exits.end() );

   // // Remove all 2-edge rooms.
   // {
   //    std::list<uint32_t> to_remove;
   //    uint32_t exit_pos = 0;
   //    for( uint32_t ii = 1; ii < N; ++ii )
   //    {
   //       if( ii == exits[exit_pos] )
   //       {
   //          ++exit_pos;
   //          continue;
   //       }
   //       if( adj.count( ii ) == 2 )
   //          to_remove.push_back( ii );
   //    }
   //    for( auto rem : to_remove )
   //       adj.erase( rem );
   // }

   dijkstra( exits, adj, dist );

   std::cout << dist[0][1] << "\n";
}
