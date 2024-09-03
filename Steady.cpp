#include "Steady.h"

/**
 * Performs a single mating event in the population by performing tournament selection,
 * crossover on copies of the two most-fit members of the tournament, and mutation of these
 * copies.  Afterwards, the two least-fit members of the tournament are replaced with
 * the mutated children.
 *
 * @param population the population undergoing evolution
 * @return
 */
int Steady::MatingEvent(SDA* population){
    // Tournament Selection
    vector<int> tournIdxs = TournSelect(tournSize, lowerBetter);
    SDA parent1, parent2, child1, child2;
    parent1 = population[tournIdxs[0]];
    parent2 = population[tournIdxs[1]];
    child1.copy(parent1);
    child2.copy(parent2);

    // Crossover
    if(crossOp == 1 && drand48() < crossRate)child1.crossover(child2);

    // Mutation
    if (drand48() < mutationRate) {
        child1.mutate(numMutations);
        child1.mutate(numMutations);
    }

    // Update population with genrated children
    population[tournIdxs[tournSize - 1]] = child1;
    population[tournIdxs[tournSize - 2]] = child2;

    // Update fitness of worst two members of the tournament
    popFits[tournIdxs[tournSize-1]] = CalcFitness(child1);
    popFits[tournIdxs[tournSize-2]] = CalcFitness(child2);

    return 0;
}

/**
 * Performs a tournament selection deciding which members from the population, of the
 * current generation, will undergo a mating event to produce the children that will
 * populate the new population for the next generation
 *
 * KEVINDO: Update to work with Steady
 *
 * @param size of the returned vector of indicies from the tournament selection
 * @param decreasing is a boolean determining the ordering of the indicies in the returned vector based on their fitness
 * @return a vector of indices for members of the population sorted based on fitness
 */
vector<int> Steady::TournSelect(int size, bool decreasing) {
    vector<int> tournIdxs;
    int idxToAdd;

    tournIdxs.reserve(size);
    // If we want to sort the population, we call tournSelect with size == popsize
    if (size == popSize) {
        for (int idx = 0; idx < size; idx++) {
            tournIdxs.push_back(idx);
        }
    } else {
        do {
            idxToAdd = (int) lrand48() % popSize;
            // If this index is not already in tournIdxs
            if (count(tournIdxs.begin(), tournIdxs.end(), idxToAdd) == 0) {
                // Then add it
                tournIdxs.push_back(idxToAdd);
            }
        } while (tournIdxs.size() < size);
    }
    
    //sort(tournIdxs.begin(), tournIdxs.end(), CompareFitness);
    sort(tournIdxs.begin(), tournIdxs.end());
    if (decreasing) {
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
bool Steady::CompareFitness(int popIdx1, int popIdx2) {
    if (popFits[popIdx1] < popFits[popIdx2]) {
        return true;
    }
    if (popFits[popIdx1] > popFits[popIdx2]) {
        return false;
    }
    cout << "ERROR: compare fitness not working as intended!" << endl;
    return false;
}

double Steady::CalcFitness(SDA &member){

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

int Steady::PrintPopFits(ostream &outStrm, vector<double> &popFits) {
    outStrm << "Fitness Values: ";
    bool first = true;
    for (double fit: popFits) {
        // This ensures commas after each fitness value other than the last
        if (!first) {
            outStrm << ", ";
        }
        outStrm << fit;
        first = false;
    }
    outStrm << "\n";
    return 0;
}

/**
 * This method prints out the information of the best SDA
 * from the EA algorithm such as its fitness, the SDA, average fitness
 * and the best netowrk layout (Will come later when heurestic is complete)
 * 
 * @param outStrm is the designated output stream for the report
 * @param popFits is the vector holding the firness for the population
 * @param population is the population produced by the Genetic Algorithm
 * @return 
*/

int Steady::PrintReport(ostream &outStrm, vector<double> &popFits, SDA* population){
    double avgFit = 0;// Holds the average fitness
    int bestIdx = 0;// Stores the best member of populations location

    for (int x = 0; x < popFits.size(); x++){// For each entry
        avgFit += popFits[x];// Add to average sum
        if(popFits[bestIdx] < popFits[x]) bestIdx = x;// Get best population index
    }

    // Report the best SDA from GA
    outStrm << "Mutation Rate: " << mutationRate * 100 << "%" << endl;
    outStrm << "Best SDA: " << population[bestIdx].print(outStrm) << endl;
    outStrm << "Best Layout: " << "Coming Soon" << endl;
    outStrm << "Best Fitness: " << popFits[bestIdx] << endl;
    outStrm << "Average Fitness: " << avgFit/popFits.size() << endl;

    return 0;
}

int Steady::Evolver(int SDANumStates, int SDAOutputLen, int numMatingEvents){
    SDA* population;
    population = new SDA[popSize];
    popFits.reserve(popSize);

    // Step 1: initialize the population
    for (int i = 0; i < popSize; ++i) {
        population[i] = SDA(SDANumStates, SDANumChars, SDAResponseLength, SDAOutputLen);
        popFits[i] = CalcFitness(population[i]);
    }

    PrintPopFits(cout, popFits);

    // Step 2: Evolution
    for (int gen = 0; gen < numMatingEvents; ++gen) {
        MatingEvent(population);
    }

    // Step 3: Reporting
    PrintReport(cout, popFits, population);

    delete[] population;
    return 0;
}

Steady::Steady(int numStates, int numChars, int popSize, int tournSize, int numGen, int crossOp, double crossRate, int mutOperator, double mutRate, int heurFunction){
    Topology steadyT(5,5,1,1,3);
    this->T = T;
    this->SDANumChars = numChars;
    this->popSize = popSize;
    this->crossOp = crossOp;
    this->crossRate = crossRate;
    this->mutOperator = mutOperator;
    this->mutationRate = mutRate;
    this->tournSize = tournSize;

    this->heurFunction = heurFunction;

    int outputLen = (T.tNumNodes*(T.tNumNodes-1))/2;
    Evolver(numStates, outputLen, numGen);
}