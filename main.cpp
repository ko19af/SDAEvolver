#include <iostream>
#include <cstdlib>
#include <bits/stdc++.h>
#include <iomanip> 
#include <limits>
#include "SDA.h"
#include "Topology.h"
#include "Generational.h"
#include "Steady.h"

using namespace std;

int main(int argc, char* argv[]) {

    int numStates = 100;
    int numChars = 2;
    int popSize = 10;
    int tournSelector = 7;
    int gaOperator = 0;
    int numGen = 1;
    int crossOp = 1;
    double crossRate = 0.5;
    int mutOperator = 1;
    double mutRate = 0.5;
    int runs = 1;
    int heurFunction = 0;

    //collect hyper-parameters for the run
    if(argc > 1){
    numStates = atoi(argv[1]);
    numChars = atoi(argv[2]);
    popSize = atoi(argv[3]);
    tournSelector = atoi(argv[4]);
    gaOperator = atoi(argv[5]);
    numGen = atoi(argv[6]);
    crossOp = atoi(argv[7]);
    crossRate = atof(argv[8]) / 100;
    mutOperator = atoi(argv[9]);
    mutRate = atof(argv[10]) / 100;
    runs = atoi(argv[11]);
    heurFunction = atoi(argv[12]);
    }

    cout << "# States: " << numStates << " # Chars: " << numChars <<
     " popSize: " << popSize << " tournSelector " << tournSelector <<
      " gaOperator: " << gaOperator << " numGen: " << numGen <<
       " crossOp: " << crossOp  << " crossRate(%): " << 
       setprecision(15) << crossRate << " mutationOperator: " <<
        mutOperator << " mutationRate(%):  " << setprecision(15) <<
         mutRate << " runs: " << runs << endl;

    srand(1); // seed the random number generator

    for (int x = 0; x < runs; x++){
        cout << "Run: " << x + 1 << endl;
        if(gaOperator == 0) Generational(numStates, numChars, popSize, tournSelector, numGen, crossOp, crossRate, mutOperator, mutRate, heurFunction);
        else if(gaOperator == 1) Steady(numStates,numChars, popSize, tournSelector, numGen, crossOp, crossRate, mutOperator, mutRate, heurFunction);
    }   
    return 0;
}