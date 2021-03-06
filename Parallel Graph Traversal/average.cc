#include <iostream>
#include <cstdlib>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace std;

//Should fill the int pointed to by sum with the sum of
//elements in arr from index start up to index end - 1
//using numThreads threads for parallelism.
void getSum(int* arr, long long start, long long end, int numThreads, long long* sum)
{
 if(end == start + 1)
   {
      *sum = arr[start];
   }
   else
   {
      long long leftSum;
      long long rightSum;
      if(numThreads == 1)
      {
	 long long midway = (start + end)/2;
	 getSum(arr, start, midway, 1, &leftSum);
	 getSum(arr, midway, end, 1, &rightSum);
      }
      else
      {
	 int halfNumThreads = numThreads/2;
    
	 long long mid = start + halfNumThreads*((end - start)/numThreads); //load balancing
	 
	 boost::thread leftThread(getSum, arr, start, mid, halfNumThreads, &leftSum);
	 
	 getSum(arr, mid, end, numThreads - halfNumThreads, &rightSum);
	 
	 leftThread.join();
      }	 
	
      *sum = leftSum+rightSum;
   }
}

int main(int argc, char** argv)
{
   if(argc < 3)
   {
      cout << "Usage: average <numNumbers> <numThreads>" << endl;
      exit(1);
   }
   long long numNumbers = atoll(argv[1]);
   int numThreads = atoi(argv[2]);

   int* numbers = new int[numNumbers];

   srand(time(0));

   cout << "Filling..." << endl;
   //Fill the array with random numbers
   for(long long i = 0; i < numNumbers; i++)
   {
      numbers[i] = rand();
   }
   
   //Get the sum serially
   cout << "Summing (serial)..." << endl;

   boost::timer timer;
   boost::posix_time::ptime serialStart(boost::posix_time::microsec_clock::local_time());

   long long sum;
   getSum(numbers, 0, numNumbers, 1, &sum);

   boost::posix_time::ptime serialEnd(boost::posix_time::microsec_clock::local_time());
   double serialTime = double((serialEnd - serialStart).total_milliseconds())/1000;
   cout << "Average: " <<  (double)sum/numNumbers << endl;
   cout << "CPU Time: " << timer.elapsed() << "s" << endl;
   cout << "Wall Clock Time: " << serialTime << "s" << endl;
  
   //Get the sum in parallel
   cout << "Summing (parallel)..." << endl;
   timer.restart();
   boost::posix_time::ptime parallelStart(boost::posix_time::microsec_clock::local_time());
   
   getSum(numbers, 0, numNumbers, numThreads, &sum);

   boost::posix_time::ptime parallelEnd(boost::posix_time::microsec_clock::local_time());
   double parallelTime = double((parallelEnd - parallelStart).total_milliseconds())/1000;
   cout << "Average: " << (double)sum/numNumbers << endl;
   cout << "CPU Time: " << timer.elapsed() << "s" << endl;
   cout << "Wall Clock Time: " << parallelTime << "s" << endl;
   cout << "Speedup: " << serialTime/parallelTime << " (" << numThreads << " threads)" << endl;
}
