#ifndef Generational_H
#define Generational_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>
#include "SDA.h"
#include "Topology.h"

using namespace std;

class Generational {

    public:
        explicit Generational(Topology& T, ostream& MyFile, int numStates, int numChars, int popSize, int tournSelector, int numGen, int crossOp, double crossRate, int mutOperator, double mutRate, int heurFunction);
        double genCalcFitness(SDA &member, Topology& T);//move to private when finished testing!!!!!!!!!!!!!!!!!!!
        

    private:
        
        int genPrintPopFits(ostream &outStrm, vector<double> &popFits);
        int genMatingEvent(SDA *currentPop, SDA *newPop, Topology T);
        int genEvolver(int SDANumStates = 100, int SDAOutputLen = 10, int numGenerations = 20, Topology T = Topology(5,5,1,1,3), ostream& MyFile = cout);
        double dataFitness(Topology& T);
        double distanceFitness(Topology& T);
        double energyFitness(Topology& T);
        bool genCompareFitness(int popIdx1, int popIdx2);
        vector<int> genTournSelect(int size, bool decreasing);

        vector<double> genPopFits;
        vector<double> genNewPopFits;
        int heurFunction = 0;
        int genSDANumChars = 2;
        int genSDAResponseLength = 2;
        int genPopSize = 100;
        int genTournSize = 7;
        int tournCandidates = 4;
        int tournMaxRepeats = 2;
        int elitism = 2;
        bool genLowerBetter = false;
        double genMutationRate = 0.1;
        int numGenerations = 100;
        int genNumMutations = 1;
        int genMutOperator = 1;
        int genCrossOp = 1;
        double genCrossRate = 0.5;
        int maxConnections;
};

#endif // Topology_H