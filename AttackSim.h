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

#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

class AttackSim {
public:
    explicit AttackSim(int heurFunction = 0, double attTowers = .05, bool verbose = false, string& fileName);

private:
    vector<vector<int>> network;// vector holding the network layout
    vector<vector<int>> location;// vector holding the location of the nodes in the network
    vector<vector<vector<int>>> networkCon;// vector holding all the network connections from a file
    vector<int> attTowers;// vector recording the towers being attacked
    Topology T;

    void selectAttackedTowers(int numTowers);
    void performTowerAttack();
    void readEData(const auto filePath); // read the experiment data (connections and topology used)
    bool split(string input, char del, string &c);
};

#endif // AttackSim_H