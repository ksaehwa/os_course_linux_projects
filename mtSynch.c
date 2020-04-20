#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include<sched.h>
#define THREAD_NUM 5
#define SLEEP_TIME 200000

#define MAX_SIZE 10
int buf[10000];
int in = 0;

#define USE_SPINLOCK

#ifdef USE_SPINLOCK
// Insert your code here. Hint: pthread_spinlock_t ...;
#elif USE_MUTEX
// Insert your code here. Hint: pthread_mutex_t ...;
#endif

static void *myTask( void * arg )
{
    int i, runCount = 0; 
    int taskIndex = *(int *) arg;
    printf("Task%d (cpu=%d) started. 1234-5678 Saehwa Kim\n", taskIndex, sched_getcpu());
    fflush(stdout);
    while(1) {
		   usleep(SLEEP_TIME*taskIndex);
#ifdef USE_SPINLOCKa
         // Insert your code here. Hint: pthread_spin_lock(&...);
#elif USE_MUTEX
         // Insert your code here. Hint: pthread_mutex_lock(&...);
#endif

#ifndef USE_CAS
		   if (in >= MAX_SIZE) {
			   printf("Task%d now exits. in=%d\n", taskIndex, in);
#ifdef USE_SPINLOCK
            // Insert your code here. Hint: pthread_spin_unlock(&...);
#elif USE_MUTEX
            // Insert your code here. Hint: pthread_mutex_unlock(&...);
#endif
				return (int*) arg;
		   }
#endif

#ifdef USE_CAS
        {
           int old_in, new_in, old_buf, new_buf;
           do {
             if (in >= MAX_SIZE) {
               printf("Task%d now exits. in=%d\n", taskIndex, in);
               return (int*) arg;
             }
             old_in = in;
             old_buf = buf[old_in];
             new_buf = taskIndex * 100 + old_in;
             usleep(SLEEP_TIME*taskIndex);
             new_in = old_in + 1;
           } while( /* Insert your code here */  || 
                  !__sync_bool_compare_and_swap(&buf[old_in], old_buf, new_buf) );
        }
#else
		buf[in] = taskIndex * 100 + in;
		usleep(SLEEP_TIME*taskIndex);
        in = in + 1;
#endif
		usleep(SLEEP_TIME*taskIndex);
		printf("Task%d (cpu=%d) [runCount=%d] {in=%d} ", 
				taskIndex, sched_getcpu(), ++runCount, in);
	    for (i=0; i<in; i++) 
	         printf("%d ", buf[i]);
        printf("\n");
        fflush(stdout);
		usleep(SLEEP_TIME*taskIndex);
#ifdef USE_SPINLOCK
         // Insert your code here. Hint: pthread_spin_unlock(&...);
#elif USE_MUTEX
         // Insert your code here. Hint: pthread_mutex_unlock(&...);
#endif
    } // end of while(1)
}
/*-----------------------------------------------------------*/

int main( void )
{
	int status[THREAD_NUM];
   int arg[THREAD_NUM];
	int i;
   double elapsedT;
   struct timespec startT, endT;
	pthread_t thread_t[THREAD_NUM];

#ifdef USE_SPINLOCK
   // Insert your code here. Hint: pthread_spin_init(&..., 0);
#elif USE_MUTEX
   // Insert your code here. Hint: pthread_mutex_init(&..., NULL);
#endif
	printf("Main (cpu=%d). Before creating any thread.\n", sched_getcpu());

   for(i=0; i < THREAD_NUM; i++) {
	   arg[i] = i+1;
//    	fflush(stdout);
      if( pthread_create( &thread_t[i], NULL, myTask, &arg[i]) < 0 )  
				printf("Error: was failed to create thread %d.", i);
      else printf("Main (cpu=%d) Thread%d created.\n", sched_getcpu(), i);
   }
	printf("Main (cpu=%d) After creating all threads.\n", sched_getcpu());
	
	clock_gettime(CLOCK_REALTIME, &startT);
	for(i=0 ; i<THREAD_NUM ; i++)
		pthread_join(thread_t[i],(void **)&status[i]);
   clock_gettime(CLOCK_REALTIME, &endT);
   elapsedT = endT.tv_sec - startT.tv_sec + (endT.tv_nsec - startT.tv_nsec)/1e9L; 
	printf("Main (cpu=%d) now exits. elapsedT=%0.9lf\n", sched_getcpu(), elapsedT);

#ifdef USE_SPINLOCK
   printf("USE_SPINLOCK\n");
#endif
#ifdef USE_MUTEX
   printf("USE_MUTEX\n");
#endif
#ifdef USE_CAS
   printf("USE_CAS\n");
#endif
   return 0;
}
