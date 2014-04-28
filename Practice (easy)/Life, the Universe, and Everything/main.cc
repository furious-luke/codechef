#include <cstdlib>
#include <iostream>

int
main()
{
   int n;
   while( 1 )
   {
      std::cin >> n;
      if( n == 42 )
         break;
      std::cout << n << "\n";
   };
   return EXIT_SUCCESS;
}
