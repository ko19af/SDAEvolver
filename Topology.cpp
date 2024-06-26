#include "Topology.h"

/**
 * Constructor for the topology object that will represent the network layour the SDA program will
 * be attempting to construct an optimal configuration for.
*/

Topology::Topology(int x, int y, int starts, int ends, int numNodes){
    
    vector<vector<bool>> network(y, vector<bool>(x, false));

    this->network = network;
    this->numNodes = numNodes;
    this->numCNodes = ends;
    this->numENodes = starts;
    this->tNumNodes = numNodes + ends + starts; // calculate number of nodes in Topology

    srand(time(0));

    ChooseStart(x, starts);
    ChooseEnd(y, x, ends);
    ChooseNodeLocations(x, y, numNodes);
    PrintLayout();
}

void Topology::PrintLayout(){
    int nodeID = 1;
    for (int y = 0; y < network.size(); y++)
    {
        for (int x = 0; x < network[0].size(); x++){
            if(network[y][x] == 1){
                cout << nodeID << '\t';
                nodeID++;
            }else cout << network[y][x] << '\t';
        }
        cout << '\n';
    }
    cout << "Topology" << endl;
}

void Topology::printConnections(){
    for (int y = 0; y < connections.size(); y++){
        for (int x = 0; x < connections[0].size(); x++){
            cout << connections[y][x] << ' ';
        }
        cout << '\n';
    }
    cout << "Connections" << endl;
}

/**
 * This will randomly select the start location(s) the information is originating from in the network
 * from the first row in the 2-d array representing the topology
 * 
 * @param x is the column limit of the network topology
 * @param numStarts is the number of edge layer nodes that will be added to the topology
*/

int Topology::ChooseStart(int x, int numStarts){
    
    for (int i = 0; i < numStarts; i++){
        int start;
        do{
            start = rand() % x; // randomly choose the data starting position in the network
        } while (network[0][start] == true);
        network[0][start] = true;
    }
    return 0;
}

/**
 * This will randomly select the end lolcations the data in our network is attempting to reach
 * these will be located in the row furthest from the row the start locations are positioned
 * 
 * @param y is the row limit of the grid (last row in the grid) representing the network topology
 * @param x is the column limit of the grid
 * @param numEnds is the number of cloud layer nodes present in the topology
*/

int Topology::ChooseEnd(int y, int x, int numEnds){
    for (int i = 0; i < numEnds; i++){
        int end;
        do{
            end = rand() % x; // randomly choose the data starting position in the network
        } while (network[y-1][end] == true); // if position was already choosen choose another
        network[y-1][end] = true;
    }
    return 0;
}

/**
 * This method randomly selects where the fog layer nodes in the network will be placed
 * 
 * @param x is the column limit of the grid representing the network topology
 * @param y is the row limit of the grid representing the network topology
 * @param numNodes is the number of fog layer nodes that will go into the network
*/

void Topology::ChooseNodeLocations(int x, int y, int numNodes){
    for (int z = 0; z < numNodes; z++){
        int row;
        int column;
        do{
            row = 1 + rand() % (y-2); // randomly choose row between 1-8 to insert node
            column = rand() % x;// randomly choose column to insert node
        } while (network[row][column] == true); // if position is already choosen choose another
        network[row][column] = true;
    }
}

/**
 * This method calculates the shortest path (i.e. euclidean distance) between two nodes in
 * the network topology
 * 
 * @param position is the node being travelled from currently
 * @param sPath is the shortest path from the start node to any other node in the network it has a path to
*/

int Topology::ShortestPath(int position, vector<double> &sPath){// initialize distance from start node to all others at max value)

    for (int x = 0; x < connections.size(); x++){ // go through row recording the connections for current position
        if (connections[position][x] == 1 && position != x){ // if there is a connection to explore
            int x1, x2, y1, y2;
            findNode(x1, y1, position + 1); // find x and y co-ordinate of node we are at
            findNode(x2, y2, x + 1); // find x and y co-ordinate of node we wish to calculate distance to
            int dist = sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2)); // caluclate euclidean distance
            if (dist + sPath[position] < sPath[x]){// compare distance
                sPath[x] = dist + sPath[position]; // if shorter path update distance in shorter path vector
                ShortestPath(x, sPath);// recalculate distance to all other nodes from that node to find shorter paths
                }
            }
        }

    return 0;
}

/**
 * This method finds the location of the nodes the program is calculating the euclidean
 * distance for in the topology of the network
 *
 * @param x is the designated column of the node in the topology
 * @param y is the designated row of the node in the  topology
 * @param node is the node the program is looking for in the Topology
*/

void Topology::findNode(int &x, int &y, int node){
    int count = 0;// keep track of what node we have found in the network
    for (y = 0; y < network.size(); y++){// select row we will look through
        for (x = 0; x < network[0].size(); x++){// select column
            if(network[y][x] == true) count++;// if there is a node at position increment count
            if(count == node) return;// if found the node return to call
        }
    }
}

/**
 * This method takes the SDA vector and translates it into the connections
 * present in the network topology
 * 
 * @param c is the vector produced by the SDA detainling the connections present in the network
*/

void Topology::setConnections(vector<int> c, bool verbose){
    
    vector<vector<int>> connections({{0,1,0,1,0},
                                     {1,0,1,0,0},
                                     {0,1,0,0,1},
                                     {1,0,0,0,1},
                                     {0,0,1,1,0}});

    // vector<vector<int>> connections(tNumNodes, vector<int>(tNumNodes, 0));
    // int pos = 0;// keep track of position in SDA connection vector
    // for (int y = 0; y < connections.size(); y++){// fill the connection matrix
    //     for (int x = 0; x < connections[0].size(); x++){
    //         connections[y][x] = c[pos];
    //         connections[x][y] = c[pos];
    //         pos++;// increment position in SDA connection vector
    //     }
    // }

    this->connections = connections;
    if(verbose) printConnections();
}