/// NOTES: 1) If there is one exit, we can never be guaranteed
///           of reaching it.
///        2) There must be a node with two exits attached in
///           order to succeed.
///        3) Any non-starting, non-exit node with only 2 edges
///           might as well be removed, as it could be blocked
///           such that passage is impossible.

#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <limits>

class tokenizer
{
public:

   static const unsigned max_buffer_size = 20000;
   static const unsigned default_limit = 20;

public:

   tokenizer()
      : _pos( _buf + max_buffer_size ),
        _limit( default_limit )
   {
   }

   // Hardcoding to just return an int to avoid
   // repetition.
   inline
   int
   // std::pair<char const*,size_t>
   next()
   {
      refill();
      while( std::isspace( *_pos ) )
         ++_pos;
      // char* start = _pos;
      // while( !std::isspace( *_pos ) )
      //    ++_pos;
      // return std::make_pair( start, _pos - start );
      int val = *_pos++ - '0';
      while( std::isdigit( *_pos ) )
         val = 10*val + *_pos++ - '0';
      return val;
   }

   inline
   void
   refill()
   {
      if( !_done )
      {
         size_t rem = _buf + max_buffer_size - _pos;
         if( rem < _limit )
         {
            std::copy( _pos, _buf + max_buffer_size, _buf );
            auto size = std::fread( _buf + rem, sizeof(char), sizeof(_buf) - rem, stdin );
            _pos = _buf;
            if( !size )
               _done = true;
         }
      }
   }

protected:

   char _buf[max_buffer_size];
   char* _pos;
   bool _done = false;
   size_t _limit;
};

// ///
// /// Edges need to be unidirectional, always pointing from
// /// lower indexed vertex to higher.
// ///
// template< class T,
//           class DisplType >
// void
// edges_to_adjacency( tokenizer& tok,
//                     std::vector<T>& data,
//                     std::vector<DisplType>& displs )
// {
//    std::sort( ((std::vector<std::array<T,2> >&)data).begin(), ((std::vector<std::array<T,2>>)data).end(),
//               []( std::array<T,2> const& a,
//                   std::array<T,2> const& b )
//               {
//                  return a[0] < b[0];
//               } );
//    // std::qsort( adj.data(), M, 2*sizeof(uint16_t), cmp );
// }

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
          std::vector<uint32_t> const& displs,
          std::vector<std::array<uint32_t,2>> const& adj,
          std::vector<std::array<uint32_t,2> >& dist,
          std::vector<std::array<uint32_t,2> >& prog )
{
   std::priority_queue<uint32_t,std::vector<uint32_t>,Q_cmp> Q{ Q_cmp( dist ) };
   for( uint32_t ii = 0; ii < dist.size(); ++ii )
   {
      dist[ii][0] = dist[ii][0] = std::numeric_limits<uint32_t>::max();
      prog[ii][0] = prog[ii][1] = std::numeric_limits<uint32_t>::max();
   }
   for( uint32_t ii = 0; ii < exits.size(); ++ii )
   {
      dist[exits[ii]][0] = dist[exits[ii]][1] = 0;
      Q.push( exits[ii] );
   }
   while( !Q.empty() )
   {
      uint32_t u = Q.top(); Q.pop();
      // std::cout << "u: " << u << "," << dist[u][0][0] << "," << dist[u][1][0] << "\n";
      for( uint32_t ii = displs[u]; ii < displs[u + 1]; ++ii )
      {
         uint32_t v = adj[ii][0];
         uint32_t w = adj[ii][1];
         // std::cout << "  v: " << v << "," << w << "\n";
         // std::cout << "    BEFORE\n";
         // std::cout << "      d[v][0]: " << dist[v][0][0] << "\n";
         // std::cout << "      d[v][1]: " << dist[v][1][0] << "\n";
         bool changed = false;
         if( dist[u][1] + w < dist[v][0] )
         {
            if( prog[v][0] != u )
            {
               dist[v][1] = dist[v][0];
               prog[v][1] = prog[v][0];
               changed = true;
            }
            dist[v][0] = dist[u][1] + w;
            prog[v][0] = u;
         }
         else if( dist[u][1] + w < dist[v][1] &&
                  prog[v][0] != u )
         {
            dist[v][1] = dist[u][1] + w;
            prog[v][1] = u;
            changed = true;
         }
         if( changed )
            Q.push( v );
         // std::cout << "    AFTER\n";
         // std::cout << "      d[v][0]: " << dist[v][0][0] << "\n";
         // std::cout << "      d[v][1]: " << dist[v][1][0] << "\n";

         // Can never have both best and second best from the same source.
         assert( !(prog[v][0] != std::numeric_limits<uint32_t>::max() ||
                   prog[v][1] != std::numeric_limits<uint32_t>::max()) ||
                 prog[v][0] != prog[v][1] );

         // Costs can never be above 1B.
         assert( dist[v][0] == std::numeric_limits<uint32_t>::max() ||
                 dist[v][0] <= 1000000000 );
         assert( dist[v][1] == std::numeric_limits<uint32_t>::max() ||
                 dist[v][1] <= 1000000000 );
      }
   }
}

int
main()
{
   unsigned N, M, K;
   std::vector<uint32_t> displs, exits;
   std::vector<std::array<uint32_t,2>> adj;
   std::vector<std::array<uint32_t,2> > dist, prog;
   {
      tokenizer token;
      N = token.next();
      M = token.next();
      K = token.next();
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
         edges[ii][0] = token.next(); edges[ii][1] = token.next();
         costs[ii] = token.next();
         ++displs[edges[ii][0] + 1];
         ++displs[edges[ii][1] + 1];
      }
      for( uint32_t ii = 0; ii < K; ++ii )
         exits[ii] = token.next();
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

   dijkstra( exits, displs, adj, dist, prog );

   // Check result.
   uint32_t u = 0, cost = 0;
   while( u != std::numeric_limits<uint32_t>::max() )
   {
      uint32_t v = prog[u][1];
      if( v < std::numeric_limits<uint32_t>::max() )
      {
         uint32_t cost_pre = cost;
         for( uint32_t ii = displs[u]; ii < displs[u + 1]; ++ii )
         {
            if( adj[ii][0] == v )
            {
               cost += adj[ii][1];
               break;
            }
         }
         assert( cost != cost_pre );
      }
      u = v;
   }
   assert( cost == dist[0][1] );

   std::cout << dist[0][1] << "\n";
}
