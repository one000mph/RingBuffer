#include <stdio.h>
#include <time.h>   /* Needed for struct timespec */


int nsleep(long milliseconds)
{
   struct timespec req, rem;

   if(milliseconds > 999)
   {   
        req.tv_sec = (int)(milliseconds / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (milliseconds - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   }   
   else
   {   
        req.tv_sec = 0;                         /* Must be Non-Negative */
        req.tv_nsec = milliseconds * 1000000;    /* Must be in range of 0 to 999999999 */
   }   

   return nanosleep(&req , &rem);
}

int main()
{
   int ret = nsleep(500);
   printf("sleep result %d\n",ret);
   return 0;
}