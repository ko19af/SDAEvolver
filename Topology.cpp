#include "Topology.h"

/**
 * Constructor for the topology object that will represent the network layour the SDA program will
 * be attempting to construct an optimal configuration for.
*/

Topology::Topology(int x, int y, int starts, int ends, int numNodes, bool verbose){
    
    vector<vector<int>> network(y, vector<int>(x, false));
    this->location = vector<vector<int>>();// vector holding the y and x position of a node in the network
    
    this->network = network;
    this->numNodes = numNodes;
    this->numCNodes = ends;
    this->numENodes = starts;
    this->tNumNodes = numNodes + ends + starts; // calculate number of nodes in Topology

    EdgeTraffic(starts, 100);// distribute the traffic among the edge nodes
    ChooseStart(x, starts);// choose edge node locations
    ChooseNodeLocations(x, y, numNodes);// choose tower node locations
    ChooseEnd(y, x, ends);// choose cloud node locations
    calculateDist();// calculate the distance bettwen the nodes and record their positions
    if(verbose) PrintLayout();
}

Topology::Topology(string& fileName, bool verbose){
    
    readLayout(fileName);// read in the file containing the topology
    
    this->numENodes = 0;
    this->numCNodes = 0;
    this->numNodes = 0;
    this->tNumNodes = 0; // count the total number of nodes in the Topology

    this->location = vector<vector<int>>();// vector holding the y and x position of a node in the network

    for (int y = 0; y < network.size(); y++){// go through the rows
        for (int x = 0; x < network[0].size(); x++){// go through the coloumns
            if(network[y][x] != 0){// if network has a tower at that position
                if(y == 0) numENodes++;// if at first layer of network increment number of edge nodes
                else if(y > 0 && y < network.size()-1) numNodes++;// if in the middle layer increment number of tower nodes
                else numCNodes++;// else increment number of cloud nodes
                location.push_back({y, x});// record tower position in matrix
                tNumNodes++;// increment total number of towers
            }
        }
    }

    EdgeTraffic(numENodes, 100);// distribute the traffic to the edge nodes
    calculateDist();// calculate the distance bettwen the nodes

    if (verbose) PrintLayout();
}

/**
 * This method reads a provided network layout into the program
 * 
 * @param fileName is the name of the file being read into the system
 */

void Topology::readLayout(string& fileName){
    vector<vector<int>> network;
    vector<int> row;
    string text;
    ifstream ReadFile(fileName);
    
    while (getline (ReadFile, text, '\n')){
        stringstream ss(text);// create string stream with text delimited by newline character
        while (getline(ss, text, '\t')) row.push_back(stoi(text));
        network.push_back(row);
        row.clear();
    }
    ReadFile.close();
    this->network = network;
}

double Topology::round(float var){
    float value = (int)(var * 100 + .5);
    return (double)value / 100;
}

/**
 * This method calculates the amount of energy required to recive and transmit the various data streams it is 
 * receving from/to other nodes
 * 
 * Transmit-Energy-Consumed = Transmit-Current * Voltage * Time-for-which-node-transmits-packets
 * Receive-Energy-Consumed = Receive-Current * Voltage * Time -for-which-node-receives-packets
 */

void Topology::EnergyConsumption(double transmissionRate, double recevingRate){
    energyConsumption.clear();// clear the old energy consumption values

    for (int x = numENodes; x < tNumNodes; x++){// for each node
        double incomingData = 0.0;// calculate the total amount of data being transmitted from the node
        for (int d = 0; d < tNumNodes; d++){ // get incoming traffic and distancec values
            incomingData += trafficMatrix[x][d] * distance[x][d];
        }
        energyConsumption.push_back(round(incomingData * transmissionRate));// multiply the number of MBps being transmitted by the energy consumption cost (0.03 W)
    }
}

/**
 * This method will randomly define the traffic emitting from the designated nodes representing an aggregation
 * of various edge and fog nodes in a network.
 * 
 * @param numStarts is the number of nodes pushing data into the network towards the cloud layer
 * @param maxOut is the maximum amount of data a node is permitted to transmit (units in mbps)
 * @param upper is the upperlimit of a data stream emitting from a node (units in mbps)
 * @param lower is the lowerlimit of a data stream emitting from a node (units in mbps)
 */

void Topology::EdgeTraffic(int numStarts, int maxOut, int upper, int lower){
    
    //10,000 Mb = 6,666,666 packets (assuming max packet size)
    // 1 packet = 20 ~ 1,500 bytes
    // 1,500 bytes = .0015 Mb

    this->data = vector<vector<double>>(tNumNodes);
    int dataStreams = 0;// variable to count the number of data streams

    for (int x = 0; x < numStarts; x++){//for each starting node
        double output = 0;// record how much data is being ouputed at the node
        while(output < maxOut){// while the ouput from the node is less than the max ouput
            double d = round((rand()/(double)RAND_MAX)*(upper-lower)+lower);// randomly generate a value in the packet size range
            if(output + d > maxOut) break;// if ouput plus that value is greater than max output break out of loop
            data[x].push_back(d);// if max is not violated add that data to the list of streams eminating from the node
            output += d;// apped the data to the ouput value
            dataStreams++;// increment the number of datastreams that exist
        }
    }
}

/**
 * This method pushes the data through the network based on the connections, established from the SDA,
 * and the layering method. Data is pushed through a connection based on the cumulative amount of data 
 * being pushed to a node,meaning the node receving the least amount of data will receive the data stream
 * (i.e. a greedy approach to data distribution).
 * 
*/

void Topology::DistributeTraffic(){
    this->trafficMatrix = vector<vector<double>> (tNumNodes, vector<double>(tNumNodes));

    for (int x = numENodes; x < tNumNodes; x++) data[x].clear(); // clear distribution from previous runs (except at edge nodes)
    
    vector<int> toDo;// vector containing the nodes that have data to distribute
    vector<vector<double>> toDistribute(tNumNodes);// vector containing data to distribute at each node

    for(int x = 0; x < numENodes; x++){// for all the edge nodes
        toDo.push_back(x);// initialize vector with the edge nodes
        for(double d : data[x]) toDistribute[x].push_back(d);// place data needing distribution in the toDistribute vector
    }

    while(toDo.size() != 0){// while there are nodes with data to distribute
        int node = toDo[0];// get node node id from toDo
        toDo.erase(toDo.begin());// remove node id from toDo

        while(toDistribute[node].size() != 0){// while their is data to distribute from the node
            double d = toDistribute[node][0];// get the data being distributed
            toDistribute[node].erase(toDistribute[node].begin());// remove data from distribution list

            int insert = node;// node receiving the data
            double best = DBL_MAX;// lowest impact
            for (int x = numENodes; x < tNumNodes; x++){// go through all the connections to the node
                if (node != x && connections[node][x] == 1 && trafficMatrix[node][x] + d < best){
                    if(layer[insert] < layer[x]){// prioritse sending data to higher layer nodes
                        insert = x;// sent insert to node that satisfys above parameters
                        best = trafficMatrix[node][x] + d;// set best as current data plus new data
                    }else if(layer[insert] == layer[x] && x >= numENodes){// send to equal layer node if it is not an edge node
                        insert = x;// sent insert to node that satisfys above parameters
                        best = trafficMatrix[node][x] + d;// set best as current data plus new data
                    }
                }
            }

            if(insert >= numENodes + numNodes){// if data is sent to a cloud node
                trafficMatrix[node][insert] += d;// update traffic matrix with the new data transmitted between nodes
                trafficMatrix[insert][node] += d;// update traffic matrix with the new data transmitted between nodes
                data[insert].push_back(d);// record packet stream being sent/recived to node
            } else{// data is sent to another node in network
                trafficMatrix[node][insert] += d;
                trafficMatrix[insert][node] += d;
                data[insert].push_back(d);
                toDistribute[insert].push_back(d);// add data to be distributed from node
                if(count(toDo.begin(), toDo.end(), insert) < 1) toDo.push_back(insert);// add node to list of nodes that must distribute data (if not already added)
            }
        }
    }
}

/**
 * This method uses a Breadth-First Search to place the nodes in a queue to 
 * determine the order the nodes will have their
 * data distributed, this is done to avoid issues with bottlenecks and dead ends.
 */

void Topology::LayerNodes(){
    this->layer = vector<int>(tNumNodes, -1);// vector to hold the queue order (layer) of the nodes
    vector<int> checking;// vector holding the current layer of nodes being numbered
    vector<int> toCheck;// vector holding the next layer of nodes to be numbered

    for (int x = (numENodes + numNodes); x < tNumNodes; x++) checking.push_back(x);// add cloud nodes to checking as start point
    
    int level = tNumNodes;// initialize layer level

    while(checking.size() != 0){// go through checking vector until none left to check
        int check = checking[0];// get first node from checking and set it as element being checked
        checking.erase(checking.begin()); // remove node from checking

        if (layer[check] == -1){// if node has not been assigned to a layer
            layer[check] = level;// set the layer of the node 
            for (int x = 0; x < tNumNodes; x++){// add the nodes connected to this node to the group being checked later
                if(connections[check][x] == 1 && layer[x] == -1) toCheck.push_back(x);// if node is connected to check add to toCheck
            }
        }

        if(checking.size() == 0 && toCheck.size() != 0){//if current layer is completed and more nodes need to be layered
            for (int x = 0; x < toCheck.size(); x++) checking.push_back(toCheck[x]);// move toCheck elements to checking
            toCheck.clear();// clear the elements from the toCheck matrix
            level--;// move to next level (as old level is finished)
        }
    }
}

/** This method prints the resulting layout of the nodes in the network topology
 * 
 */

void Topology::PrintLayout(){
    cout << "Topology:" << endl;
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
}

/** This mehtod prints the connections present in the network topology
 * 
 */

void Topology::printConnections(){
    cout << "Connections:" << endl;
    for (int y = 0; y < connections.size(); y++){
        for (int x = 0; x < connections[0].size(); x++){
            cout << connections[y][x] << ' ';
        }
        cout << '\n';
    }   
}

/** This method prints the traffic flowing through the nodes in the network
 * 
 */

void Topology::printTraffic(){
        cout << "Traffic: " << endl;
    for (int y = 0; y < trafficMatrix.size(); y++){
        for (int x = 0; x < trafficMatrix[0].size(); x++){
            cout << trafficMatrix[y][x] << ' ';
        }
        cout << '\n';
    }
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
        } while (network[0][start] == 1);
        network[0][start] = 1; // set node location in the network
        location.push_back({0, start}); // record node location in the network
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
        } while (network[y-1][end] == 1); // if position was already choosen choose another
        network[y-1][end] = 1;
        location.push_back({y - 1, end});
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
        } while (network[row][column] == 1); // if position is already choosen choose another
        network[row][column] = 1;
        location.push_back({row, column});
    }
}

/**
 * This method calculates the shortest path (i.e. euclidean distance) between two nodes in
 * the network topology
 * 
 * @param position is the node being travelled from currently
 * @param sPath is the shortest path from the start node to any other node in the network it has a path to
*/

void Topology::ShortestPath(int src, vector<double> &sPath, vector<vector<int>> &nodes){// initialize distance from start node to all others at max value)
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    pq.push(make_pair(0, src));// push starting point with inital distance of zero into priority queue
    sPath[src] = 0;// set initial distance to source as zero
    while(!pq.empty()){// while there is still a node to examine
        int u = pq.top().second;// get the node being examined from the pq
        pq.pop();// remove element from the queue
        for (int x = 0; x < tNumNodes; x++){// look at all of the nodes connections
            if(connections[u][x] == 0) continue;// if there is no connection to that node skip it
            double weight = distance[u][x];// reterive distance to connected node
            if(sPath[x] > sPath[u] + weight){// if there is a shorter path to that node
                sPath[x] = sPath[u] + weight;// update the shorter path
                pq.push(make_pair(sPath[x], x));// push the update onto the queue
                if(!nodes[u].empty()) nodes[x] = (nodes[u]);// set the path to get from node x to the same as node u (as long as its not empty)
                nodes[x].push_back(u);// add node u to the path as it must be traversed to reach node x
            }
        }
    }
}

/**
 * This method creates the priority queue holding the edges of the network in non-decreasing order
 * @param edges is the priority queue consisting of tuples contianing the edges and their distance
 */

void Topology::makeEdges(auto &edges){
    for (int y = 0; y < tNumNodes; y++){//for all the nodes
        for (int x = 0; x < y; x++){// go through their connections, stopping at thier own column
            // if there is no edge then skip, else create edge and add to the queue
            (connections[y][x]) ? 0 continue : edges.push_back(make_tuple(distance[y][x], y, x))
        }
    }
}

/**
 * This method determines the which connections are required at minimum to connect the network
 * @param newNet is the new network produced by the MST algorithm
 */

double Topology::MinimumNetwork(vector<vector<int>> &newNet){
    priority_queue<tuple<int,int,int>, vector<tuple<int,int,int>>, greater<tuple<int,int,int>>> edges;
    makeEdges(edges);// make the edges of the network
    double cost = 0, count = 0;

    vector<int> parent(tNumNodes), rank(tNumNodes, 1);// initialize the parents of the nodes and thier ranks
    for (int x = 0; x < parent.size(); x++) parent[x] = x;// initialize parent of nodes as themselves

    while(!edges.empty()){// while there is an edge to examine
        int x = get<0>(edges.top()), y = get<1>(edges.top()), w = get<2>(edges.top());// get info about edge from tuple
        edges.pop();// remove edge

        int parentx = find(x, parent), parenty = find(y, parent);// find the parents of the two nodes

        if(parentx != parenty){// if the nodes do not have a common parent (i.e are disconnected)
            unite(parentx, parenty, parent, rank);// set the nodes to have a common parent
            //cost+=w;// include the cost of the added edge
            newNet[y][x] = newNet[x][y] = 1;// set the connection in the new network
            if (++count == tNumNodes - 1) break;// if reached minimum number of edges for the network
        }else{
            cost += w;// add the cost of the unused connection
        }
    }
    return cost;// return cost of mimimum spanning network
}

/**
 * This method determines the parent of a node by backtracking to the node that is a parent of itself
 * @param i is the node being examined
 * @param parent is the vector recording which node has which parent
 */

bool Topology::find(int i, vector<int> &parent){
    return (parent[i] == i) ? i : (parent[i] = find(parent[i], parent));
}

/**
 * This method sets the parent of two nodes in the network so they are the same, thus adding the edge to the min network
 * @param s1 is the parent of one node
 * @param s2 is the parent of the other node
 * @param parent is the vector recording the parents of the nodes
 * @param rank is the vector recording the ranks of the nodes
 */

void Topology::unite(int s1, int s2, vector<int> &parent, vector<int> &rank) {
    if (rank[s1] < rank[s2]) parent[s1] = s2;// if node xs parent has a better rank it becomes the parent of node y
    else if (rank[s1] > rank[s2]) parent[s2] = s1;// vice versa of previous statement
    else parent[s2] = s1, rank[s1]++;// if they have the same rank then it does not matter
}

/**
 * This method calculates the distance between all the nodes and record thier position in the matrix
 */

void Topology:: calculateDist(){
    this->distance = vector<vector<double>>(tNumNodes, vector<double>(tNumNodes)); // vector for recording the distance
    
    for (int y = 0; y < tNumNodes; y++){// select origin node
        for (int x = 0; x < y; x++){// select the node we want to calculate distance to
            
            int y1 = location[y][0];//retreive location of nodes in the network
            int x1 = location[y][1];
            int y2 = location[x][0];
            int x2 = location[x][1];

            distance[y][x] = distance[x][y] = sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));// set distance values between the nodes
             
        }
    }
}

/**
 * This method takes the SDA vector and translates it into the connections
 * present in the network topology and then layers the nodes to check all
 * edge nodes connect to a cloud node
 * 
 * (For non-attacked networks)
 * 
 * @param attacked decides if the filter is examaning an attacked network or not
 * @param c is the vector produced by the SDA detainling the connections present in the network
 * @return is the boolean determing if this network connects all edge nodes to their cloud nodes
*/

bool Topology::setConnections(bool attacked, vector<int>& c){
    
    if(!attacked){// if network is not attacked
        this->connections = vector<vector<int>>(tNumNodes, vector<int>(tNumNodes));// 2-d vector representing connections between nodes
        for (int y = 0; y < connections.size(); y++){// fill the connection matrix
            for (int x = 0; x < y; x++){
                if(y >= (numNodes+numENodes) && x < numENodes){// set connections between edge & cloud nodes to zero
                    connections[y][x] = 0;
                    connections[x][y] = 0;
                }else{// set connection based on vector given from SDA
                    connections[y][x] = c[0];
                    connections[x][y] = c[0];
                }
                c.erase(c.begin()); // remove node from checking
            }
        }
    }

    LayerNodes();// layer the nodes in the network

    for (int x = 0; x < numENodes; x++){// go through the edge nodes
        if(layer[x] == -1) return true;// if edge node is not connected to cloud node network is dead
    }
    
    return false;// return true if all edge nodes connect to a cloud node
}

/**
 * This method loads the additional informaiton from the network connections once
 * the network has been determined to be not dead
 * 
 * @param heurFunction is the heurestic being used to evaluate the network
 * @param verbose is used to print the infrmation of the network
 */

void Topology::configNet(int& heurFunction, bool verbose){
    if(heurFunction == 0) return;// if only using distance as heurestic
    else if (heurFunction == 1) DistributeTraffic();// calculate throughput of nodes
    else{ // calculate energy consumption of the network
        DistributeTraffic();
        EnergyConsumption();
    }

    if(verbose && heurFunction > 0){
        printConnections();
        printTraffic();
    }else if(verbose) printConnections();
}