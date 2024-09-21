#ifndef Topology_H
#define Topology_H

#include <iostream>
#include <vector>
#include <algorithm> 
#include <cmath>
#include <string>
#include <bits/stdc++.h>

using namespace std;

class Topology {
public:
    explicit Topology(int x = 10, int y = 10, int starts = 1, int ends = 1, int numNodes = 30, bool verbose = true);
    explicit Topology(string& fileName, bool verbose = false);
    int ShortestPath(int position, vector<double> &sPath);
    void setConnections(vector<int>& c, bool verbose, int& heurFunction);

    /**
     * The layout of the network that the program will attempt to find the optimal configuration for
     */

    vector<vector<int>> network;
    vector<vector<int>> connections;
    vector<int> layer;
    vector<double> energyConsumption;
    vector<vector<double>> data;
    vector<vector<double>> distance; // 2-d vector tracking the distance between nodes
    vector<vector<double>> trafficMatrix; // 2-d vector tracking the incoming and ougoing traffic from a node in the network
    vector<double> failed;// records data that failed to reach a cloud node
    int numNodes; // variable representing number of transmission nodes present in the network
    int numCNodes;// varialbe representing total number of cloud nodes in the topology
    int numENodes;// number of edge/fog nodes present in the network
    int tNumNodes; // variable representing total number of nodes in the network
    int dataStreams;
    bool analyzeData;

private:
    void readLayout(string& fileName);
    void calculateDist();
    void PrintLayout();
    void printConnections();
    void printTraffic();
    void ChooseNodeLocations(int x, int y, int numNodes);
    void findNode(int &x, int &y, int node);
    void EdgeTraffic(int numStarts, int maxOut = 10000, int upper = 20, int lower = 5);
    void DistributeTraffic();
    void LayerNodes();
    int ChooseStart(int x, int numStarts);
    int ChooseEnd(int y, int x, int numEnds);
    void EnergyConsumption(double transmissionRate = 0.03, double recevingRate = 0.03);
    double round(float var);
};

#endif // Topology_H