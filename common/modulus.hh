template< size_t M,
          class T >
inline
T
mod( T x )
{
   if( x >= M )
      return x%M;
   else
      return x;
}
