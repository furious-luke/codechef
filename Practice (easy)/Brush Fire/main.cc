#include <cstdlib>
#include <cstdio>
#include <algorithm>

class tokenizer
{
public:

   static const unsigned max_buffer_size = 20000;
   static const unsigned default_limit = 10;

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

// fast_atoi to_int;
tokenizer token;
static const int max_n = 10001;
static const int max_adj = 30003;
int displs[max_n + 1];
int adj[max_adj];
bool flags[max_n];

inline
int
get_int()
{
   // to_int( token.next() );
   // return to_int.value();
   return token.next();
}

bool
dfs( int cur,
     int par = -1 )
{
   if( flags[cur] )
      return false;
   int ok = 0;
   for( int ii = displs[cur]; ii < displs[cur + 1]; ++ii )
   {
      if( adj[ii] == par )
         continue;
      ok += !dfs( adj[ii], cur );
   }
   return ok <= 1;
}

int
main()
{
   int T = get_int();
   for( int ii = 0; ii < T; ++ii )
   {
      int n = get_int(), s = get_int(), t = get_int();

      std::fill( flags + 1, flags + n + 1, false );

      int pos = 1;
      for( int jj = 1; jj <= n; ++jj )
      {
         int n_nbrs = get_int();
         displs[jj] = pos;
         for( int kk = 0; kk < n_nbrs; ++kk )
            adj[pos++] = get_int();
      }
      displs[n + 1] = pos;

      for( int jj = 1; jj <= t; ++jj )
      {
         int save = get_int();
         flags[save] = true;
      }

      if( dfs( s ) )
         printf( "yes\n" );
      else
         printf( "no\n" );
   }

   return EXIT_SUCCESS;
}
