#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include "string"

using namespace std;

enum stateType {nullSt, startSt, acceptSt, rejectSt};

struct transition{
    //“q,a->r,b,x”
    char a;
    int r;
    char b;
    char x;

};

class State{
public:
    int q;
    stateType stTyp;
    //find the transition for the given input
    vector<transition> transitions;
    State(){
    }
    ~State(){
        for(int t = 0; t < transitions.size(); t++){
            free(transitions.at(t));
        }
    }
};

int processFile(char* fileName, vector<State*>* states){
    ifstream inputFile(fileName);
    string currLine;
    int startState;

    if(!inputFile.is_open()){
        printf("Could not open input file.\n");
        return -1;
    }

    transition newTrans;
    string subLine;
    string tempSub;
    int tempState;
    bool existsQ;
    bool existsR;


    while(getline(inputFile, currLine)){
        existsQ = false;
        existsR = false;
        istringstream lineStream(currLine);
        while(getline(lineStream, subLine, '\t')){
            //identify what kind of line it is
            if(subLine == "state"){
                State* newState = new State();
                //get number of state
                getline(lineStream, tempSub, '\t');
                //store the number of the state
                newState->q = atoi(tempSub.c_str());
                //get the type of state it is
                getline(lineStream, tempSub, '\t');
                if(strcmp(tempSub.c_str(), "start") == 0){
                    newState->stTyp = startSt;
                    startState = newState->q;
                } else if(strcmp(tempSub.c_str(), "accept") == 0){
                    newState->stTyp = acceptSt;
                } else if(strcmp(tempSub.c_str(), "reject") == 0){
                    newState->stTyp = rejectSt;
                } else {
                    newState->stTyp = nullSt;
                }
                (*states).push_back(newState);
            } else if(subLine == "transition"){
                //get the initial state and turn it into an int
                getline(lineStream, tempSub, '\t');
                tempState = atoi(tempSub.c_str());
                //get the input symbol
                getline(lineStream, tempSub, '\t');
                newTrans.a = tempSub[0];
                //get the transition state and turn it into an int
                getline(lineStream, tempSub, '\t');
                newTrans.r = atoi(tempSub.c_str());
                //get the replacement symbol
                getline(lineStream, tempSub, '\t');
                newTrans.b = tempSub[0];
                //get the L or R
                getline(lineStream, tempSub, '\t');
                newTrans.x = tempSub[0];

                for(int i = 0; i < (*states).size(); i++){
                    //check if state exists
                    if((*states).at(i)->q == tempState){
                        //if it does add transition to it
                        (*states).at(i)->transitions.push_back(newTrans);
                        existsQ = true;
                    }
                    //check if target state exists
                    if((*states).at(i)->q == newTrans.r){
                        existsR = true;
                    }
                }
                //if state was not previously defined
                if(!existsQ){
                    State* newState = new State();
                    newState->q = tempState;
                    newState->transitions.push_back(newTrans);
                    (*states).push_back(newState);
                    if(tempState == newTrans.r){
                        existsR = true;
                    }
                }
                if(!existsR){
                    State* newState = new State();
                    newState->q = newTrans.r;
                    (*states).push_back(newState);
                }
            }
        }
    }
    inputFile.close();
    /*
    //test states and transitions are stored properly
    transition tempTrans;
    for(int i = 0; i < states.size(); i++){
        if(states.at(i)->transitions.size() == 0){
            printf("%d", states.at(i)->q);
            printf("\n");
        } else{
            for(int j = 0; j < states.at(i)->transitions.size(); j++){
                printf("%d,", states.at(i)->q);
                tempTrans = states.at(i)->transitions.at(j);
                printf("%c->%d,%c,%c\n", tempTrans.a, tempTrans.r, tempTrans.b, tempTrans.x);
            }
        }
    }
    */
    return startState;
}

void stateMachine(vector<State*>* states, int startState, char* inputString, int maxTrans){
    char newInputStr[sizeof(inputString)+2];
    sprintf(newInputStr, "_%s_", inputString);
    State* st;
    bool terminated = false;

    for(int i = 0; i < (*states).size(); i++){
        if((*states).at(i)->q == startState){
            st = (*states).at(i);
            break;
        }
    }

    int i = 1;
    int newQ;
    while(i <= strlen(newInputStr) && maxTrans > 0){
        if(st->stTyp == acceptSt){
            printf("%d accept\n", st->q);
            terminated = true;
            break;
        } else if(st->stTyp == rejectSt){
            printf("%d reject\n", st->q);
            terminated = true;
            break;
        } else{
            maxTrans--;
            printf("%d->", st->q);
            for(int j = 0; j < st->transitions.size(); j++){
                if(st->transitions[j].a == newInputStr[i]){
                    newQ = st->transitions[j].r;
                    newInputStr[i] = st->transitions[j].b;
                    if(st->transitions[j].x == 'L'){
                        i--;
                    } else{
                        i++;
                    }
                    break;
                }
            }
            //if it isn't a self loop then find a state to transition to
            if(st->q != newQ){
                for(int s = 0; s < (*states).size(); s++){
                    //if we find a state that matches the transition's r
                    if((*states).at(s)->q == newQ){
                        st = (*states).at(s);
                        break;
                    }
                }
            }
        }
    }
    if(!terminated){
        printf("%d quit\n", st->q);
    }
    return;
}

int main(int argc, char *argv[]){
    char* fileName = argv[1];
    char* inputString = argv[2];
    int maxTrans = atoi(argv[3]);
    //find the state with the matching number
    vector<State*> states;
    //parses file and stores contents into state vector
    //returns pointer to start state
    int startState = processFile(fileName, &states);
    stateMachine(&states, startState, inputString, maxTrans);



    /*
    ifstream inputFile(fileName);
    string currLine;

    if(!inputFile.is_open()){
        printf("Could not open input file.\n");
        return 0;
    }

    transition newTrans;
    char tempA;
    string subLine;
    string tempSub;
    int tempState;
    bool existsQ;
    bool existsR;


    while(getline(inputFile, currLine)){
        existsQ = false;
        existsR = false;
        istringstream lineStream(currLine);
        while(getline(lineStream, subLine, '\t')){
            //identify what kind of line it is
            if(subLine == "state"){
                State* newState = new State();
                //get number of state
                getline(lineStream, tempSub, '\t');
                //store the number of the state
                newState->q = atoi(tempSub.c_str());
                //get the type of state it is
                getline(lineStream, tempSub, '\t');
                if(strcmp(tempSub.c_str(), "start")){
                    newState->stTyp = startSt;
                } else if(strcmp(tempSub.c_str(), "accept")){
                    newState->stTyp = acceptSt;
                } else if(strcmp(tempSub.c_str(), "reject")){
                    newState->stTyp = rejectSt;
                } else {
                    newState->stTyp = nullSt;
                }
                states.push_back(newState);
            } else if(subLine == "transition"){
                //get the initial state and turn it into an int
                getline(lineStream, tempSub, '\t');
                tempState = atoi(tempSub.c_str());
                //get the input symbol
                getline(lineStream, tempSub, '\t');
                newTrans.a = tempSub[0];
                //get the transition state and turn it into an int
                getline(lineStream, tempSub, '\t');
                newTrans.r = atoi(tempSub.c_str());
                //get the replacement symbol
                getline(lineStream, tempSub, '\t');
                newTrans.b = tempSub[0];
                //get the L or R
                getline(lineStream, tempSub, '\t');
                newTrans.x = tempSub[0];

                for(int i = 0; i < states.size(); i++){
                    //check if state exists
                    if(states.at(i)->q == tempState){
                        //if it does add transition to it
                        states.at(i)->transitions.push_back(newTrans);
                        existsQ = true;
                    }
                    //check if target state exists
                    if(states.at(i)->q == newTrans.r){
                        existsR = true;
                    }
                }
                //if state was not previously defined
                if(!existsQ){
                    State* newState = new State();
                    newState->q = tempState;
                    newState->transitions.push_back(newTrans);
                    states.push_back(newState);
                    if(tempState == newTrans.r){
                        existsR = true;
                    }
                }
                if(!existsR){
                    State* newState = new State();
                    newState->q = newTrans.r;
                    states.push_back(newState);
                }
            }
        }
    }
    */

    /*
    //test states and transitions are stored properly
    transition tempTrans;
    for(int i = 0; i < states.size(); i++){
        if(states.at(i)->transitions.size() == 0){
            printf("%d", states.at(i)->q);
            printf("\n");
        } else{
            for(int j = 0; j < states.at(i)->transitions.size(); j++){
                printf("%d,", states.at(i)->q);
                tempTrans = states.at(i)->transitions.at(j);
                printf("%c->%d,%c,%c\n", tempTrans.a, tempTrans.r, tempTrans.b, tempTrans.x);
            }
        }
    }
    */
    for(int s = 0; s < states.size(); s++){
        delete states.at(s);
    }
    return 0;

}
