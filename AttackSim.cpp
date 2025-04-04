#include "AttackSim.h"

/**
 * Performs a cyberattack simulation on a provided network to determine its reslience to the attack
 * 
 * @param heurFunction decides which heurestic function should be used for the analysis
 * @param attTowers is the percentage of towers being attacked in the network
 * @param path is the directory holding the files with the completed runs being examined
 * @param verbose is a boolean determining if the info should be printed
 */
AttackSim::AttackSim(int heurFunction, double attTowers, bool verbose, string path){

    readTopologies();// read in all the topologies

    for (const auto &entry : fs::directory_iterator(path)){// iterates over the files in the directory provided without modifying them
        vector<string> hyperParameters;
        Topology T = readEData(entry.path(), hyperParameters);// read the information in from the file and set the topology

        string file = string(entry.path()).erase(0, 7);// get the name of the file being attacked
        ofstream outputFile("Output_2/Attacked_" + file);// create the file recording the results

        outputFile << "Heurestic Function: " << to_string(heurFunction) << "\t Attacked Towers (%): " << setprecision(15) << attTowers << endl;
        selectAttackedTowers(T.numNodes * attTowers, T);// select the towers being attacked in the simulation

        performTowerAttack();// perform the DOS/DDOS attack that disables towers

        while(networkCon.size() > 0){// while there is a network to examine
            outputFile << "Run: " << hyperParameters[13] << endl;
            T.connections = networkCon[0];         // set the connections in the topology
            Steady(T, heurFunction, outputFile); // call steady to make use of heurestic methods in the class
            networkCon.erase(networkCon.begin());
        }
        outputFile.close();
    }
}

/**
 * This method reads a provided connection layout and reads it into the program
 * 
 * @param filePath is the path of the file being read into the system
 */

Topology AttackSim::readEData(const std::filesystem::__cxx11::path& filePath, vector<string> &hyperParameters){
    string text;// holds text from file
    string tFile;// holds the topology file name
    
    ifstream ReadFile(filePath);// input file stream reading in the desired file
    getline(ReadFile, text);
    readHyperParameters(text, hyperParameters);
    getline(ReadFile, text); // get the second line from the file
    split(text, ':', tFile);// split the text from the data wanted for getting the topology file name

    Topology T = Topology(topologies[tFile[0] - '0' - 1]);// initialize the topology with the correct layout for the file
    this->population = new SDA[stoi(hyperParameters[2])];
    SDAResponseLength = (T.tNumNodes*(T.tNumNodes-1))/2;;// assign that value to the SDA response length global variable
    int numSDAs = 0;

    vector<vector<int>> connections = vector<vector<int>>(T.tNumNodes, vector<int>(T.tNumNodes));// initialize connections matrix

    while(getline(ReadFile, text)){// read  rest of the lines from the file
        string c = "";// string holds splited part from the files text

        if(split(text, ':', c)){// if reading "Best SDA"
            vector<string> theSDA;// create vector to hold string form of vector
            do{
                theSDA.push_back(c);// push the initial character and state into the SDA
                c = "";
                getline(ReadFile, text);// get next line from file
            } while (!split(text, ':', c));// while not reading in a "Best Layout"

            population[numSDAs] = SDA(stoi(hyperParameters[0]), stoi(hyperParameters[1]), SDAResponseLength, SDAResponseLength, theSDA);
        
            for (int y = 0; y < T.tNumNodes; y++){// load connections vector into the connections matrix
                for (int x = 0; x < y; x++){
                    if(y >= (T.numNodes+T.numENodes) && x < T.numENodes){// set connections between edge & cloud nodes to zero
                        connections[y][x] = 0;
                        connections[x][y] = 0;
                    }else{// set connection based on vector given from SDA
                        connections[y][x] = c[0] - '0';
                        connections[x][y] = c[0] - '0';
                        if(c.size() > 2) c.erase(c.begin(), c.begin() + 2);// remove the inserted element and following space from the vector
                    }
                }
            }
            networkCon.push_back(connections);// push network connections into vector
        }
    }
    ReadFile.close();// close the file being read
    return T;// return the newly created Topology
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
        else{// if we hit delimiting character
            if(c == "Best SDA" || c == "Best Layout" || c == "Topology"){// check if the characters read in match the string preceding the connections vector
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
 * This method randomly selects the towers that will be attacked in the simulation
 * 
 * @param attTowers is the number of towers being attacked in the simulation
 */

void AttackSim::selectAttackedTowers(int numTowers, Topology& T){
    vector<bool> attackedTowers = vector<bool>(T.tNumNodes, 0);// vector determing which towers are attacked

    T.tNumNodes -= numTowers;// remove the number of deactivated towers from the total count and non cloud/edge node towers
    T.numNodes -= numTowers;

    random_device rd;// initialize random number generator
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(T.numENodes, (T.tNumNodes-T.numENodes-1));// define range to exclude the cloud and edge nodes

    for (int x = 0; x < numTowers; x++){// for the number of towers being attacked
        int aTow = 0;
        do{
            aTow = distrib(gen);// randomly select a tower to attack
        } while (attackedTowers[aTow]);// choose a different tower if it was alread selected
        attackedTowers[aTow] = true; // if tower is not already selected add to vector
    }
}

/**
 * This method performs the DOS/DDOS attack variation that disables the towers in the network by removing the 
 * rows/coloumns from the connections vector representing these towers
 * 
 * @param actTowers is the number of active towes in the network before the attack simulation
 * @param remTowers is the number of towers being disabled in the network for the simulation
 */

void AttackSim::performTowerAttack(){
    for(vector<vector<int>> connections : networkCon){// for each connections matrix
        for (int tow = 0; tow < attTowers.size(); tow++){// go through the vector containing the attacked towers
            if(attTowers[tow]){// if the tower is being attacked
                fill(connections[tow].begin(), connections[tow].end(), 0);// set the connectctions in the attacked tower as zero
                for(vector<int> row : connections) row[tow] = 0;// remove the connections in the attacked towers coloumn
            }
        }
    }
}

void AttackSim::performDataAttack(Topology &T, int maxOut, int upper, int lower ){
    for (int tow = 0; tow < attTowers.size(); tow++){// go through the vector containing the attacked towers
        if(attTowers[tow]){// if tower is being attacked
            double output = 0; // record how much data is being ouputed at the node
        do{// while the ouput from the node is less than the max ouput
            double d = round((rand()/(double)RAND_MAX)*(upper-lower)+lower);// randomly generate a value in the packet size range
            if(output + d > maxOut) break;// if ouput plus that value is greater than max output break out of loop
            T.data[tow].push_back(d);// if max is not violated add that data to the list of streams eminating from the node
            output += d;// apped the data to the ouput value
        } while (output < maxOut);
        }
    }
}

/**
 * This method reads all the topologies into the program for easy access
 */

void AttackSim::readTopologies(){
    for (const auto &entry : fs::directory_iterator("Topologies")){// for each topology file
        vector<vector<int>> network;// vector recording the networks
        vector<int> row;
        string text;
        ifstream ReadFile(entry.path());// load in one of the topology files
        while (getline (ReadFile, text, '\n')){
            stringstream ss(text);// create string stream with text delimited by newline character
            while (getline(ss, text, '\t')) row.push_back(stoi(text));
            network.push_back(row);
            row.clear();
        }
        topologies.push_back(network);
        ReadFile.close();
    }
}

void AttackSim::readHyperParameters(string input, vector<string> &hyperParameters){
    for (int x = 0; x < input.size(); x++){
        if (input[x] == ':'){
            x+=2;
            if (input[x + 1] != ' '){
                string p;
                while( x < input.size() && input[x] != ' '){
                    p += input[x];
                    x++;
                }
                hyperParameters.push_back(p);
            }
            else{
                hyperParameters.push_back(string(1,input[x]));
                x+=2;
            }
        }
    }
}