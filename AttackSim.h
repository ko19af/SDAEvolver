#ifndef AttackSim_H
#define AttackSim_H

#include <iostream>
#include <vector>
#include <algorithm> 
#include <cmath>
#include <string>
#include <bits/stdc++.h>
#include "Topology.h"
#include "Steady.h"

using namespace std;

class AttackSim {
public:
    explicit AttackSim(string& fileName, bool verbose = false, int heurFunction = 0);

private:
    vector<vector<int>> network;// vector holding the network layout
    vector<vector<int>> location;// vector holding the location of the nodes in the network
    vector<vector<int>> connections;// vector holding the connection layout
    vector<int> attTowers;

    int numENodes;
    int numCNodes;
    int numNodes;
    int tNumNodes;

    void readLayout(string &fileName);
    void performTowerAttack(int actTowers, int remTowers = 1);
    void readEData(string &fileName); // read the experiment data (connections and topology used)
    bool split(string input, char del, string &c);
};

#endif // AttackSim_H