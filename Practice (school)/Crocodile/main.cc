#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <limits>

static const uint32_t oo = std::numeric_limits<uint32_t>::max();

// class tokenizer
// {
// public:

//    static const unsigned max_buffer_size = 20000;
//    static const unsigned default_limit = 20;

// public:

//    tokenizer()
//       : _pos( _buf + max_buffer_size ),
//         _limit( default_limit )
//    {
//    }

//    // Was using a "fast" atoi implementation in addition,
//    // but am hard-coding for integer types.
//    template< class T >
//    inline
//    T
//    next()
//    {
//       refill();
//       while( std::isspace( *_pos ) )
//          ++_pos;
//       T val = *_pos++ - '0';
//       while( std::isdigit( *_pos ) )
//          val = 10*val + *_pos++ - '0';
//       return val;
//    }

//    inline
//    void
//    refill()
//    {
//       if( !_done )
//       {
//          size_t rem = _buf + max_buffer_size - _pos;
//          if( rem < _limit )
//          {
//             std::copy( _pos, _buf + max_buffer_size, _buf );
//             auto size = std::fread( _buf + rem, sizeof(char), sizeof(_buf) - rem, stdin );
//             _pos = _buf;
//             if( !size )
//                _done = true;
//          }
//       }
//    }

// protected:

//    char _buf[max_buffer_size];
//    char* _pos;
//    bool _done = false;
//    size_t _limit;
// };

struct room_type
{
   bool
   operator<( room_type const& other ) const
   {
      return dist > other.dist;
   }

   uint32_t room;
   uint32_t dist;
};

void
dijkstra( std::vector<uint32_t> const& exits,
          std::vector<uint32_t> const& displs,
          std::vector<std::array<uint32_t,2>> const& adj,
          std::vector<uint32_t>& dist )
{
   std::priority_queue<room_type> Q;
   for( uint32_t ii = 0; ii < dist.size(); ++ii )
      dist[ii] = oo;
   for( uint32_t ii = 0; ii < exits.size(); ++ii )
   {
      dist[exits[ii]] = 0;
      Q.push( room_type{ exits[ii], 0 } );
   }
   while( !Q.empty() )
   {
      uint32_t u = Q.top().room; Q.pop();
      uint32_t Tu = dist[u];
      std::array<uint32_t,2> best{ oo, oo };
      for( uint32_t ii = displs[u]; ii < displs[u + 1]; ++ii )
      {
         uint32_t v = adj[ii][0];
         uint32_t w = adj[ii][1];
         uint32_t Tv = dist[v];
         if( Tv == oo )
            continue;
         Tv += w;
         if( Tv < best[0] )
         {
            best[1] = best[0];
            best[0] = Tv;
         }
         else if( Tv < best[1] )
            best[1] = Tv;
      }
      if( best[1] < Tu || Tu == 0 )
      {
         if( Tu > 0 )
         {
            Tu = best[1];
            dist[u] = Tu;
         }
         for( uint32_t ii = displs[u]; ii < displs[u + 1]; ++ii )
         {
            uint32_t v = adj[ii][0];
            uint32_t w = adj[ii][1];
            uint32_t Tv = dist[v];
            if( Tu + w < Tv )
               Q.push( room_type{ v, Tu + w } );
         }
      }
   }
}

int
main()
{
   unsigned N, M, K;
   std::vector<uint32_t> displs, exits;
   std::vector<std::array<uint32_t,2>> adj;
   std::vector<uint32_t> dist;
   {
      // tokenizer token;
      // N = token.next<uint32_t>();
      // M = token.next<uint32_t>();
      // K = token.next<uint32_t>();
      scanf( "%d %d %d", &N, &M, &K );
      exits.resize( K );
      dist.resize( N );
      displs.resize( N + 1 );
      adj.resize( 2*M );
      std::vector<std::array<uint32_t,2>> edges( M );
      std::vector<uint32_t> costs( M );
      std::fill( displs.begin(), displs.end(), 0 );
      for( uint32_t ii = 0; ii < M; ++ii )
      {
         uint32_t u, v, l;
         // edges[ii][0] = token.next<uint32_t>(); edges[ii][1] = token.next<uint32_t>();
         // costs[ii] = token.next<uint32_t>();
         scanf( "%d %d %d", &u, &v, &l );
         edges[ii][0] = u;
         edges[ii][1] = v;
         costs[ii] = l;
         ++displs[u + 1];
         ++displs[v + 1];
      }
      for( uint32_t ii = 0; ii < K; ++ii )
      {
         // exits[ii] = token.next<uint32_t>();
         scanf( "%d", &exits[ii] );
      }
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
      {
         uint32_t sol;
         scanf( "%d", &sol );
         std::cout << sol << "\n";
      }
   }

   dijkstra( exits, displs, adj, dist );

   std::cout << dist[0] << "\n";
}
