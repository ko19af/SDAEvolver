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

    vector<vector<double>> trafficMatrix(tNumNodes, vector<double>(tNumNodes, 0.0));
    this->trafficMatrix = trafficMatrix;

    EdgeTraffic(starts, 100);
    ChooseStart(x, starts);
    ChooseEnd(y, x, ends);
    ChooseNodeLocations(x, y, numNodes);
    PrintLayout();
}

/**
 * This method calculates the amount of energy required to recive and transmit the various data streams it is 
 * receving from/to other nodes
 * 
 * Transmit-Energy-Consumed = Transmit-Current * Voltage * Time-for-which-node-transmits-packets
 * Receive-Energy-Consumed = Receive-Current * Voltage * Time -for-which-node-receives-packets
 */

void Topology::EnergyConsumption(double transmissionRate, double recevingRate){
    vector<double> energyConsumption(tNumNodes-numENodes, 0.0);

    for (int x = numENodes; x < tNumNodes; x++){// for each node
        double incomingData = 0.0;// calculate the total amount of data being transmitted from the node
        for (int d = 0; d < tNumNodes; d++){// get (d)ata values from the traffic matrix
            incomingData += trafficMatrix[x][d];
        }
        energyConsumption[x] = incomingData * transmissionRate;// multiply the number of MBps being transmitted by the energy consumption cost (0.03 W)
    }
    this->energyConsumption = energyConsumption;
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
    
    vector<vector<double>> data(tNumNodes);
    int dataStreams = 0;// variable to count the number of data streams

    for (int x = 0; x < numStarts; x++){//for each starting node
        double output = 0;// record how much data is being ouputed at the node
        while(output < maxOut){// while the ouput from the node is less than the max ouput
            double d = (rand()/(double)RAND_MAX)*(upper-lower)+lower;// randomly generate a value in the packet size range
            if(output + d > maxOut) break;// if ouput plus that value is greater than max output break out of loop
            data[x].push_back(d);// if max is not violated add that data to the list of streams eminating from the node
            output += d;// apped the data to the ouput value
            dataStreams++;// increment the number of datastreams that exist
        }
        trafficMatrix[x][x] = output;// the ouput value in the traffic matrix for the start node
    }
    this->data = data;// set local data vector as global data vector
    this->dataStreams = dataStreams;// set local datastream variable as global data stream variable
}

/**
 * This method pushes the data through the network based on the connections, established from the SDA,
 * and the layering method. Data is pushed through a connection based on the cumulative amount of data 
 * being pushed to a node,meaning the node receving the least amount of data will receive the data stream
 * (i.e. a greedy approach to data distribution).
 * 
*/

void Topology::DistributeTraffic(){

    for (int x = numENodes; x < tNumNodes; x++) data[x].clear(); // clear distribution from previous runs (except at edge nodes)

    vector<int> toDo;// vector containing the nodes that have data to distribute
    vector<vector<double>> toDistribute(tNumNodes);// vector containing data to distribute at each node
    vector<double> failed(dataStreams);// create vector to record datastreams that failed to reach target destination

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

            int insert = node;// node receinving the data
            double best = DBL_MAX;// lowest impact
            for (int x = numENodes; x < tNumNodes; x++){// go through all the connections to the node
                if (node != x && connections[node][x] == 1 && trafficMatrix[node][x] + d < best){
                    if(layer[insert] < layer[x]){// prioritse sending data to higher layer nodes
                        insert = x;// sent insert to node that satisfys above parameters
                        best = trafficMatrix[node][x] + d;// set best as current data plus new data
                    }else if(layer[insert] == layer[x]){
                        insert = x;// sent insert to node that satisfys above parameters
                        best = trafficMatrix[node][x] + d;// set best as current data plus new data
                    }
                }
            }

            if(insert >= numENodes + numNodes){// if data is sent to a cloud node
                trafficMatrix[node][insert] += d;// update traffic matrix with the new data transmitted between nodes
                data[insert].push_back(d);// record packet stream being sent/recived to node
            } else if (insert != node && insert < numENodes + numNodes){// data is sent to another node in network
                trafficMatrix[node][insert] += d;
                data[insert].push_back(d);
                toDistribute[insert].push_back(d);// add data to be distributed from node
                if(count(toDo.begin(), toDo.end(), insert) < 1) toDo.push_back(insert);// add node to list of nodes that must distribute data (if not already added)
            }else{// if data has no place to go & did not reach a cloud node
                failed.push_back(d);
            }
        }
    }
    this->failed = failed;// set local failed to global failed
}

/**
 * This method places the nodes in a queue to determine the order in which order the nodes will have their
 * data distributed, this is done in order to avoid issues with bottlenecks.
 */

void Topology::LayerNodes(){
    vector<int> layer(tNumNodes, -1);// vector to hold the queue order of the nodes
    vector<int> checking;
    vector<int> toCheck;

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

        if(checking.size() == 0 && toCheck.size() != 0){//if current layer is completed and mode nodes need to be layered
            for (int x = 0; x < toCheck.size(); x++) checking.push_back(toCheck[x]);// move toCheck elements to checking
            toCheck.clear();// clear the elements from the toCheck matrix
            level--;// move to next level (as old level is finished)
        }
    }

    this->layer = layer;// store layering
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

void Topology::setConnections(vector<int> c, bool verbose, bool analyzeData){
    vector<vector<int>> connections(tNumNodes, vector<int>(tNumNodes, 0));
    int pos = 0;// keep track of position in SDA connection vector
    
    for (int y = 0; y < connections.size(); y++){// fill the connection matrix
        for (int x = 0; x < y; x++){
            connections[y][x] = c[pos];
            connections[x][y] = c[pos];
            if(c[pos] == 1) numConnections++;// if there is a connection between the nodes increment the connection count
            pos++; // increment position in SDA connection vector
        }
    }

    this->connections = connections;// set the connection defined by the vector as the connections for the topology
    
    if(verbose && analyzeData){
        LayerNodes();
        DistributeTraffic();
        printConnections();
        printTraffic();
    }
    else if(analyzeData){
        LayerNodes();
        DistributeTraffic();
    }else if(verbose){
        printConnections();
    }else return;
}