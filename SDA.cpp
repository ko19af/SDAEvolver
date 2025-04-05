#include "SDA.h"

/**
 * Constructor for the SDA object that takes in the various parameters.  SDAs are
 * finite state machines where each transition appends characters to a buffer.  Buffers dictate
 * which transitions to transit through the machine and determine the output characters.
 * The SDA generated will have numStates states with numChars transitions emanating from each state.
 * Each transition will be composed of up to maxRespLen characters.  Each character will
 * be an integer between 0 and numChars-1, inclusively.  Thus, the output will be a vector
 * of integers which can be translated as needed to your preferred alphabet.  The output
 * generated will be outputLen number of characters.  The SDA will start at the state with
 * index initState and proceed using a random character determined in create().  If
 * verbose is true feedback will be printed to console.
 *
 * @param numStates The number of states in the SDA.
 * @param numChars The number of unique characters that you want in the output string.
 * @param maxRespLen The maximum number of characters appended to the SDA's buffer for each transition traversed.
 * @param outputLen The number of characters in the string generated by the SDA.
 * @param initState The initial state of the SDA (default value is 0).
 * @param verbose Whether or not you want text feedback from the SDA (default value is false).
 */
SDA::SDA(int numStates, int numChars, int maxRespLen, int outputLen, int initState, bool verbose) {
    initChar = -1;
    this->numStates = numStates;
    this->initState = initState;
    this->numChars = numChars;
    this->maxRespLen = maxRespLen;
    this->outputLen = outputLen;
    this->verbose = verbose;

    transitions.reserve(numStates);
    for (vector<int> v: transitions) {
        v.reserve(numChars);
    }

    responses.reserve(numStates);
    for (vector<vector<int> > vec1: responses) {
        vec1.reserve(numChars);
        for (vector<int> vec2: vec1) {
            vec2.reserve(maxRespLen);
        }
    }
    create();
    if (verbose) cout << "SDA made with " << numStates << " states." << endl;
}

/**
 * Default constructor which assigns default values for the parameters in the above constructor.
 * Namely, an SDA with 10 states, a two-character alphabet, responses with up to two characters,
 * and 1000 characters in the output.
 */
SDA::SDA() : SDA(10, 2, 2, 1000) {}

/**
 * A copy constructor.
 *
 * @param other The SDA to be copied.
 */
SDA::SDA(SDA &other) {
    copy(other);
}

/**
 * Destructor.
 */
SDA::~SDA() = default;

/**
 * This will randomly initialize the starting character for the SDA and randomly
 * generate the transitions and responses from each state.
 *
 * @return -1 if there is an error
 */
int SDA::create() {
    initChar = (int) lrand48() % numChars; // Randomize the initial character that drives the first transition.

    // Transitions
    vector<int> oneState;
    oneState.reserve(numChars);
    for (int state = 0; state < numStates; ++state) {
        oneState.clear();
        for (int val = 0; val < numChars; ++val) {
            oneState.push_back((int) lrand48() % numStates);
        }
        transitions.push_back(oneState);
    }

    // Responses
    vector<int> oneResponse;
    oneResponse.reserve(maxRespLen);
    vector<vector<int>> oneStateResps;
    oneStateResps.reserve(numChars);
    int respSize;
    for (int state = 0; state < numStates; ++state) {
        oneStateResps.clear();
        for (int trans = 0; trans < numChars; ++trans) {
            oneResponse.clear();
            respSize = (int) lrand48() % maxRespLen + 1;
            for (int val = 0; val < respSize; ++val) {
                oneResponse.push_back((int) lrand48() % numChars);
            }
            oneStateResps.push_back(oneResponse);
        }
        responses.push_back(oneStateResps);
    }
    if (verbose) cout << "SDA initialized." << endl;
    return 0;
}

vector<vector<vector<int>>> SDA::getResponses(){
    return this->responses;
}

int SDA::setOutputLen(int newLen) {
    this->outputLen = newLen;
    return 0;
}

/**
 * Randomly re-generate the initial character and the transitions and responses from
 * each state in the SDA.
 *
 * @return -1 if there is an error
 */
int SDA::randomize() {
    if (initChar < 0) {
        cout << "Error in SDA Class: randomize(): this SDA has not been initialized.";
        return -1;
    }

    initChar = (int) lrand48() % numChars;

    vector<int> oneResponse;
    oneResponse.reserve(maxRespLen);
    int respLen;
    for (int state = 0; state < numStates; ++state) {
        for (int trans = 0; trans < numChars; ++trans) {
            // Choose a new destination for this transition.
            transitions[state][trans] = (int) lrand48() % numStates;

            // Clear and generate a new response string for this transition.
            oneResponse.clear();
            respLen = (int) lrand48() % maxRespLen + 1;
            for (int val = 0; val < respLen; ++val) {
                oneResponse.push_back((int) lrand48() % numChars);
            }
            responses[state][trans] = oneResponse;
        }
    }
    if (verbose) cout << "SDA Randomized." << endl;
    return 0;
}

/**
 * Facilitate the allocation of space and copying of contents from other
 * to this SDA.
 *
 * @param other The SDA being copied
 * @return -1 if there is an error
 */
int SDA::copy(SDA &other) {
    if (initChar < 0) {
        cout << "Error in SDA Class: copy(...): this SDA has not been initialized.";
        return -1;
    }
    if (other.initChar < 0) {
        cout << "Error in SDA Class: copy(...): other SDA has not been initialized.";
        return -1;
    }

    initChar = other.initChar;

    // Ensure the correct space for the number of transitions and responses.
    if (numStates != other.numStates) {
        transitions.reserve(other.numStates);
        responses.reserve(other.numStates);
    }
    numStates = other.numStates;
    initState = other.initState;

    // Ensure the correct space for the number of transitions from each state.
    if (numChars != other.numChars) {
        for (auto &stateTrans: transitions) {
            stateTrans.reserve(other.numChars);
        }
        for (auto &stateResp: responses) {
            stateResp.reserve(other.numChars);
        }
    }
    numChars = other.numChars;

    // Ensure the correct space for the number of characters in each response.
    if (maxRespLen != other.maxRespLen) {
        for (auto &stateResp: responses) {
            for (auto &resp: stateResp) {
                resp.reserve(other.maxRespLen);
            }
        }
    }

    maxRespLen = other.maxRespLen;
    outputLen = other.outputLen;
    verbose = other.verbose;
    transitions = other.transitions;
    responses = other.responses;
    if (verbose) cout << "SDA Copied." << endl;
    return 0;
}

/**
 * Perform a two-point crossover between this SDA and the other SDA.  This will
 * swap the transitions and responses of this SDA with the other SDA between two
 * crossover points, A and B, with A < B.  The states with index A to B-1 will
 * be swapped. If A is 0 then we also swap the initChar's of the SDAs.
 *
 * @param other The other SDA
 * @return -1 if there is an error
 */
int SDA::crossover(SDA &other) {
    if (initChar < 0) {
        cout << "Error in SDA Class: crossover(...): this SDA has not been initialized.";
        return -1;
    }
    if (other.initChar < 0) {
        cout << "Error in SDA Class: crossover(...): other SDA has not been initialized.";
        return -1;
    }
    if (numStates != other.numStates) {
        cout << "Error in SDA Class: crossover(...): the two SDAs have a different numStates.";
        return -1;
    }
    if (numChars != other.numChars) {
        cout << "Error in SDA Class: crossover(...): the two SDAs have a different numChars.";
        return -1;
    }
    if (maxRespLen != other.maxRespLen) {
        cout << "Error in SDA Class: crossover(...): the two SDAs have a different maxRespLen.";
        return -1;
    }

    int crossStart, crossEnd, swapInt;
    vector<int> swapVec;
    swapVec.reserve(numChars);

    // Determine the values for crossStart and crossEnd
    do {
        crossStart = (int) lrand48() % (numStates + 1); // 0 <= crossStart <= numStates
        crossEnd = (int) lrand48() % (numStates + 1); // 0 <= crossEnd <= numStates
        // Ensure that crossStart < crossEnd
        if (crossStart > crossEnd) {
            swapInt = crossStart;
            crossStart = crossEnd;
            crossEnd = swapInt;
        }
    } while (crossStart == crossEnd); // Ensure crossStart != crossEnd

    // If crossing over state 0, also swap the initChar
    if (crossStart == 0) {
        swapInt = initChar;
        initChar = other.initChar;
        other.initChar = swapInt;
    }

    // This loop swaps the transitions and responses between this and the other SDA
    // for the states with index between crossStart and crossEnd - 1, inclusively
    for (int state = crossStart; state < crossEnd; state++) {
        swapVec = transitions[state];
        transitions[state] = other.transitions[state];
        other.transitions[state] = swapVec;
        for (int trans = 0; trans < numChars; trans++) {
            swapVec = responses[state][trans];
            responses[state][trans] = other.responses[state][trans];
            other.responses[state][trans] = swapVec;
        }
    }

    if (verbose) {
        cout << "Completed crossover between two SDAs with crossover points ";
        cout << crossStart << " and " << crossEnd << "." << endl;
    }
    return 0;
}

/**
 * This method mutates the structure of the SDA by performing numMuts number of mutations.  Each mutation
 * will either mutate the initial character of the SDA (4% likelihood) or it will randomly mutate a transition
 * (48% likelihood) or response in the SDA (48% likelihood).
 *
 * @param numMuts is the number of mutations that will occur
 * @return
 */
int SDA::mutate(int numMuts) {
    if (initChar < 0) {
        cout << "Error in SDA Class: mutate(...): this SDA has not been initialized.";
        return -1;
    }

    int mutPt, respSize;
    vector<int> oneResponse;

    for (int mut = 0; mut < numMuts; ++mut) {
        if (drand48() < 0.04) { // 4% chance of mutating initial character
            initChar = (int) lrand48() % numChars;
            if (verbose) {
                cout << "Completed mutation on the SDA's initial character." << endl;
            }
            return 0;
        } else {
            mutPt = (int) lrand48() % numStates;
            int transNum = (int) lrand48() % numChars;

            if ((int) lrand48() % 2 == 0) { // Mutate transition (48%)
                transitions.at(mutPt).at(transNum) = (int) lrand48() % numStates;
                if (verbose) {
                    cout << "Completed mutation for state " << mutPt << ": ";
                    cout << "New transition for character " << transNum << "." << endl;
                }
            } else { // Mutate response (48%)
                oneResponse.clear();
                respSize = (int) lrand48() % maxRespLen + 1;
                for (int i = 0; i < respSize; ++i) {
                    oneResponse.push_back((int) lrand48() % numChars);
                }
                responses.at(mutPt).at(transNum) = oneResponse;
                if (verbose) {
                    cout << "Completed mutation for state " << mutPt << ": ";
                    cout << "New response for character " << transNum << "." << endl;
                }
            }
        }
    }
    return 0;
}

int SDA::fillOutput(vector<int> &output, bool printToo, ostream &outStream) {
    if (initChar < 0) {
        cout << "Error in SDA Class: fillOutput(...): this SDA has not been initialized.";
        return -1;
    }
    if (output.capacity() < outputLen) {
        cout << "Error in SDA Class: fillOutput(...): output vector capacity is " << output.capacity();
        cout << " but the outputLen is " << outputLen << "." << endl;
        return -1;
    }

    int headIdx = 0;
    int tailIdx = 0;
    curState = initState;
    output[headIdx++] = initChar;
    if (printToo) outStream << initChar;

    while (headIdx < outputLen) {
        for (int val: responses[curState][output[tailIdx]]) {
            if (headIdx < outputLen) {
                output[headIdx++] = val;
                if (printToo) outStream << " " << val;
            }
        }
        curState = transitions[curState][output[tailIdx++]];
    }
    if (printToo) outStream << endl;
    return 0;
}

/**
 * Determines if this SDA and the other SDA are different from each other.
 *
 * @param other the SDA being compared to
 * @return true if different
 */
bool SDA::operator!=(SDA &other) {
    if (initChar != other.initChar) return true;
    if (numStates != other.numStates) return true;
    if (initState != other.initState) return true;
    if (curState != other.curState) return true;
    if (numChars != other.numChars) return true;
    if (maxRespLen != other.maxRespLen) return true;
    if (outputLen != other.outputLen) return true;
    if (verbose != other.verbose) return true;
    if (transitions != other.transitions) return true;
    if (responses != other.responses) return true;
    return false;
}

/**
 * Determines if this SDA and the other SDA are identical.
 *
 * @param other the SDA being compared to
 * @return true if identical
 */
bool SDA::operator==(SDA &other){
    SDA first(*this);
    SDA second(other);
    return !(first != second);
}

vector<int> SDA::rtnOutput(bool printToo, ostream &outStream) {
    if (initChar < 0) {
        cout << "Error in SDA Class: rtnOutput(...): this SDA has not been initialized.";
        return {-1};
    }

    vector<int> output(outputLen);
    fillOutput(output, printToo, outStream);
    return output;
}

/**
 * Print out the SDA to the provided ostream.  This will include the initial state,
 * initial character, and the transitions and responses on all of the states.
 *
 * @param to Where the SDA will be printed to
 * @return -1 if there is an error
 */
void SDA::print(ostream &to = cout) {
    if (initChar < 0) {
        cout << "Error in SDA Class: print(...): this SDA has not been initialized.";
    }

    to << initState << " <- " << initChar << endl;
    for (int state = 0; state < numStates; ++state) {
        for (int trans = 0; trans < numChars; ++trans) {
            to << state << " + " << trans << " -> " << transitions[state][trans] << " [";
            for (int vec: responses[state][trans]) {
                to << " " << vec;
            }
            to << " ]" << endl;
        }
    }
    if (verbose) cout << "SDA Printed." << endl;
}

SDA::SDA(int numStates, int numChars, int maxRespLen, int outputLen, vector<string> toConvert, int initState, bool verbose) {
    initChar = -1;
    this->numStates = numStates;
    this->initState = initState;
    this->numChars = numChars;
    this->maxRespLen = maxRespLen;
    this->outputLen = outputLen;
    this->verbose = verbose;

    transitions.reserve(numStates);
    for (vector<int> v: transitions) {
        v.reserve(numChars);
    }

    responses.reserve(numStates);
    for (vector<vector<int> > vec1: responses) {
        vec1.reserve(numChars);
        for (vector<int> vec2: vec1) {
            vec2.reserve(maxRespLen);
        }
    }

    for(string s : toConvert){// for each line of the SDA
        if (s[2] == '<'){// if its the initialization
            initState = s[0] - '0';// get the initial state
            initChar = s[0 + 5] - '0';// get the initial character
        }else{// else it is a response
            transitions[s[0] - '0'][s[4] - '0'] = s[9] - '0';// record the transition
            int c = 13;// start from first character of response
            vector<int> stateResponse;// vector recording the response
            do{ // record the response
                stateResponse.push_back(s[c] - '0');// push the character onto the response
            } while (s[c] != ']');// while not reached end of line
            responses[s[0] - '0'][s[9] - '0'] = stateResponse;// set the response for the transition
        }
    }
    
    if (verbose) cout << "SDA made with " << numStates << " states." << endl;
}