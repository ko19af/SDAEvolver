#include <iostream>
#include <fstream>
#include <cstdlib>
#include <bits/stdc++.h>
#include <iomanip> 
#include <limits>
#include <filesystem>
#include "SDA.h"
#include "Topology.h"
#include "AttackSim.h"
#include "Steady.h"
#include "vector"

using namespace std;

int main(int argc, char* argv[]) {
    srand(1);// seed the random number generator
    vector<int> hyperParameters;// initialize vector for holding hyper-parameters

    if (argc > 1){ // collect hyper-parameters for the run (if provided)
        hyperParameters.push_back(atoi(argv[1]));
        hyperParameters.push_back(atoi(argv[2]));
        hyperParameters.push_back(atoi(argv[3]));
        hyperParameters.push_back(atoi(argv[4]));
        hyperParameters.push_back(atoi(argv[5]));
        hyperParameters.push_back(atoi(argv[6]));
        hyperParameters.push_back(atoi(argv[7]));
        hyperParameters.push_back(atoi(argv[8]));
        hyperParameters.push_back(atoi(argv[9]));
        hyperParameters.push_back(atoi(argv[10]));
        hyperParameters.push_back(atoi(argv[11]));
        hyperParameters.push_back(atoi(argv[12]));
        hyperParameters.push_back(atoi(argv[13]));
    }else{// initialize the variables in case none are passed on the cmd line
            hyperParameters.push_back(6);      // SDA number of states
            hyperParameters.push_back(2);      // SDA number of characters
            hyperParameters.push_back(2);      // SDA max response length
            hyperParameters.push_back(50);     // Population size
            hyperParameters.push_back(3);      // Tournament Selector
            hyperParameters.push_back(1);      // Genetic Algorithm Operator
            hyperParameters.push_back(1000);   // Number of Generations
            hyperParameters.push_back(1);      // Cross-over Operator
            hyperParameters.push_back(10);     // Cross-over Rate
            hyperParameters.push_back(1);      // Mutation Operator
            hyperParameters.push_back(10);     // Mutation Rate
            hyperParameters.push_back(30);     // Number of Runs
            hyperParameters.push_back(2);      // Heurestic Function  
        }

    for (int t = 0; t < 5; t++){
        string path = "Topologies/Layout_"+to_string(t)+".txt";
        Topology T = Topology(path, false); // initialize the topology
        T.preMadePop = new SDA[(int)hyperParameters[11]];
        
        string fName = "Output/Experiment_" + to_string((int)hyperParameters[0]) + to_string((int)hyperParameters[1]) + 
        to_string((int)hyperParameters[2]) + to_string((int)hyperParameters[3]) + to_string((int)hyperParameters[4]) + 
        to_string((int)hyperParameters[5]) + to_string((int)hyperParameters[6]) + to_string((int)hyperParameters[7]) + 
        to_string((double)hyperParameters[8]/100) + to_string((int)hyperParameters[9]) + to_string((double)hyperParameters[10]/100) + 
        to_string((int)hyperParameters[11]) + to_string((int)hyperParameters[12]) + to_string(t) + ".txt";

        ofstream MyFile(fName);
        
        stringstream ss;
        ss << "# States: " << to_string((int)hyperParameters[0]) << " # Chars: " << to_string((int)hyperParameters[1]) << " SDAResponseLength: " << to_string((int)hyperParameters[2])
        << " popSize: " << to_string((int)hyperParameters[3]) << " tournSelector: " << to_string((int)hyperParameters[4]) << " gaOperator: " 
        << to_string((int)hyperParameters[5]) << " numGen: " << to_string((int)hyperParameters[6]) << " crossOp: " << to_string((int)hyperParameters[7]) 
        << " crossRate(%): " << setprecision(2) << ((double)hyperParameters[8])/100 << " mutationOperator: " << to_string((int)hyperParameters[9]) << " mutationRate(%): "
        << setprecision(2) << ((double)hyperParameters[10])/100 << " Heurestic: " << to_string((int)hyperParameters[12]) << " Topology: " << to_string(t) << " runs: " 
        << to_string((int)hyperParameters[11]);
        string params = ss.str();
        MyFile << params << endl;
        
        for (int x = 0; x < hyperParameters[11]; x++){// perform the runs
            MyFile << "Run: " << x + 1 << endl;// report the run number
            Steady(T, MyFile, hyperParameters, false);
        }
        
        MyFile.close();
        
        if(atoi(argv[17])){
            string eFName = fName;
            eFName.insert(7, "Extended_");
            eFName.insert(6, "_3");
            ofstream ExtendedFile(eFName);
            ExtendedFile << params << endl;
            ExtendedFile << "Topology: " << t + 1 << endl;
            for (int x = 0; x < hyperParameters[11]; x++){// perform the runs
                ExtendedFile << "Run: " << x + 1 << endl;// report the run number
                Steady(T, ExtendedFile, hyperParameters, true);
            }
            ExtendedFile.close();
        }

        if(atoi(argv[14])){// if performing attack simulations
            AttackSim(T, hyperParameters, fName, params, atoi(argv[15]), atof(argv[16]) / 100);
        }

    delete[] T.preMadePop;
    }
    return 0;
}