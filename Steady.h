#ifndef Steady_H
#define Steady_H

#include <iostream>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>
#include "SDA.h"
#include "Topology.h"

using namespace std;

class Steady {
    public:
        explicit Steady(int numStates, int numChars, int popSize, int tournSize, int numGen, int crossOp, double crossRate, int mutOperator, double mutRate, int heurFunction);
        double CalcFitness(SDA &member, Topology T);//move to private when finished testing!!!!!!!!!!!!!!!!!!!

    private:
        double distanceFitness(Topology T);
        double energyFitness(Topology T);
        double dataFitness(Topology T);

        int PrintPopFits(ostream &outStrm, vector<double> &popFits);
        bool CompareFitness(int popIdx1, int popIdx2);
        vector<int> TournSelect(int size, bool decreasing);
        int MatingEvent(SDA *population, Topology T);
        vector<double> popFits;
        int Evolver(int SDANumStates = 100, int SDAOutputLen = 10, int numGenerations = 20, Topology T = Topology(5,5,1,1,5,true));
        int PrintReport(ostream &outStrm, vector<double> &popFits, SDA* population);

        int heurFunction = 0;
        int SDANumChars = 2;
        int SDAResponseLength = 2;
        int popSize = 100;
        int tournSize = 7;
        int tournCandidates = 4;
        int tournMaxRepeats = 10;
        int elitism = 2;
        bool lowerBetter = false;
        double mutationRate = 0.1;
        int numGenerations = 100;
        int numMutations = 1;
        int mutOperator = 1;
        int crossOp = 1;
        double crossRate = 0.5;
        int numColoumns = 5;
        int numRows = 5;
        int numStarts = 1;
        int numEnds = 1;
        int numNodes = 3;
        int maxConnections;
};

#endif // Topology_H