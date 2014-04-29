#include "assert.hh"

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
