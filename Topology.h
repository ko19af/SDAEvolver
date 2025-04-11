#ifndef Topology_H
#define Topology_H

#include <iostream>
#include <vector>
#include <algorithm> 
#include <cmath>
#include <string>
#include <bits/stdc++.h>
#include "SDA.h"

using namespace std;

class Topology {
public:
    explicit Topology(int x = 10, int y = 10, int starts = 1, int ends = 1, int numNodes = 30, bool verbose = false, int attFunction = -1);
    explicit Topology(string& fileName, bool verbose = false, int attFunction = -1);
    explicit Topology(vector<vector<int>>, int attFunction = -1);
    void ShortestPath(int position, vector<double>& sPath, vector<vector<int>> &nodes);
    void minimumNetwork(vector<vector<int>> &newNet, double& excess);
    bool setConnections(vector<int>& c, int AttackFunction);
    void configNet(int &heurFunction, bool verbose = false);

    /**
     * The layout of the network that the program will attempt to find the optimal configuration for
     */

    vector<vector<int>> network;
    vector<vector<int>> location;// vecto holding the location of the nodes in the network
    vector<vector<int>> connections;
    vector<int> layer;
    vector<double> energyConsumption;
    vector<vector<double>> data;// records the individual data streams a node is processing
    vector<vector<double>> attackData;// records the attack streams
    vector<vector<double>> distance; // 2-d vector tracking the distance between nodes
    vector<vector<double>> trafficMatrix; // 2-d vector tracking the incoming and ougoing traffic from a node in the network
    vector<bool> attTowers; // records which towers are being attacked in the simulation
    int numNodes; // variable representing number of transmission nodes present in the network
    int numCNodes;// varialbe representing total number of cloud nodes in the topology
    int numENodes;// number of edge/fog nodes present in the network
    int tNumNodes; // variable representing total number of nodes in the network
    int dataStreams;
    bool analyzeData;
    int numPreMadeSDA = 0;
    SDA *preMadePop; // pre-made SDA population

private:
    void countNodes(int attFunction = -1);
    void makeEdges(vector<tuple<double, int, int>> &edges);
    int find(int i, vector<int>& parent);
    void unite(int x, int y, vector<int> &parent, vector<int> &rank);
    void readLayout(string &fileName);
    void calculateDist();
    void PrintLayout();
    void printConnections();
    void printTraffic();
    void ChooseNodeLocations(int x, int y, int numNodes);
    void EdgeTraffic(int numStarts, int maxOut = 10000, int upper = 20, int lower = 5);
    void DistributeTraffic();
    void LayerNodes();
    int ChooseStart(int x, int numStarts);
    int ChooseEnd(int y, int x, int numEnds);
    void EnergyConsumption(double transmissionRate = 0.03, double recevingRate = 0.03);
    double round(float var);

   
};

#endif // Topology_H