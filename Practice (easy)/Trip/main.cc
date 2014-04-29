#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <array>

#ifndef ONLINE_JUDGE
#include <cassert>
#define ASSERT( expr ) assert( expr )
#else
#define ASSERT( expr )
#endif

// Not sure this guy is working properly...
class tokenizer
{
public:

   static const unsigned max_buffer_size = 20000;
   static const unsigned default_limit = 20;

public:

   tokenizer( unsigned limit = default_limit )
      : _pos( _buf + max_buffer_size ),
        _limit( limit )
   {
   }

   inline
   std::pair<char const*,size_t>
   next()
   {
      skip_ws();
      char* start = _pos;
      while( !std::isspace( *_pos ) )
         ++_pos;
      ASSERT( _pos < _buf + max_buffer_size );
      return std::make_pair( start, _pos - start );
   }

   template< class T >
   inline
   T
   next_int()
   {
      skip_ws();
      int val = *_pos++ - '0';
      while( std::isdigit( *_pos ) )
         val = 10*val + *_pos++ - '0';
      ASSERT( _pos < _buf + max_buffer_size );
      return val;
   }

   inline
   void
   skip_ws()
   {
      do
      {
         refill();
         while( _pos < _buf + max_buffer_size && std::isspace( *_pos )  )
            ++_pos;
      }
      while( _pos == _buf + max_buffer_size );
      refill();
   }

   inline
   void
   refill()
   {
      size_t rem = _buf + max_buffer_size - _pos;
      if( rem < _limit )
      {
         std::copy( _pos, _buf + max_buffer_size, _buf );
         auto size = std::fread( _buf + rem, sizeof(char), sizeof(_buf) - rem, stdin );
         _pos = _buf;
      }
   }

protected:

   char _buf[max_buffer_size];
   char* _pos;
   size_t _limit;
};

static const unsigned max_N = 1000001;
static const unsigned MOD = 1000000007;
unsigned aa[max_N];
unsigned min_stops[max_N], C[max_N];
tokenizer tok;

int
main()
{
   unsigned N = tok.next_int<unsigned>(), M = tok.next_int<unsigned>();
   for( unsigned ii = 1; ii <= N; ++ii )
      aa[ii] = tok.next_int<unsigned>();

   // Urgh, was trying too much to minimize memory footprint. Decided
   // to abandon it and use two arrays.
   min_stops[N] = 0;
   C[N] = 1;
   int total = 0;
   // long long total = 0;
   unsigned jj = N, kk = N;
   for( unsigned ii = N - 1; ii > 0; --ii )
   {
      for( ; aa[jj] - aa[ii] > M; --jj )
      {
         total -= C[jj];
         if( total < 0 )
            total += MOD;
      }
      min_stops[ii] = min_stops[jj] + 1;
      for( ; min_stops[kk] != min_stops[ii]; --kk )
      {
         // Suspect this will also not exceed the limits
         // for a single update.
         total += C[kk];
         if( total >= MOD )
            total -= MOD;
      }
      C[ii] = total;
   }
   --min_stops[1];

   printf( "%d %d\n", min_stops[1], C[1] );

   return EXIT_SUCCESS;
}
