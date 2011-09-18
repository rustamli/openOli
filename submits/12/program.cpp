#include <iostream>
#include <map>
#include <string>

using namespace std;
int main()
{
   int n, i, c;
   cin >> n;
   for (i=0; i<n; i++)
   {
     cin >> c;
     if ((c%2)>0)
         cout << c+2 << " ";
     else
         cout << c << " ";
   };
   return 0;
}
