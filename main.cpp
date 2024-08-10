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


void testing(){

    Topology T(5,5,1,1,3);
    //Generational G;

    int outputLen = (T.tNumNodes*(T.tNumNodes-1))/2;
    int numStates = 3;
    int numChars = 2;

    SDA member(numStates, numChars, 2, outputLen, 0, false);
    //vector<int> c(outputLen);// vector for holding response from SDA
    //member.fillOutput(c, true, cout); // fill vector using SDA
    //cout << c.size() << endl;
    //T.setConnections(c, true); // set the connections in the topology

    //G.genCalcFitness(member);

    exit(1);

    SDA firstSDA(numStates, numChars, 2, outputLen, 0, false);
    SDA secondSDA(numStates, numChars, 2, outputLen, 0, false);
    vector<int> firstOutput;
    vector<int> secondOutput;
    firstOutput.reserve(outputLen);
    secondOutput.reserve(outputLen);

    cout << "First SDA:" << endl;
    firstSDA.print(cout);
    firstSDA.fillOutput(firstOutput, true, cout);

    cout << "Second SDA:" << endl;
    secondSDA.print(cout);
    secondSDA.fillOutput(secondOutput, true, cout);

    firstSDA.crossover(secondSDA);

    cout << "First SDA:" << endl;
    firstSDA.print(cout);
    firstSDA.fillOutput(firstOutput, true, cout);
    firstSDA.mutate(2);
    cout << "First SDA:" << endl;
    firstSDA.print(cout);
    firstSDA.fillOutput(firstOutput, true, cout);

    cout << "Second SDA:" << endl;
    secondSDA.print(cout);
    secondSDA.fillOutput(secondOutput, true, cout);
    secondSDA.mutate(2);
    cout << "Second SDA:" << endl;
    secondSDA.print(cout);
    secondSDA.fillOutput(secondOutput, true, cout);

}

int main(int argc, char* argv[]) {

    int numStates = 100;
    int numChars = 2;
    int popSize = 10;
    int tournSize = 7;
    int gaOperator = 0;
    int numGen = 1;
    int crossOp = 1;
    double crossRate = 0.5;
    int mutOperator = 1;
    double mutRate = 0.5;
    int runs = 1;

    //collect hyper-parameters for the run
    if(argc > 1){
    numStates = atoi(argv[1]);
    numChars = atoi(argv[2]);
    popSize = atoi(argv[3]);
    tournSize = atoi(argv[4]);
    gaOperator = atoi(argv[5]);
    numGen = atoi(argv[6]);
    crossOp = atoi(argv[7]);
    crossRate = atof(argv[8]) / 100;
    mutOperator = atoi(argv[9]);
    mutRate = atof(argv[10]) / 100;
    runs = atoi(argv[11]);
    }
    
    cout << "# States: " << numStates << " # Chars: " << numChars <<
     " popSize: " << popSize << " tournSize " << tournSize <<
      " gaOperator: " << gaOperator << " numGen: " << numGen <<
       " crossOp: " << crossOp  << " crossRate(%): " << 
       setprecision(15) << crossRate << " mutationOperator: " <<
        mutOperator << " mutationRate(%):  " << setprecision(15) <<
         mutRate << " runs: " << runs << endl;

    

    srand(1); // seed the random number generator

    for (int x = 0; x < runs; x++){

        if(gaOperator == 0) Generational G(numStates, numChars, popSize, tournSize, numGen, crossOp, crossRate, mutOperator, mutRate);
        else if(gaOperator == 1) Steady(numStates, numChars, popSize, tournSize, numGen, crossOp, crossRate, mutOperator, mutRate);;
        // else testing();
    }   
    return 0;
}