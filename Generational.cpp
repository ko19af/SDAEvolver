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
int Generational::genMatingEvent(SDA *currentPop, SDA *newPop) {
    // Tournament Selection
    vector<int> tournIdxs = genTournSelect(genTournSize, genLowerBetter);
    SDA parent1, parent2, child1, child2;

    for (int event = elitism; event < genPopSize; event += 2) {
        do {
            parent1 = currentPop[tournIdxs[(int) lrand48() % genTournSize]];
            parent2 = currentPop[tournIdxs[(int) lrand48() % genTournSize]];
        } while (parent1 == parent2); // TODO: Ensure this check works

        child1.copy(parent1);
        child2.copy(parent2);

        // Crossover
        if(genCrossOp == 1 && drand48() < genCrossRate) child1.crossover(child2);

        // Mutation
        if (drand48() < genMutationRate) {
            if(genMutOperator == 1){
                child1.mutate(genNumMutations);
                child2.mutate(genNumMutations);
            }
        }

        // Add to new population
        newPop[event] = child1;
        newPop[event + 1] = child2;

        // Calculate fitness
        genPopFits[event] = genCalcFitness(child1);
        genPopFits[event + 1] = genCalcFitness(child2);
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

double Generational::genCalcFitness(SDA &member){

    int outputLen = (T.tNumNodes*(T.tNumNodes-1))/2;
    vector<int> c(outputLen);// vector for holding response from SDA
    member.fillOutput(c, false, cout);// fill vector using SDA
    T.setConnections(c, true);//set the connections in the topology

    // Fitness function sums all distances an edge node uses to reach a cloud node through topology
    int val = 0;
    for (int x = 0; x < T.numENodes; x++){// for each edge node
        vector<double> sPath;// create vector to record distance from edge node to all other nodes
        sPath.reserve(T.tNumNodes);
        sPath.assign(T.tNumNodes, DBL_MAX); // set all values to max double value
        sPath[x] = 0;// set distance to starting edge node to zero
        T.ShortestPath(x, sPath);// calculate shortest path to all nodes in topology from selected edge node

        for (int s:sPath) cout << to_string(s) + '\t';//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        cout << endl;

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
    outStrm << "Above 0.5: " << count << "\n";
    return 0;
}

int Generational::genEvolver(int SDANumStates, int SDAOutputLen, int numGenerations) {
    SDA *currentPop, *newPop;
    currentPop = new SDA[genPopSize];
    newPop = new SDA[genPopSize];
    genPopFits.reserve(genPopSize);
    Topology T;// initialize the topology of the network

    // Step 1: initialize the population
    for (int i = 0; i < genPopSize; ++i) {
        currentPop[i] = SDA(SDANumStates, genSDANumChars, genSDAResponseLength, SDAOutputLen);
        newPop[i] = SDA(SDANumStates, genSDANumChars, genSDAResponseLength, SDAOutputLen);
        genPopFits.push_back(genCalcFitness(currentPop[i]));
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
        genMatingEvent(currentPop, newPop);

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

    Topology T(5,5,1,1,3);
    this->T = T;

    int outputLen = (T.tNumNodes*(T.tNumNodes-1))/2;
    genSDAResponseLength = outputLen;

    genEvolver(numStates, outputLen, numGen);
}