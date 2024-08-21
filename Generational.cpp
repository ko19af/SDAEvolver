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
    // Tournament Selection
    vector<int> tournIdxs = genTournSelect(genTournSize, genLowerBetter);
    SDA parent1, parent2, child1, child2, bc1, bc2;

    for (int event = elitism; event < genPopSize; event += 2) {
        do {
            parent1 = currentPop[tournIdxs[(int) lrand48() % genTournSize]];
            parent2 = currentPop[tournIdxs[(int) lrand48() % genTournSize]];
        } while (parent1 == parent2); // TODO: Ensure this check works

        child1.copy(parent1);
        child2.copy(parent2);

        // perform initial Crossover
        if(genCrossOp == 1 && drand48() < genCrossRate) child1.crossover(child2);

        // perform initial Mutation
        if (drand48() < genMutationRate && genMutOperator == 1) {
                child1.mutate(genNumMutations);
                child2.mutate(genNumMutations);
        }

        vector<int> v(genSDAResponseLength);// vector for holding response from SDA
        child1.fillOutput(v, false, cout);// fill the vector with the ouput from the SDA
        T.setConnections(v, false, false);// set the connections in the Topology based on the SDA output
        bool retry = true;
        bool change = false;

        while(retry){
            retry = false;
            if(T.numConnections > maxConnections) retry = true;
            else{
                for (int y = 0; y < T.numENodes; y++){// go though all the edge nodes
                    for (int x = T.numENodes + T.numNodes; x < T.tNumNodes; x++){// go through all edge connections to cloud nodes
                        if(T.connections[y][x] == 1){// if there exists a connection between a edge and cloud node
                            bc1.copy(parent1);// make copies of the parents seperarte from the children
                            bc2.copy(parent2);
                            if(genCrossOp == 1 && drand48() < genCrossRate) bc1.crossover(bc2);// reperform crossover and mutation
                            if (drand48() < genMutationRate && genMutOperator == 1) bc1.mutate(genNumMutations);
                            bc1.fillOutput(v, false, cout);// fill the ouput with the new SDA
                            T.setConnections(v, false, false); // set the connections in the topology
                            retry = true;
                            change = true;
                            break;
                        }
                    }
                    if(retry) break;
                }
            }
        } if(change) child1.copy(bc1);//if og child was seen as invalid in loop set child1 SDA to the valid SDA

        child2.fillOutput(v, false, cout);// repeat above process for child2
        T.setConnections(v, false, false);// set the connections in the Topology based on the SDA output
        retry = true;
        change = false;

        while(retry){
            retry = false;
            if(T.numConnections > maxConnections) retry = true;
            else{
                for (int y = 0; y < T.numENodes; y++){
                    for (int x = T.numENodes + T.numNodes; x < T.tNumNodes; x++){
                        if(T.connections[y][x] == 1){
                            bc1.copy(parent1);
                            bc2.copy(parent2);
                            if(genCrossOp == 1 && drand48() < genCrossRate) bc2.crossover(bc1);
                            if (drand48() < genMutationRate && genMutOperator == 1) bc2.mutate(genNumMutations);
                            bc2.fillOutput(v, false, cout);
                            T.setConnections(v, false, false);
                            retry = true;
                            change = true;
                            break;
                        }
                    }
                    if(retry) break;
                }
            }
        } if(change) child2.copy(bc2);//if og child2 is seen as invalid set child2 SDA to the valid SDA

        // Add to new population
        newPop[event] = child1;
        newPop[event + 1] = child2;

        // Calculate fitness
        genPopFits[event] = genCalcFitness(child1, T);
        genPopFits[event + 1] = genCalcFitness(child2, T);
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
    // If we want to sort the population, we call tournSelect with size == popsize
    if (size == genPopSize) {
        for (int idx = 0; idx < size; idx++) {
            tournIdxs.push_back(idx);
        }
    } else {
        // For each member we want to add to the tournament
        for (int mem = 0; mem < size; mem++) {
            // Find an index that's not in the tournament tournMaxRepeats times
            do {
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

    // Sort the indexes, default is ascending
    //sort(tournIdxs.begin(), tournIdxs.end(), genCompareFitness);
    sort(tournIdxs.begin(), tournIdxs.end());
    // Flip the sort to descending
    if (decreasing){
        reverse(tournIdxs.begin(), tournIdxs.end());
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

double Generational::genCalcFitness(SDA &member, Topology T){

    vector<int> c(genSDAResponseLength);// vector for holding response from SDA
    member.fillOutput(c, false, cout);// fill vector using SDA
    T.setConnections(c, false, false);//set the connections in the topology

    // Fitness function sums all distances an edge node uses to reach a cloud node through topology
    int val = 0;
    for (int x = 0; x < T.numENodes; x++){// for each edge node
        vector<double> sPath(T.tNumNodes, DBL_MAX);// create vector to record distance from edge node to all other nodes
        sPath[x] = 0;// set distance to starting edge node to zero
        T.ShortestPath(x, sPath);// calculate shortest path to all nodes in topology from selected edge node

        int count = 0;// number of cloud nodes edge node connects to
        int dist = 0;// total distance from edge node to cloud node
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

int Generational::genPrintPopFits(ostream &outStrm, vector<double> &popFits) {
    outStrm << "Fitness Values: ";
    int count = 0;
    bool first = true;
    for (double fit: popFits) {
        // This ensures commas after each fitness value other than the last
        if (!first) {
            outStrm << ", ";
        }
        outStrm << fit;
        if (fit > 150){
            count++;
        }
        first = false;
    }
    outStrm << "\n";
    //outStrm << "Above 0.5: " << count << "\n";
    return 0;
}

int Generational::genEvolver(int SDANumStates, int SDAOutputLen, int numGenerations, Topology T) {
    SDA *currentPop, *newPop, np, cp;
    currentPop = new SDA[genPopSize];
    newPop = new SDA[genPopSize];
    genPopFits.reserve(genPopSize);

    // Step 1: initialize the population
    for (int i = 0; i < genPopSize; ++i) {

        vector<int> v(genSDAResponseLength);// vector for holding response from SDA
        bool retry = true;

        while(retry){
            cp = SDA(SDANumStates, genSDANumChars, genSDAResponseLength, SDAOutputLen);// create a new SDA
            cp.fillOutput(v, false, cout);// fill the vector with the new SDA's output
            T.setConnections(v, false, false);// set the connections in the topology
            retry = false;// set retry bool to false so loop is exited if the SDA is valid
            if(T.numConnections > maxConnections) retry = true;// if number of connections exceed cap
            else{// if number of connection does not exceed cap check connections
                for (int y = 0; y < T.numENodes; y++){// go though all the edge nodes
                    for (int x = T.numENodes + T.numNodes; x < T.tNumNodes; x++){// go through all edge connections to cloud nodes
                        if(T.connections[y][x] == 1){// if there exists a connection between a edge and cloud node
                            retry = true;// set retry boolean to true
                            break;// break out of second for-loop as SDA is invalid
                        }
                    }
                    if(retry) break;// break out of first for-lopp as current SDA is invalid
                }
            }
        }

        retry = true;

        while(retry){
            np = SDA(SDANumStates, genSDANumChars, genSDAResponseLength, SDAOutputLen);
            np.fillOutput(v, false, cout);
            T.setConnections(v, false, false);
            retry = false;
            if(T.numConnections > maxConnections) retry = true;
            else{
                for (int y = 0; y < T.numENodes; y++){
                    for (int x = T.numENodes + T.numNodes; x < T.tNumNodes; x++){
                        if(T.connections[y][x] == 1){
                            retry = true;
                            break;
                        }
                    }
                    if(retry) break;
                }
            }
        }

        currentPop[i] = cp;
        newPop[i] = np;
        genPopFits.push_back(genCalcFitness(currentPop[i], T));
    }

    genPrintPopFits(cout, genPopFits);

    // Step 2: Evolution
    for (int gen = 0; gen < numGenerations; ++gen) {
        // Keep the most elite members
        vector<int> sortedIdxs = genTournSelect(genPopSize, genLowerBetter);
        vector<double> oldFits;
        for (int elite = 0; elite < elitism; ++elite) {
            newPop[elite].copy(currentPop[sortedIdxs[elite]]);
            oldFits.push_back(genPopFits[sortedIdxs[elite]]);
        }
        // Store the fitness of the most elite members in popFits
        for (int mem = 0; mem < elitism; mem++){
            genPopFits[mem] = oldFits[mem];
        }

        // Generate the new population
        genMatingEvent(currentPop, newPop, T);

        // Replace current population with new population
        for (int mem = 0; mem < genPopSize; mem++){
            currentPop[mem] = newPop[mem];
        }
        genPrintPopFits(cout, genPopFits);
    }
    genPrintPopFits(cout, genPopFits);
    return 0;
}

Generational::Generational(int numStates, int numChars, int popSize, int tournSize, int numGen, int crossOp, double crossRate, int mutOperator, double mutRate) {
    
    this->genSDANumChars = numChars;
    this->genPopSize = popSize;
    this->genTournSize = tournSize;
    this->genCrossOp = crossOp;
    this->genCrossRate = crossRate;
    this->genMutOperator = mutOperator;
    this->genMutationRate = mutRate;

    // define parameters for Topology
    this->numColoumns = 10;
    this->numRows = 10;
    this->numStarts = 1;
    this->numEnds = 1;
    this->numNodes = 15;// 30;// 3;

    

    Topology T = Topology(numColoumns, numRows, numStarts, numEnds, numNodes);// initialize the topology object

    int outputLen = (T.tNumNodes*(T.tNumNodes-1))/2;// calculate the required ouput from the SDA
    this->maxConnections = outputLen;
    genSDAResponseLength = outputLen;// assign that value to the SDA response length global variable

    genEvolver(numStates, outputLen, numGen, T);// call the genetic algorithm
}