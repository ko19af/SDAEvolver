#include <iostream>
#include <fstream>
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

    int numStates = 6;
    int numChars = 2;
    int popSize = 50;
    int tournSelector = 3;
    int gaOperator = 1;
    int numGen = 1000;
    int crossOp = 1;
    double crossRate = .1;
    int mutOperator = 1;
    double mutRate = 0.1;
    int runs = 30;
    int heurFunction = 1;

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

    string fileName = "Output/Experiment_" + to_string(numStates) + to_string(numChars) + 
    to_string(popSize) + to_string(tournSelector) + to_string(gaOperator) + 
    to_string(numGen) + to_string(crossOp) + to_string(crossRate) + 
    to_string(mutOperator) + to_string(mutRate) + to_string(runs) + to_string(heurFunction) + ".txt";

    ofstream MyFile(fileName);

    MyFile << "# States: " << numStates << " # Chars: " << numChars <<
     " popSize: " << popSize << " tournSelector " << tournSelector <<
      " gaOperator: " << gaOperator << " numGen: " << numGen <<
       " crossOp: " << crossOp  << " crossRate(%): " << 
       setprecision(15) << crossRate << " mutationOperator: " <<
        mutOperator << " mutationRate(%):  " << setprecision(15) <<
         mutRate <<  "Heurestic: " << heurFunction << " runs: " << runs << endl;

    srand(1); // seed the random number generator

    for (int x = 0; x < runs; x++){
        MyFile << "Run: " << x + 1 << endl;
        if(gaOperator == 0) Generational(MyFile, numStates, numChars, popSize, tournSelector, numGen, crossOp, crossRate, mutOperator, mutRate, heurFunction);
        else if(gaOperator == 1) Steady(MyFile, numStates,numChars, popSize, tournSelector, numGen, crossOp, crossRate, mutOperator, mutRate, heurFunction);
    }
    MyFile.close();
    return 0;
}