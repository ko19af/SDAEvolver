#include <iostream>
#include <fstream>
#include <cstdlib>
#include <bits/stdc++.h>
#include <iomanip> 
#include <limits>
#include <filesystem>
#include "SDA.h"
#include "Topology.h"
#include "Generational.h"
#include "AttackSim.h"
#include "Steady.h"
#include "vector"

using namespace std;

int main(int argc, char* argv[]) {
    srand(1);// seed the random number generator
    
    if(atoi(argv[13])){// if the 13th console passed parameter is one initiate attack simulation
        AttackSim(atoi(argv[12]), atof(argv[14]) / 100); // call attack sim and provide it with the appropriate parameters
        return 0;// exit program
    }
    
    int numStates = 6;// initialize the variables in case none are passed on the cmd line
    int numChars = 2;
    int popSize = 50;
    int tournSelector = 3;
    int gaOperator = 1;
    int numGen = 10000;
    int crossOp = 1;
    double crossRate = 0.9;
    int mutOperator = 1;
    double mutRate = 0.9;
    int runs = 30;
    int heurFunction = 0;

    
    if(argc > 1){// collect hyper-parameters for the run (if provided)
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

    for (int t = 0; t < 5; t++){
        
        string path = "Topologies/Layout_"+to_string(t)+".txt";
    
        Topology T = Topology(path, false); // initialize the topology

        ofstream MyFile("Output/Experiment_" + to_string(numStates) + to_string(numChars) + 
        to_string(popSize) + to_string(tournSelector) + to_string(gaOperator) + 
        to_string(numGen) + to_string(crossOp) + to_string(crossRate) + 
        to_string(mutOperator) + to_string(mutRate) + to_string(runs) + 
        to_string(heurFunction) + to_string(t) + ".txt");

        MyFile << "# States: " << numStates << " # Chars: " << numChars <<
        " popSize: " << popSize << " tournSelector " << tournSelector <<
        " gaOperator: " << gaOperator << " numGen: " << numGen <<
        " crossOp: " << crossOp  << " crossRate(%): " << 
        setprecision(15) << crossRate << " mutationOperator: " <<
            mutOperator << " mutationRate(%):  " << setprecision(15) <<
            mutRate <<  " Heurestic: " << heurFunction << " Topology: " << t << " runs: " << runs << endl;

        MyFile << "Topology: " << t + 1 << endl;

        for (int x = 0; x < runs; x++){
            MyFile << "Run: " << x + 1 << endl;
            if(gaOperator == 0) Generational(T, MyFile, numStates, numChars, popSize, tournSelector, numGen, crossOp, crossRate, mutOperator, mutRate, heurFunction);
            else if(gaOperator == 1) Steady(T, MyFile, numStates,numChars, popSize, tournSelector, numGen, crossOp, crossRate, mutOperator, mutRate, heurFunction);
        }
        MyFile.close();
    }
    return 0;
}