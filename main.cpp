#include <iostream>
#include <cstdlib>
#include "SDA.h"
#include "Topology.h"
#include "Generational.h"
#include "Steady.cpp"
#include <bits/stdc++.h>

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

    //collect hyper-parameters for the run
    // int numStates = atoi(argv[1]);
    // int numChars = atoi(argv[2]);
    // int popSize = atoi(argv[3]);
    // int tournSize = atoi(argv[4]);
    //int gaOperator = atoi(argv[5]);
    // int numGen = atoi(argv[6]);
    // int crossOp = atoi(argv[7]);
    // double crossRate = atoi(argv[8]) / 100;
    // int mutOperator = atoi(argv[9]);
    // double mutRate = atoi(argv[10]) / 100;
    // int runs = atoi(argv[11]);

    //if(gaOperator == 0) Generational G(numStates, numChars, popSize, tournSize, numGen, crossOp, crossRate, mutOperator, mutRate);
    //else if(gaOperator == 1) Steady;
    //else testing();
    return 0;
}