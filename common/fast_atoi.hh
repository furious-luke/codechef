#include "assert.hh"

class fast_atoi
{
public:

   fast_atoi()
      : _value( 0 )
   {
   }

   inline
   int
   value() const
   {
      return _value;
   }

   inline
   bool
   operator()( std::pair<char const*,size_t> const& str )
   {
      (*this)( str.first, str.second );
   }

   inline
   bool
   operator()( char const* str,
               size_t len )
   {
      _value = 0;

      switch( len )
      {
         case 10:    _value += (str[len - 10] - '0')*1000000000;
         case  9:    _value += (str[len -  9] - '0')*100000000;
         case  8:    _value += (str[len -  8] - '0')*10000000;
         case  7:    _value += (str[len -  7] - '0')*1000000;
         case  6:    _value += (str[len -  6] - '0')*100000;
         case  5:    _value += (str[len -  5] - '0')*10000;
         case  4:    _value += (str[len -  4] - '0')*1000;
         case  3:    _value += (str[len -  3] - '0')*100;
         case  2:    _value += (str[len -  2] - '0')*10;
         case  1:    _value += (str[len -  1] - '0');
            return _value > 0;

         default:
            return false;
      }
   }

private:

   int _value;
};
