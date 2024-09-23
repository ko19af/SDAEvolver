#include "Steady.h"

/**
 * Performs a single mating event in the population by performing tournament selection,
 * crossover on copies of the two most-fit members of the tournament, and mutation of these
 * copies. Afterwards, the two least-fit members of the tournament are replaced with
 * the mutated children.
 *
 * @param population the population undergoing evolution
 * @return
 */
int Steady::MatingEvent(SDA* population, Topology& T){
    // Tournament Selection
    vector<int> tournIdxs = TournSelect(tournSize, lowerBetter);
    SDA parent1, parent2, child1, child2, bc1, bc2;
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

    vector<int> c(SDAResponseLength, 0); // vector for holding response from SDA
    child1.fillOutput(c, false, cout);// fill vector using SDA of first child
    
    // Update fitness of worst two members of the tournament and set members dead status taking into account if they are dead
    if(necroticFilter(c, T)){// if member is dead
        popFits[tournIdxs[tournSize - 1]] = popWorstFit;// set childs fitness to worst fitness value
        dead[tournIdxs[tournSize - 1]] = true;// set its status as dead
    }
    else{// if member is not dead
        popFits[tournIdxs[tournSize - 1]] = CalcFitness(child1, T, tournIdxs[tournSize - 1]);// calculate members fitness
        dead[tournIdxs[tournSize - 1]] = false;// set its status as not dead
    }

    child2.fillOutput(c, false, cout);// fill vector using SDA of second child

    if(necroticFilter(c, T)){// repeat above process for second child
        popFits[tournIdxs[tournSize - 2]] = popWorstFit;
        dead[tournIdxs[tournSize - 2]] = true;
    }else{
        popFits[tournIdxs[tournSize-2]] = CalcFitness(child2, T, tournIdxs[tournSize-2]);
        dead[tournIdxs[tournSize - 2]] = false; 
    }
    return 0;
}

/**
 * A funct class designed to perform the fitness comparison for organizing the tournament indexes based on fitness
 */
class CompareFitness{
private:
    vector<double>& popFits;
public:
    CompareFitness(vector<double>& popFit) : popFits(popFit) {  }
 
    // operator function () on objects to compare their size and return boolean
    int operator () (int popIdx1, int popIdx2) const {return (popFits[popIdx1] > popFits[popIdx2]);}
};

/**
 * Performs a tournament selection deciding which members from the population, of the
 * current generation, will undergo a mating event to produce the children that will
 * populate the new population for the next generation
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
                tournIdxs.push_back(idxToAdd);// Then add it
            }
        } while (tournIdxs.size() < size);
    }

    sort(tournIdxs.begin(), tournIdxs.end(), CompareFitness(popFits));
    if (decreasing) reverse(tournIdxs.begin(), tournIdxs.end());
    
    return tournIdxs;
}


double Steady::CalcFitness(SDA &member, Topology& T, int idx){
    vector<int> c(SDAResponseLength, 0); // vector for holding response from SDA
    member.fillOutput(c, false, cout);// fill vector using SDA
    
    if(necroticFilter(c, T)){// check if the member is necrotic
        dead[idx] = true;// set dead value of the member to true
        return popWorstFit;// set the memebers fitness value to the populations current worst fitness
    }                      

    T.setConnections(c, false, heurFunction);//set the connections in the topology
    
    switch (heurFunction)// switch function determing which heurestic function is being used for evaluation
    {
    case 0:
        return distanceFitness(T);
            break;
        case 1:
            return dataFitness(T);
            break;
        case 2:
            return energyFitness(T);
            break;
        case 3:
            return distanceFitness(T) + energyFitness(T);
            break;
        case 4:
            return dataFitness(T) + energyFitness(T);
            break;
        case 5:
            return distanceFitness(T) + dataFitness(T);
            break;
        case 6:
            return distanceFitness(T) + dataFitness(T) + energyFitness(T);
        }
        return 0.0;
}

/**
 * This method determines if a member of the population is necrotic
 * by checking the number of connections present in its output
 * 
 * @param connections is the vector containing the connections from the member being examined
 * @param T is the Topology being examined
 */

bool Steady::necroticFilter(vector<int>& connections, Topology& T){
    int count = 0;// variable counting the number of connections in the member
    
    for (int val: connections) if(val == 1) count++;// count the number of connections in the members output
    
    if (count < 1 * T.tNumNodes || count > 7 * T.tNumNodes)  return true; // DEAD
    else return false;// return false if member is within bounds
}

/**
 * This method calculates the average amount of data flowing through each node in the network
 * 
 * @param T is the topology being used in the evaluation of the fitness
 * @return is the average amount of data being passed through the nodes in the network
 */

double Steady::dataFitness(Topology& T){
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

double Steady::distanceFitness(Topology& T){
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
 * @return is the average energy in the network
 */

double Steady::energyFitness(Topology& T){
    double val = 0.0;// is the fitness of the data being passed through the nodes in the topology
    for (double e : T.energyConsumption) val += e;// add up energy consumption in the network
    return val / (T.tNumNodes-T.numENodes);// return the averaged value for all the nodes in the network
}

/**
 * This method prints the fitness values of the population and determines which member has the worst fitness
 * and assigns that fitness to the dead memebers of the population
 * 
 * @param outStrm is where the program is prinitng out the  information
 * @param popFits is the vector holding the fitness of the population
 */

int Steady::PrintPopFits(ostream &outStrm, vector<double> &popFits) {
    outStrm << "Fitness Values: ";
    bool first = true;

    worst(true);// find worst value in the population

    for (int fit = 0; fit < popFits.size(); fit++){// for each entry in the popFits vector
       
        if(dead[fit]){// if member of population is dead
            popFits[fit] = popWorstFit;// set its fitness value to the worst value
            continue;// don't print ???????????????????????????????????????????????
        }

        // This ensures commas after each fitness value other than the last
        if (!first) outStrm << ", ";
        outStrm << popFits[fit];// print out the fitness of the member
        first = false;// set first to false as to start prinintg commas after first
    }
    outStrm << "\n";// print new line character after printing population
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
        if(popFits[bestIdx] > popFits[x]) bestIdx = x;// Get best population index
    }

    // Report the best SDA from GA
    outStrm << "Mutation Rate: " << mutationRate * 100 << "%" << endl;
    outStrm << "Best SDA: " << population[bestIdx].print(outStrm) << endl;
    outStrm << "Best Layout: " << "Coming Soon" << endl;
    outStrm << "Best Fitness: " << popFits[bestIdx] << endl;
    outStrm << "Average Fitness: " << avgFit/popFits.size() << endl;
    
    return 0;
}

int Steady::Evolver(int SDANumStates, int numMatingEvents, Topology& T, ostream& MyFile){
    SDA* population;
    population = new SDA[popSize];

    // Step 1: initialize the population
    for (int i = 0; i < popSize; ++i) {
        vector<int> SDAOutput(SDAResponseLength, 0); // vector for holding response from SDA
        do{
            population[i] = SDA(SDANumStates, SDANumChars, SDAResponseLength, SDAResponseLength); // create a new SDA
            population[i].fillOutput(SDAOutput, false, cout);// fill vector using SDA
        } while(necroticFilter(SDAOutput, T));// while the member is necrotic
        SDAOutput.clear();
        popFits.push_back(CalcFitness(population[i], T, i)); // calculate the fitness of the member
        dead.push_back(false);// set dead status as false
    }

    MyFile << "Initial Pop Fitness values: " << endl;

    // Step 2: Evolution
    for (int gen = 0; gen < numMatingEvents; ++gen) {
        if(gen % (numMatingEvents / 10) == 0) PrintPopFits(MyFile, popFits);// print the initial pop value and pop value every 10th generation
        MatingEvent(population, T);// perform mating event
    }

    // Step 3: Reporting
    MyFile << "Final Pop Fitness values: " << endl;
    PrintReport(MyFile, popFits, population);

    delete[] population;
    return 0;
}

/**
 * This method goes through the population and determines which fitness value is the worst
 * based on the param passed into the method
 * 
 * @param min decides if the worst value is the max value or min value in the population
 */

void Steady::worst(bool min){

    if (min) { // if minimizing the worst value is the largest fitness
        popWorstFit = 0;
        for (double val : popFits){// go through the populations fitness
            if (val > popWorstFit) popWorstFit = val;// if worst value found set it to worst value
        }
    }else{ // if maximizing the worst value is the smallest fitness
        popWorstFit = DBL_MAX;
        for (double val : popFits){
            if (val < popWorstFit) popWorstFit = val;
        }
    }
}

Steady::Steady(Topology& T, ofstream& MyFile, int numStates, int numChars, int popSize, int tournSize, int numGen, int crossOp, double crossRate, int mutOperator, double mutRate, int heurFunction){

    this->SDANumChars = numChars;
    this->popSize = popSize;
    this->crossOp = crossOp;
    this->crossRate = crossRate;
    this->mutOperator = mutOperator;
    this->mutationRate = mutRate;
    this->tournSize = tournSize;
    this->heurFunction = heurFunction;

    SDAResponseLength = (T.tNumNodes*(T.tNumNodes-1))/2;;// assign that value to the SDA response length global variable

    Evolver(numStates, numGen, T, MyFile);
}