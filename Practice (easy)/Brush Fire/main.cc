#include <cstdlib>
#include <cstdio>
#include <algorithm>

// class fast_atoi
// {
// public:

//    fast_atoi()
//       : _value( 0 )
//    {
//    }

//    inline
//    int
//    value() const
//    {
//       return _value;
//    }

//    inline
//    bool
//    operator()( std::pair<char const*,size_t> const& str )
//    {
//       (*this)( str.first, str.second );
//    }

//    inline
//    bool
//    operator()( char const* str,
//                size_t len )
//    {
//       _value = 0;

//       switch( len )
//       {
//          case 10:    _value += (str[len - 10] - '0')*1000000000;
//          case  9:    _value += (str[len -  9] - '0')*100000000;
//          case  8:    _value += (str[len -  8] - '0')*10000000;
//          case  7:    _value += (str[len -  7] - '0')*1000000;
//          case  6:    _value += (str[len -  6] - '0')*100000;
//          case  5:    _value += (str[len -  5] - '0')*10000;
//          case  4:    _value += (str[len -  4] - '0')*1000;
//          case  3:    _value += (str[len -  3] - '0')*100;
//          case  2:    _value += (str[len -  2] - '0')*10;
//          case  1:    _value += (str[len -  1] - '0');
//             return _value > 0;

//          default:
//             return false;
//       }
//    }

// private:

//    int _value;
// };

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
