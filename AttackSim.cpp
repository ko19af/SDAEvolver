#include "AttackSim.h"
#include "Topology.h"

AttackSim::AttackSim(string& filename, bool verbose){
    readEData(filename);

    performTowerAttack(connections.size());// perform the DOS/DDOS attack that disables towers

    Topology(connections);// perform analysis on the topologies
}

/**
 * This method read a provided network layout and reads it into the program
 * 
 * @param fileName is the name of the file being read into the system
 */

void AttackSim::readLayout(string& fileName){
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

    this->numENodes = 0;
    this->numCNodes = 0;
    this->numNodes = 0;
    this->tNumNodes = 0; // calculate total number of nodes in Topology

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

    this->network = network;
}

/**
 * This method reads a provided connection layout and reads it into the program
 * 
 * @param fileName2 is the name of the file being read into the system
 */

void AttackSim::readEData(string& fileName){
    string text;// holds text from file
    string tFile;// holds the topology file name
    ifstream ReadFile(fileName);// input file stream reading in the desired file
    for (int x = 0; x < 2; x++) getline(ReadFile, text); // get the second line from the file
    split(text, ':', tFile);// split the text from the data wanted

    tFile = "Topologies/Layout_" + to_string(tFile[0] - '0' - 1) + ".txt";// get the topology file used for the experiment

    readLayout(tFile);// read the topology information

    this->connections = vector<vector<int>>(tNumNodes, vector<int>(tNumNodes));// initialize connections matrix

    while(getline (ReadFile, text)){// read  rest of the lines from the file
        string c = "";// string holds splited part from the files text
        if(split(text, ':', c)){
            for (int y = 0; y < tNumNodes; y++){// load connections vector into the connections matrix
                for (int x = 0; x < y; x++){
                    if(y >= (numNodes+numENodes) && x < numENodes){// set connections between edge & cloud nodes to zero
                        connections[y][x] = 0;
                        connections[x][y] = 0;
                    }else{// set connection based on vector given from SDA
                        connections[y][x] = c[0] - '0';
                        connections[x][y] = c[0] - '0';
                        if(c.size() > 2) c.erase(c.begin(), c.begin() + 2);// remove the inserted element and following space from the vector
                    }
                }
            }
            break;// if found the line and extracted the vector break out of loop
        } 
    }
}

/**
 * This method splits a string in the experiment file apart based on the ':' character to reterive the 
 * Best Layout result
 * 
 * @param input is the line from the file being split
 * @param del is the character being deleted
 * @param c is the string containing the connections
 */
bool AttackSim::split(string input, char del, string& c){
    for (int i = 0; i < (int)input.size(); i++){// go through the array of character constituting the string
        if(input[i] != (del)) c += input[i];// if the character does not match the one being delimiting the text add it to the string
        else{// if we hit the character delimiting the text
            if(c == "Best Layout" || c == "Topology"){// check if the characters read in match the string preceding the connections vector
                c = "";// reset the string to hold only the connections vector
                for (int x = i + 2; x < (int)input.size(); x++) c += input[x];//starting from the charcter after the delimiter add the characters to the string
                return true;// return true that we have found connections
            }
            else return false;// if it does not match preceeding string, return and try next line
        }
    }
    return false;// return false if read entire line
}

/**
 * This method performs the DOS/DDOS attack variation that disables the towers in the network by removing the 
 * rows/coloumns from the connections vector representing these towers
 * 
 * @param actTowers is the number of active towes in the network before the attack simulation
 * @param remTowers is the number of towers being disabled in the network for the simulation
 */

void AttackSim::performTowerAttack(int actTowers, int remTowers){
    vector attackedNet = vector<vector<int>>(actTowers - remTowers, vector<int>(actTowers - remTowers));// create reduced connections matrix
    vector attTowers = vector<int>(actTowers, 0);// vector recording the towers being attacked
    int tow;// randomly select a tower (may be changed later to SDA)

    for (int x = 0; x < remTowers; x++){// for the number of towers being attacked
        do{
            tow = ((double)rand() / RAND_MAX) * (actTowers - (0)) + (0);
        }while (attTowers[tow] != 0);// if that tower was already selected, select a tower until it is one that has not been chosen
        attTowers[tow] = 1;// set tower as being attacked
    }

    int cr = 0;// initial pointers telling which row and coloumn are being added to the new connection matrix
    int cc = 0;

    for (int y = 0; y < attackedNet.size(); y++){// go through rows of attacked connections
        while (attTowers[cr] == 1) cr++;// if row is being removed move to next row
        for (int x = 0; x < attackedNet.size(); x++){// go through coloumns of attacked connections
            while (attTowers[cc] == 1) cc++;// if coloumn is being attacked move to next coloumn
            attackedNet[y][x] = connections[cr][cc];// set connections in attacked matrix
            attackedNet[x][y] = connections[cc][cr];
            cc++;// move to the next colomn in original connections matrix
        }
        cr++;// move to next row in original connections matrix
    }
    this->connections = attackedNet;
}