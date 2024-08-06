#ifndef Generational_H
#define Generational_H

#include <iostream>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>
#include "SDA.h"
#include "Topology.h"

using namespace std;

class Generational {
    public:
        explicit Generational(int numStates, int numChars, int popSize, int tournSelector, int numGen, int crossOp, double crossRate, int mutOperator, double mutRate);
        double genCalcFitness(SDA &member);//move to private when finished testing!!!!!!!!!!!!!!!!!!!

    private:
        
        int genPrintPopFits(ostream &outStrm, vector<double> &popFits);
        bool genCompareFitness(int popIdx1, int popIdx2);
        vector<int> genTournSelect(int size, bool decreasing);
        int genMatingEvent(SDA *currentPop, SDA *newPop);
        vector<double> genPopFits;
        int genEvolver(int SDANumStates = 100, int SDAOutputLen = 10, int numGenerations = 20);

        int genSDANumChars = 2;
        int genSDAResponseLength = 2;
        int genPopSize = 100;
        int genTournSize = 7;
        int tournCandidates = 4;
        int tournMaxRepeats = 10;
        int elitism = 2;
        bool genLowerBetter = false;
        double genMutationRate = 0.1;
        int numGenerations = 100;
        int genNumMutations = 1;
        int genMutOperator = 1;
        int genCrossOp = 1;
        double genCrossRate = 0.5;
        Topology T;
};

#endif // Topology_H