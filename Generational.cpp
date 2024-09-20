#include "Generational.h"

/**
 * Generates the next generation's population by performing tournament selection on currentPop,
 * crossover on copies of members of random members of the tournament, and potentially mutation of these
 * copies.  The new population is stored in newPop.
 *
 * @param currentPop the current generation's population
 * @param newPop the next generation's population
 * @return
 */
int Generational::genMatingEvent(SDA *currentPop, SDA *newPop, Topology T) {
    vector<int> tournIdxs = genTournSelect(genTournSize, genLowerBetter);// perform Tournament Selection
    SDA parent1, parent2, child1, child2, bc1, bc2;
    int idx1, idx2;// variables holding the values of the tournament indexes

    for (int event = elitism; event < genPopSize; event += 2) {// fill population starting after elite members
        do {
            idx1 = tournIdxs[(int) lrand48() % genTournSize];
            idx2 = tournIdxs[(int)lrand48() % genTournSize];
        } while (idx1 == idx2); // choose two different tournament members

        parent1 = currentPop[idx1];// set parents to the tournament members
        parent2 = currentPop[idx2];

        child1.copy(parent1);// copy the parents to the children
        child2.copy(parent2);

        // perform initial Crossover
        if(genCrossOp == 1 && drand48() < genCrossRate) child1.crossover(child2);

        // perform initial Mutation
        if (drand48() < genMutationRate && genMutOperator == 1) {
                child1.mutate(genNumMutations);
                child2.mutate(genNumMutations);
        }

        // Add the produced children to the new population
        newPop[event] = child1;
        newPop[event + 1] = child2;

        // Calculate the fitness of the resulting children
        genNewPopFits.push_back(genCalcFitness(child1, T));
        genNewPopFits.push_back(genCalcFitness(child2, T));
    }
    return 0;
}

/**
 * Performs a tournament selection deciding which members from the population, of the
 * current generation, are candidates for creating the next generation's population.
 * The indices are sorted by their fitness, depending on the value of decreasing.
 * Can be used to sort the entire population if size == popSize.
 *
 * @param size of the returned vector of indices from the tournament selection
 * @param decreasing is a boolean determining the ordering of the indices in the returned vector based on their fitness
 * @return a vector of indices for members of the population sorted based on fitness
 */
vector<int> Generational::genTournSelect(int size, bool decreasing) {
    vector<int> tournIdxs;
    int idxToAdd, idxToCheck;

    tournIdxs.reserve(size);
    if (size == genPopSize) {// If we want to sort the population, we call tournSelect with size == popsize
        for (int idx = 0; idx < size; idx++) {
            tournIdxs.push_back(idx);
        }
    } else {
        for (int mem = 0; mem < size; mem++) {// For each member we want to add to the tournament
            do {// Find an index that's not in the tournament tournMaxRepeats times
                idxToAdd = (int) lrand48() % genPopSize;
            } while (count(tournIdxs.begin(), tournIdxs.end(), idxToAdd) >= tournMaxRepeats);

            // Compare this index to tournCandidates - 1 other indexes
            for (int check = 1; check < tournCandidates; check++) {
                // Find an index that's not in the tournament tournMaxRepeats times
                do {
                    idxToCheck = (int) lrand48() % genPopSize;
                } while (count(tournIdxs.begin(), tournIdxs.end(), idxToCheck) >= tournMaxRepeats);

                // If this test index has better fitness, replace the index to add with it
                if ((genPopFits[idxToCheck] > genPopFits[idxToAdd] && !genLowerBetter) ||
                    (genPopFits[idxToCheck] < genPopFits[idxToAdd] && genLowerBetter)) {
                    idxToAdd = idxToCheck;
                }
            }
            // Add the index to the tournament
            tournIdxs.push_back(idxToAdd);
        }
    }
    return tournIdxs;
}

/**
 * This method compares the fitness of two members of the population and returns a boolean determining
 * which member had the greatest fitness
 *
 * @param popIdx1 an index representing a member of the population
 * @param popIdx2 a second index representing a member of the population whos fitness is being compared to the previous member
 * @return a boolean determing which member had the greatest fitness
 */
bool Generational::genCompareFitness(int popIdx1, int popIdx2) {
    if (genPopFits[popIdx1] <= genPopFits[popIdx2]) {
        return true;
    }
    if (genPopFits[popIdx1] > genPopFits[popIdx2]) {
        return false;
    }
    cout << "ERROR: compare fitness not working as intended!" << endl;
    return false;
}

/**
 * This method calculates the fitness of an SDA when its output is applied to the topology being examined
 * 
 * @param member is the SDA being evaluated
 * @param Topology is the topology that is being used for the evaluation
 * @return is the fitness values of the SDA when applied to the topology
 */

double Generational::genCalcFitness(SDA &member, Topology& T){
    bool verbose = false;
    vector<int> c(genSDAResponseLength); // vector for holding response from SDA
    member.fillOutput(c, false, cout);// fill vector using SDA
    T.setConnections(c, verbose, heurFunction);//set the connections in the topology
    switch(heurFunction){
        case 0:
        return distanceFitness(T);
            break;
        case 1:
            return  dataFitness(T); 
            break;
        case 2:
            return energyFitness(T);
            break;
        case 3:
            return distanceFitness(T) + dataFitness(T) + energyFitness(T);
            break;
        case 4:
            return dataFitness(T) + energyFitness(T);
            break;
        case 5:
            return distanceFitness(T) + dataFitness(T);
            break;
        case 6:
            return distanceFitness(T) + energyFitness(T);
        }
        return 0;
}

/**
 * This method calculates the average amount of data flowing through each node in the network
 * 
 * @param T is the topology being used in the evaluation of the fitness
 * @return is the average amount of data being passed through the nodes in the network
 */

double Generational::dataFitness(Topology& T){
    double val = 0.0;// is the fitness of the data being passed through the nodes in the topology

    for (int y = T.numENodes; y < T.tNumNodes; y++){// for each node that is not an edge node
        double d = 0.0;// varialbe to record the amount of data a node is receiving
        for (int x = 0; x < T.tNumNodes; x++) d += T.trafficMatrix[y][x];// add up all the data the node is receving
        if(d != 0) val += d / T.data[y].size();// average out the amount of data based on the amount of packet streams it is receving
    }
    return val / (T.tNumNodes-T.numENodes);// return the averaged value for all the nodes in the network
}

/**
 * This method calculates the fitness of the connection distance in the topology
 * 
 * @param T is the topology being evaluates
 * @return is the average distance all edge nodes are from a cloud node
 */

double Generational::distanceFitness(Topology& T){
    // Fitness function sums all distances an edge node uses to reach a cloud node through topology
    double val = 0;
    for (int x = 0; x < T.numENodes; x++){// for each edge node
        vector<double> sPath(T.tNumNodes, DBL_MAX);// create vector to record distance from edge node to all other nodes
        sPath[x] = 0;// set distance to starting edge node to zero
        T.ShortestPath(x, sPath);// calculate shortest path to all nodes in topology from selected edge node

        int count = 0;// number of cloud nodes edge node connects to
        double dist = 0;// total distance from edge node to cloud node
        for (int i = 0; i < T.numCNodes; i++){
            if(sPath[T.tNumNodes - 1 - i] < DBL_MAX){// if there exists a path from the edge node to cloud node
                dist += sPath[T.tNumNodes - 1 - i];// add distance
                count++;// increment connection count
            }
          }
        if(count != 0) val += dist / count;// add average connection distance to total distance value
    }
    return val/T.numENodes;// return the average distance from all edge nodes to a cloud node
}

/**
 * This method calculates the fitness of the energy consumption in the topology
 * 
 * @param T is the topology being examined
 * @return is the average energy consumption per node in the network
 */

double Generational::energyFitness(Topology& T){
    double val = 0.0;// is the fitness of the data being passed through the nodes in the topology
    for (double e : T.energyConsumption)val += e;// add up energy consumption in the network
    return val / (T.tNumNodes-T.numENodes);// return the averaged value for all the nodes in the network
}

int Generational::genPrintPopFits(ostream &outStrm, vector<double> &popFits) {
    outStrm << "Fitness Values: ";
    int count = 0;
    bool first = true;
    for (double fit: popFits)
            {
                // This ensures commas after each fitness value other than the last
                if (!first)
                {
                    outStrm << ", ";
                }
                outStrm << fit;
                if (fit > 150)
                {
                    count++;
                }
                first = false;
            }
    outStrm << "\n";
    //outStrm << "Above 0.5: " << count << "\n";
    return 0;
}

int Generational::genEvolver(int SDANumStates, int SDAOutputLen, int numGenerations, Topology T, ostream& MyFile) {
    SDA *currentPop, *newPop, cp;
    currentPop = new SDA[genPopSize];
    newPop = new SDA[genPopSize];
    genPopFits.reserve(genPopSize);
    int modVal = numGenerations / 10;

    // Step 1: initialize the population
    for (int i = 0; i < genPopSize; ++i) {
        currentPop[i] = SDA(SDANumStates, genSDANumChars, genSDAResponseLength, SDAOutputLen);;// place member into population
        genPopFits.push_back(genCalcFitness(currentPop[i], T));// calculate new members fitness
    }

    MyFile << "Initial Pop Fitness Value: " << endl;
    genPrintPopFits(MyFile, genPopFits); // print population fitness

    // Step 2: Evolution
    for (int gen = 0; gen < numGenerations; ++gen) {
        genNewPopFits.clear();
        genNewPopFits.reserve(genPopSize);

        // Perform elitisim
        vector<double> elite(elitism, DBL_MAX);// vector holding two most elite fitnesses 
        int idx1, idx2;// index locations of the two most elite SDA
        for (int e = 0; e < genPopFits.size(); e++){// go through the genertions pop fitness
            if(elite[0] > genPopFits[e]){
                elite[0] = genPopFits[e];// update elite 1 fitness value
                idx1 = e; // memner fitness better than elite 1
            } 
            else if(elite[1] > genPopFits[e]){
                elite[1] = genPopFits[e];// update elite 2 fintess value
                idx2 = e;// member fitness better than elite 2 and not used by elite 1
            }
        }
        // copy the elite SDA to the new population
        newPop[0].copy(currentPop[idx1]);
        newPop[1].copy(currentPop[idx2]);

        // Store the fitness of the most elite members in the first two positions
        genNewPopFits.push_back(genPopFits[idx1]);
        genNewPopFits.push_back(genPopFits[idx2]);

        // Generate the new population
        genMatingEvent(currentPop, newPop, T);

        // Replace current population with new population
        for (int mem = 0; mem < genPopSize; mem++){
            currentPop[mem] = newPop[mem];
        }
        if(gen % modVal == 0) genPrintPopFits(MyFile, genNewPopFits);// print every 10th generation
    }
    MyFile << "Final Fitness of Run: ";
    genPrintPopFits(MyFile, genNewPopFits);
    return 0;
}

Generational::Generational(Topology& T, ostream& MyFile, int numStates, int numChars, int popSize, int tournSize, int numGen, int crossOp, double crossRate, int mutOperator, double mutRate, int heurFunction) {
    
    this->genSDANumChars = numChars;
    this->genPopSize = popSize;
    this->genTournSize = tournSize;
    this->genCrossOp = crossOp;
    this->genCrossRate = crossRate;
    this->genMutOperator = mutOperator;
    this->genMutationRate = mutRate;
    this->heurFunction = heurFunction;

    int outputLen = (T.tNumNodes*(T.tNumNodes-1))/2;// calculate the required ouput from the SDA
    this->maxConnections = outputLen;
    genSDAResponseLength = outputLen;// assign that value to the SDA response length global variable

    genEvolver(numStates, outputLen, numGen, T, MyFile);// call the genetic algorithm
}