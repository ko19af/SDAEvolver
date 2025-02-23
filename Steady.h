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
        explicit Steady(Topology& T, int heurFunction, ofstream& fName);

    private:
        double distanceFitness(Topology& T);
        double energyFitness(Topology& T);
        double dataFitness(Topology& T);
        double CalcFitness(Topology &T);
        bool necroticFilter(vector<int>& connections, Topology& T);
        bool attNecroticFilter(Topology& T);
        vector<bool> dead;

        int PrintPopFits(ostream &outStrm, vector<double> &popFits);
        vector<int> TournSelect(int size, bool decreasing);
        int MatingEvent(SDA *population, Topology& T);
        vector<double> popFits;
        int Evolver(int SDANumStates, int numGenerations, Topology& T, ostream& MyFile);
        int PrintReport(ostream &outStrm, vector<double> &popFits, SDA* population);

        int necroticMax = 7;// max number of connections allowed for the necrotic filter
        int necroticMin = 1;// min number of connections allowed by the necrotic filter
        vector<int> rawConnections;

        double popWorstFit;
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
};

#endif // Topology_H