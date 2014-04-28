#include <cstdlib>
#include <iostream>

template< class T >
T
n_choose_k( T n,
            T k )
{
   if( !k )
      return 0;
   else
   {
      T enu = 1;
      T den = 1;
      for( T ii = 1; ii <= k; ++ii )
      {
         enu *= (n + 1 - ii);
         den *= ii;
      }
      return enu/den;
   }
}

int
main()
{
   unsigned N;
   unsigned bits[32];
   std::fill( bits, bits + 32, 0 );
   std::cin >> N;
   for( unsigned ii = 0; ii < N; ++ii )
   {
      unsigned a;
      std::cin >> a;
      unsigned mask = 1;
      for( int jj = 0; jj < 32; ++jj, mask <<= 1 )
         bits[jj] += (a & mask) ? 1 : 0;
   }
   unsigned long long S = 0;
   unsigned long long fac = 1;
   for( int ii = 0; ii < 32; ++ii, fac <<= 1 )
      S += n_choose_k<unsigned long long>( bits[ii], 2 )*fac;
   std::cout << S << "\n";
}
