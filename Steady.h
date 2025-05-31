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
        explicit Steady(Topology &T, ofstream &MyFile, vector<double>& hyperParameters, bool extend = false);
        explicit Steady(Topology &T, SDA *prePop, int heurAttackFunction, vector<string> &hyperParameters, ofstream &fName);
        explicit Steady(Topology& T, int heurAttackFunction, vector<double>& hyperParameters, ofstream& fName);

    private:
        double distanceFitness(Topology& T);
        double energyFitness(Topology& T);
        double dataFitness(Topology& T);
        double CalcFitness(Topology &T);
        bool necroticFilter(vector<int>& connections, Topology& T);
        void createPath(vector<pair<int, int>> &paths, vector<vector<int>> &nodes, int src, Topology &T);
        vector<bool> dead;

        int PrintPopFits(ostream &outStrm, vector<double> &popFits);
        vector<int> TournSelect(int size, bool decreasing);
        int MatingEvent(SDA *population, Topology& T);
        vector<double> popFits;
        int Evolver(int SDANumStates, int numGenerations, Topology& T, ostream& MyFile, bool preMade = false);
        int PrintReport(ostream &outStrm, vector<double> &popFits, SDA* population, Topology& T);

        int necroticMax = 7;// max number of connections allowed for the necrotic filter
        int necroticMin = 1;// min number of connections allowed by the necrotic filter
        vector<int> rawConnections;

        SDA* preMadePop;
        int numPreMade;
        int preMadeDead = 0;// count number of premade SDA's that died
        bool addDead = false;// tells the program if it's counting the number of dead pre-made SDAs and if it's reporting it
        bool extend = false;// determines if we are taking the 30 best from the inital construction and extending their run
        int attackHeuristic = -1;
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

#endif // Steady_H