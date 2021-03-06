#include <iostream>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace std;

void initialize( double* dist, long long* prev, int numThreads,long long start, long long end, bool* inTree)
{
   if (end == start+1)               ///When it reaches the base case, update.
   {
      dist[start] = numeric_limits<double>::infinity();
      prev[start] = 0;
      inTree[start] = false;
   }
   else
   {
      if(numThreads == 1)
      {
	 long long midway = (start + end)/2;
	 initialize( dist, prev, 1, start, midway, inTree);
	 initialize(dist, prev, 1, midway, end, inTree);
      }
      else
      {
	 int halfNumThreads = numThreads/2;
    
	 long long mid = start + halfNumThreads*((end - start)/numThreads); //load balancing
	 
	 boost::thread leftThread(initialize, dist, prev, halfNumThreads, start, mid, inTree);
	 
	 initialize(dist, prev, numThreads - halfNumThreads, mid, end, inTree);
	 
	 leftThread.join();
      }
   }
}

void getMinVertex(double* dist, long long* prev, int numThreads,long long start, long long end, double* min, long long* minVertex,bool* inTree)
{
   if(end == start + 1)
   {
      if (!inTree[start] &&  dist[start]<*min)                //check the conditions and update
      {
	 *min = dist[start];
	 *minVertex = start;
      }
   }
   else
   {
      double leftMin = *min;
      double rightMin = *min;
      long long leftMinVertex=*minVertex;
      long long rightMinVertex=*minVertex;
      if(numThreads == 1)
      {
	 int midway = (start + end)/2;
	 getMinVertex(dist, prev, 1, start, midway, &leftMin, &leftMinVertex, inTree);
	 getMinVertex(dist, prev, 1 , midway, end, &rightMin, &rightMinVertex, inTree);
      }
      else
      {
	 int halfNumThreads = numThreads/2;
	 int mid = start + halfNumThreads*((end - start)/numThreads);
	 boost::thread leftThread(getMinVertex, dist, prev,  halfNumThreads , start, mid, &leftMin, &leftMinVertex, inTree);
	 getMinVertex(dist, prev, numThreads - halfNumThreads ,mid, end, &rightMin, &rightMinVertex, inTree);
	 leftThread.join();
      }
      *min = leftMin;
      *minVertex  = leftMinVertex;
      if( rightMin < leftMin)                    //if right is smaller, right is the min
      {
	 *min = rightMin;
	 *minVertex = rightMinVertex;
      }
   }
}
  
void updateDistances(const double* const * adjMatrix, double* dist, long long* prev, int numThreads, long long start, long long end, int minVertex, bool* inTree)
{
   if(end == start + 1)
   {
      if (!inTree[start] && dist[start] > adjMatrix[start][minVertex] + dist[minVertex])    //check conditions and update
      {
	 dist[start] = adjMatrix[start][minVertex] + dist[minVertex];
	 prev[start-1] = minVertex;
      }
   }
   else
   {
      if(numThreads == 1)
      {
	 int midway = (start + end)/2;
	 updateDistances(adjMatrix,dist, prev, 1, start, midway, minVertex, inTree);
	 updateDistances(adjMatrix,dist, prev, 1, midway, end, minVertex, inTree);
      }
      else
      {
	 int halfNumThreads = numThreads/2;
	 int mid = start + halfNumThreads*((end - start)/numThreads);
	 boost::thread leftThread(updateDistances,adjMatrix,dist, prev, halfNumThreads, start, mid, minVertex, inTree); 
	 updateDistances(adjMatrix,dist, prev, numThreads-halfNumThreads, mid, end, minVertex, inTree);
	 leftThread.join();
      }
   }
}   
   
      
void dijkstras(long long numVertices, const double* const * adjMatrix, long long source, double* dist, long long* prev, int numThreads)
{
   bool* inTree = new bool[numVertices]; 
   initialize(dist,prev,numThreads,0,numVertices, inTree);
   dist[source] = 0;				
   long long minVertex;
   double min;
   for (int i = 1;i<numVertices;i++)
   {
      minVertex = 0;
      min =  numeric_limits<double>::infinity();
      getMinVertex(dist,prev,numThreads,0,numVertices,&min, &minVertex, inTree);
      inTree[minVertex] = true;
      updateDistances(adjMatrix, dist, prev, numThreads, 0, numVertices, minVertex, inTree);  
   }
   delete [] inTree;
}

int main(int argc, char** argv)
{
   if(argc < 3)
   {
      cout << "Usage: dijsktras <numVertices> <numThreads>" << endl;
      exit(1);
   }
   long long numVertices = atoll(argv[1]);
   int numThreads = atoi(argv[2]);

   srand(time(0));

   cout << "Filling..." << endl;
   
   //Fill the adjacency matrix with random weights from 0 to 1
   //(10 percent chance of "no edge" which is indicated with
   //an infinite weight)
   double** adjMatrix = new double*[numVertices];
   for(int i = 0; i < numVertices; i++)
   {
      adjMatrix[i] = new double[numVertices];
      
      for(int j = 0; j < numVertices; j++)
      {
	 if(j == i || (double)rand()/RAND_MAX < 0.1)
	 {
	    //There is never an edge from a vertex to itself
	    //Also there is a 10 percent chance of no edge from
	    //any vertex to any other vertex.
	    adjMatrix[i][j] = numeric_limits<double>::infinity();
	 }
	 else
	 {
	    adjMatrix[i][j] = (double)rand()/RAND_MAX;
	 }
      }
   }

   double* dist = new double[numVertices];
   long long* prev = new long long[numVertices];

   //Get the shortest paths serially
   cout << "Computing (serial)..." << endl;

   boost::timer timer;
   boost::posix_time::ptime serialStart(boost::posix_time::microsec_clock::local_time());

   dijkstras(numVertices, adjMatrix, 0, dist, prev, 1);

   boost::posix_time::ptime serialEnd(boost::posix_time::microsec_clock::local_time());
   double serialTime = double((serialEnd - serialStart).total_milliseconds())/1000;
   cout << "Shortest distance from v0 -> v1: " <<  dist[1] << endl;
   cout << "CPU Time: " << timer.elapsed() << "s" << endl;
   cout << "Wall Clock Time: " << serialTime << "s" << endl;
  
   //Get the shortest paths in parallel
   cout << "Computing (parallel)..." << endl;
   timer.restart();
   boost::posix_time::ptime parallelStart(boost::posix_time::microsec_clock::local_time());
   
   dijkstras(numVertices, adjMatrix, 0, dist, prev, numThreads);

   boost::posix_time::ptime parallelEnd(boost::posix_time::microsec_clock::local_time());
   double parallelTime = double((parallelEnd - parallelStart).total_milliseconds())/1000;
   cout << "Shortest distance from v0 -> v1: " <<  dist[1] << endl;
   cout << "CPU Time: " << timer.elapsed() << "s" << endl;
   cout << "Wall Clock Time: " << parallelTime << "s" << endl;
   cout << "Speedup: " << serialTime/parallelTime << " (" << numThreads << " threads)" << endl;
}
