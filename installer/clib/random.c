
/*
* Generate a random number based on the 
* set limit.
*/
long rand_n(long lim)
{
   static long a = 100001;
   a = (a * 125) % 2796203;
   return ((a % lim) + 1);
}
