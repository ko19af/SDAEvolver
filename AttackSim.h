#ifndef AttackSim_H
#define AttackSim_H

#include <iostream>
#include <vector>
#include <algorithm> 
#include <cmath>
#include <string>
#include <bits/stdc++.h>
#include <random>
#include "Topology.h"
#include "Steady.h"
#include "SDA.h"
#include <fstream>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

class AttackSim {
public:
    explicit AttackSim(int heurFunction = 0, double attTowers = .05, bool verbose = false, string path = "Output");
    bool split(string input, char del, string &c);

private:
    vector<vector<int>> network;// vector holding the network layout
    vector<vector<int>> location;// vector holding the location of the nodes in the network
    vector<vector<vector<int>>> networkCon;// vector holding all the network connections from a file
    vector<bool> attTowers;// vector recording the towers being attacked
    vector<vector<vector<int>>> topologies;
    SDA* population;

    int heurFunction = 0;
    int SDAResponseLength = 2;
    int popSize = 100;
    double mutationRate = 0.1;
    int numGenerations = 100;
    int numMutations = 1;
    int mutOperator = 1;
    int crossOp = 1;
    double crossRate = 0.5;

    void selectAttackedTowers(int numTowers, Topology& T);
    void performTowerAttack();
    void performDataAttack(Topology &T, int maxOut = 10000, int upper = 20, int lower = 5);
    void readTopologies();
    Topology readEData(const std::filesystem::__cxx11::path &filePath, vector<string> &hyperParameters); // read the experiment data (connections and topology used)
    void readHyperParameters(string input, vector<string> &hyperParameters);
};

#endif // AttackSim_H