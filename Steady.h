#ifndef Steady_H
#define Steady_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>
#include "SDA.h"
#include "Topology.h"

using namespace std;

class Steady {
    public:
        explicit Steady(Topology& T, ofstream& MyFile, int numStates, int numChars, int popSize, int tournSize, int numGen, int crossOp, double crossRate, int mutOperator, double mutRate, int heurFunction);
        double CalcFitness(SDA &member, Topology& T);//move to private when finished testing!!!!!!!!!!!!!!!!!!!

    private:
        double distanceFitness(Topology& T);
        double energyFitness(Topology& T);
        double dataFitness(Topology&T);

        int PrintPopFits(ostream &outStrm, vector<double> &popFits);
        vector<int> TournSelect(int size, bool decreasing);
        int MatingEvent(SDA *population, Topology& T);
        vector<double> popFits;
        int Evolver(int SDANumStates, int SDAOutputLen, int numGenerations, Topology& T, ostream& MyFile);
        int PrintReport(ostream &outStrm, vector<double> &popFits, SDA* population);

        int heurFunction = 0;
        int SDANumChars = 2;
        int SDAResponseLength = 2;
        int popSize = 100;
        int tournSize = 7;
        int tournCandidates = 4;
        int tournMaxRepeats = 10;
        int elitism = 2;
        bool lowerBetter = true;
        double mutationRate = 0.1;
        int numGenerations = 100;
        int numMutations = 1;
        int mutOperator = 1;
        int crossOp = 1;
        double crossRate = 0.5;
        int maxConnections;
};

#endif // Topology_H