#include "AttackSim.h"

AttackSim::AttackSim(){}

/**
 * Performs a cyberattack simulation on a provided network to determine its reslience to the attack
 * 
 * @param heurFunction decides which heurestic function should be used for the analysis
 * @param attFunction decides which attack is being performed on the network
 * @param attTowers is the percentage of towers being attacked in the network
 * @param path is the directory holding the files with the completed runs being examined
 * @param verbose is a boolean determining if the info should be printed
 */
AttackSim::AttackSim(int attFunction, double percentAtt, bool verbose){
    fs::directory_iterator Start{R"(Output)"};  
    fs::directory_iterator End{};

    readTopologies(); // read ina&ll the topologies  

    for (auto Iter{Start}; Iter != End; ++Iter) { // iterates over the files in the directory provided without modifying them
        vector<string> hyperParameters;

        string file = Iter->path().string().erase(0, 7); // get the name of the file being attacked
        string attParam = string(to_string(attFunction) + to_string(percentAtt));
        ofstream outputFile("Output_2/Attacked_" + attParam + file); // create the file recording the results

        Topology T = readEData(Iter->path().string(), hyperParameters, outputFile, attFunction); // read the information in from the file and set the topology
        if(first){ // select the towers being attacked in the simulation
            selectAttackedTowers(T.numNodes * percentAtt, T);
            first = false;
        }                        
        T.attTowers = attTowers;
        T.attackData = vector<vector<double>>(T.tNumNodes, vector<double>(0, 0));

        if (attFunction == 2) performDataAttack(T.attackData, attTowers, 100);
        

        outputFile << "AttackFunction: " << to_string(attFunction) << "\t AttackedTowers(%): "
                   << setprecision(15) << percentAtt << "\n"
                   << "AttackedTowers: ";
        for (int t = 0; t < attTowers.size(); t++)
            if (attTowers[t])
                outputFile << t + 1 << "\t";
        outputFile << endl;

        for (int x = 0; x < stoi(hyperParameters[12]); x++){ // perform the runs
            outputFile << "Run: " << x + 1 << endl;
            Steady(T, population, attFunction, hyperParameters, outputFile);
        }
        outputFile.close();
    }
}

AttackSim::AttackSim(Topology& T, vector<int>& hyperParameters, string fName, string params, int attFunction, double percentAtt, bool verbose){

    string file = fName.erase(0, 7); // get the name of the file being attacked
    string attParam = string(to_string(attFunction) + to_string(percentAtt));
    ofstream outputFile("Output_2/Attacked_" + attParam + file); // create the file recording the results

    selectAttackedTowers(T.numNodes * percentAtt, T);// select the towers to attack
    T.attTowers = attTowers;
    T.attackData = vector<vector<double>>(T.tNumNodes, vector<double>(0, 0));

    if (attFunction == 2) performDataAttack(T.attackData, attTowers, 100);
    
    outputFile << params << endl;

    outputFile << "AttackFunction: " << to_string(attFunction) << "\t AttackedTowers(%): "
                << setprecision(15) << percentAtt << "\n"
                << "AttackedTowers: ";

    for (int t = 0; t < attTowers.size(); t++)
        if (attTowers[t])
            outputFile << t + 1 << "\t";
    outputFile << endl;

    for (int x = 0; x < hyperParameters[10]; x++){ // perform the runs
        outputFile << "Run: " << x + 1 << endl;
        Steady(T, attFunction, hyperParameters, outputFile);
    }
    outputFile.close();
}

/**
 * This method loads the topology information into the Topology class and reads in the
 * SDA data from the previous netwrok construction provided from the file
 * 
 * @param filePath is the path of the file being read into the system
 * @param hyperParameters stores the hyperParameters used ffrom the initial newtwork construction
 * @param outputFile is where the results will be printed to, it is provided here so the hyperparameters from the previous file are written in
 */

Topology AttackSim::readEData(const std::filesystem::__cxx11::path& filePath, vector<string> &hyperParameters, ofstream& outputFile, int attFunction){
    string text;// holds text from file
    string tFile;// holds the topology file name
    ifstream ReadFile(filePath);// input file stream reading in the desired file

    getline(ReadFile, text);// read first line from file
    outputFile << text << endl;// write hyperparameters line from file
    readHyperParameters(text, hyperParameters);// load hyperparameters into the vector
    
    getline(ReadFile, text); // get the second line from the file
    outputFile << text << endl;// write the Topology used for the experiment
    split(text, ':', tFile); // split the text from the data wanted for getting the topology file name
    Topology T = Topology(topologies[tFile[0] - '0' - 1]);// initialize the topology with the correct layout for the file

    this->population = new SDA[stoi(hyperParameters[12])];
    SDAResponseLength = (T.tNumNodes*(T.tNumNodes-1))/2;;// assign that value to the SDA response length global variable
    int numSDAs = 0;

    vector<vector<int>> connections = vector<vector<int>>(T.tNumNodes, vector<int>(T.tNumNodes));// initialize connections matrix

    while(getline(ReadFile, text)){// read rest of the lines from the file
        string c = "";// string holds splited part from the files text
        if(split(text, ':', c)){// if reading "Best SDA"
            vector<string> theSDA;// create vector to hold string form of vector
            do{
                theSDA.push_back(c);// push the initial character and state into the SDA
                c = "";
                getline(ReadFile, text);// get next line from file
            } while (!split(text, ':', c));// while not reading in a "Best Layout"
            population[numSDAs] = SDA(stoi(hyperParameters[0]), stoi(hyperParameters[1]), SDAResponseLength, SDAResponseLength, theSDA);
            numSDAs++;// increment the number of pre-designed SDAs inserted into the population
        }
    }
    ReadFile.close(); // close the file being read
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
 * @param T is the Topology being attacked
 */

void AttackSim::selectAttackedTowers(int numTowers, Topology& T){
    this->attTowers = vector<bool>(T.tNumNodes, 0);// vector determing which towers are attacked
    for (int x = 0; x < numTowers; x++){// for the number of towers being attacked
        int aTow = 0;
        do{
            aTow = (int) lrand48() % attTowers.size();// randomly select a tower to attack
        } while (attTowers[aTow] || aTow < T.numENodes || aTow  >= T.tNumNodes-T.numCNodes);// if already selected or is edge/cloud node
        attTowers[aTow] = true; // if tower is not already selected add to vector
    }
}

/**
 * This method performs the DOS/DDOS attack variation that disables the towers in the network by removing the 
 * rows/coloumns from the connections vector representing these towers
 * 
 * @param connections are the connections in the network being attacked
 * @param T is the topology being attacked
 */

void AttackSim::performTowerAttack(vector<vector<int>>& connections, vector<bool>& attackedTowers){
    for (int tow = 0; tow < attackedTowers.size(); tow++){// go through the vector containing the attacked towers
        if(attackedTowers[tow]){// if the tower is being attacked
            fill(connections[tow].begin(), connections[tow].end(), 0);// set the connectctions in the attacked tower as zero
            for(vector<int>& row : connections) row[tow] = 0;// remove the connections in the attacked towers coloumn
            
        }
    }
}

/**
 * This method performs the DoS/DDoS attack the injects additional data into the network without
 * disablling the tower being attacked
 * 
 * @param data is the matrix recording which nodesare sending what data
 * @param maxOut is the maximum amount of data a node is permitted to transmit (units in mbps)
 * @param upper is the upperlimit of a data stream emitting from a node (units in mbps)
 * @param lower is the lowerlimit of a data stream emitting from a node (units in mbps)
 */

void AttackSim::performDataAttack(vector<vector<double>>& attackData, vector<bool>& attackedTowers, int maxOut, int upper, int lower ){
    for (int tow = 0; tow < attackedTowers.size(); tow++){// go through the vector containing the attacked towers
        if(attackedTowers[tow]){// if tower is being attacked
            double output = 0; // record how much data is being ouputed at the node
        do{// while the ouput from the node is less than the max ouput
            double d = round((rand()/(double)RAND_MAX)*(upper-lower)+lower);// randomly generate a value in the packet size range
            if(output + d > maxOut) break;// if ouput plus that value is greater than max output break out of loop
            attackData[tow].push_back(d);// if max is not violated add that data to the list of streams eminating from the node
            output += d;// apped the data to the ouput value
        } while (output < maxOut);
        }
    }
}

double AttackSim::round(float var){
    float value = (int)(var * 100 + .5);
    return (double)value / 100;
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

/**
 * This method reads in the hyperparameters from the file being read
 * 
 * @param input is the string containing the hyperparameters
 * @param hyperParameters is the vector that will store the hyperparameters for later use
 */

void AttackSim::readHyperParameters(string input, vector<string> &hyperParameters){
    for (int x = 0; x < input.size(); x++){// for the entire string
        if (input[x] == ':'){// if we hit a delimiter
            x+=2;// move to the information in the string
            if (input[x + 1] != ' '){// if it is not a single digit
                string p;
                while( x < input.size() && input[x] != ' '){// read the data pieces into the string
                    p += input[x];
                    x++;
                }
                hyperParameters.push_back(p);// add the hyperparameter to the vector
            }
            else{// if it is a single digit to be read in
                hyperParameters.push_back(string(1,input[x]));// push data into the vector
                x+=2;// move to the next character in the string
            }
        }
    }
}