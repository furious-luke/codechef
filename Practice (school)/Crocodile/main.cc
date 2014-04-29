#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <limits>

static const uint32_t oo = std::numeric_limits<uint32_t>::max();

// For ordering in the queue. Also stores the progenitor
// and calculated distance (the distance is how the queue
// determines ordering).
struct room_type
{
   bool
   operator<( room_type const& other ) const
   {
      return dist > other.dist;
   }

   uint32_t room;
   uint32_t from;
   uint32_t dist;
};

void
dijkstra( std::vector<uint32_t> const& exits,
          std::vector<uint32_t> const& displs,
          std::vector<std::array<uint32_t,2>> const& adj,
          std::vector<std::array<uint32_t,2>>& dist,
          std::vector<std::array<uint32_t,2>>& prog )
{
   // Setup the queue.
   std::priority_queue<room_type> Q;
   for( uint32_t ii = 0; ii < dist.size(); ++ii )
   {
      dist[ii][0] = dist[ii][1] = oo;
      prog[ii][0] = prog[ii][1] = oo;
   }
   for( uint32_t ii = 0; ii < exits.size(); ++ii )
   {
      dist[exits[ii]][0] = 0;
      Q.push( room_type{ exits[ii], oo, 0 } );
   }

   // Need to continue until nothing else can be updated.
   while( !Q.empty() )
   {
      uint32_t u    = Q.top().room;
      uint32_t from = Q.top().from;
      uint32_t d    = Q.top().dist;
      Q.pop();

      // Need this because a closer node may have been through the
      // queue prior to this one showing up. Helps with performance;
      // I think it would still be correct without this.
      if( d >= dist[u][1] )
         continue;

      // Maintain nearest and second nearest.
      bool changed = false;
      if( d < dist[u][0] )
      {
         if( prog[u][0] != from )
         {
            dist[u][1] = dist[u][0];
            prog[u][1] = prog[u][0];
            if( dist[u][1] != oo )
               changed = true;
         }
         dist[u][0] = d;
         prog[u][0] = from;
      }
      else if( d < dist[u][1] )
      {
         dist[u][1] = d;
         prog[u][1] = from;
         changed = true;
      }

      // Update the queue.
      if( changed )
      {
         for( uint32_t ii = displs[u]; ii < displs[u + 1]; ++ii )
         {
            uint32_t v = adj[ii][0];
            uint32_t w = adj[ii][1];
            uint32_t Tv = dist[v][1];
            if( dist[u][1] + w < Tv )
               Q.push( room_type{ v, u, dist[u][1] + w } );
         }
      }
   }
}

int
main()
{
   // Read everything and convert to adjacency list.
   unsigned N, M, K;
   std::vector<uint32_t> displs, exits;
   std::vector<std::array<uint32_t,2>> adj;
   std::vector<std::array<uint32_t,2>> dist, prog;
   {
      scanf( "%d %d %d", &N, &M, &K );
      exits.resize( K );
      dist.resize( N );
      prog.resize( N );
      displs.resize( N + 1 );
      adj.resize( 2*M );
      std::vector<std::array<uint32_t,2>> edges( M );
      std::vector<uint32_t> costs( M );
      std::fill( displs.begin(), displs.end(), 0 );
      for( uint32_t ii = 0; ii < M; ++ii )
      {
         uint32_t u, v, l;
         scanf( "%d %d %d", &u, &v, &l );
         edges[ii][0] = u;
         edges[ii][1] = v;
         costs[ii] = l;
         ++displs[u + 1];
         ++displs[v + 1];
      }
      for( uint32_t ii = 0; ii < K; ++ii )
         scanf( "%d", &exits[ii] );
      for( uint32_t ii = 1; ii <= N; ++ii )
         displs[ii] += displs[ii - 1];
      std::vector<uint16_t> cnts( N );
      std::fill( cnts.begin(), cnts.end(), 0 );
      for( uint32_t ii = 0; ii < M; ++ii )
      {
         uint32_t u = edges[ii][0], v = edges[ii][1];
         uint32_t c = costs[ii];
         uint32_t d = displs[u] + cnts[u]++;
         adj[d][0] = v; adj[d][1] = c;
         d = displs[v] + cnts[v]++;
         adj[d][0] = u; adj[d][1] = c;
      }
   }

   // Solve through a Dijkstra-like algorithm (pretty much the
   // same thing but with multiple sources and a different cost
   // function.
   dijkstra( exits, displs, adj, dist, prog );

   std::cout << dist[0][1] << "\n";
}
