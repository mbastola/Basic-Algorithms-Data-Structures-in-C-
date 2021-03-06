#include <stdio.h>
#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <queue>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>


using namespace std;

void takeJobs(int threadNum, queue<int>* jobQueue,boost::mutex* myLock)
{
   //Keep listening for jobs forever

   while(true)
   {
      //If there's a job to take
      myLock->lock();
      bool isEmpty = jobQueue->empty();
      if (isEmpty)
      {	
	 myLock->unlock();
	 sleep(1);
      }
      else
      {
	 //Take it from the front of the queue
	 int jobLength = jobQueue->front();
	 jobQueue->pop();
	 myLock->unlock();
	 //Using printf instead of cout since it prints all at once
	 //(avoids output interleaving)
	 printf("%i took a job of length %i. Number of jobs left: %i\n", threadNum, jobLength, (int)jobQueue->size());
	 
	 //Sleep this thread for the duration of the job
	 sleep(jobLength);
      }
   }
}

void makeJobs(int threadNum, queue<int>* jobQueue)
{
   //Keep producing jobs forever
   while(true)
   {
      //Pick a random duration from 1 to 5 
      //and push the job onto the queue.
      int jobLength = rand()%5 + 1;
      jobQueue->push(jobLength);
      //Using printf instead of cout since it prints all at once
      //(avoids output interleaving)
      printf("%i created a job of length %i. Number of jobs left: %i\n", threadNum, jobLength, (int)jobQueue->size());

      //Wait for 1 second before producing the next job
      sleep(1);
   }
}

int main(int argc, char** argv)
{
   //Get the number of consumers
   int numConsumers = 1;
   if(argc > 1)
   {
      numConsumers = atoi(argv[1]);
   }

   queue<int> jobQueue;

   //Create the producer and consumer threads
   boost::thread producer = boost::thread(makeJobs, 0, &jobQueue);
   boost::thread* consumers = new boost::thread[numConsumers];
   boost::mutex lock;
   for(int i = 0; i < numConsumers; i++)
   {
      consumers[i] = boost::thread(takeJobs, i + 1, &jobQueue,&lock);
      sleep(1);
   }

   //Make the main thread wait for the spawned threads
   //(they will never return but that's okay...)
   producer.join();
   for(int i = 0; i < numConsumers; i++)
   {
      consumers[i].join();
   }
}
