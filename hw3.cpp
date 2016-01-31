#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <algorithm>
#include <stdlib.h> // for generating test data
using namespace std;

void printCostMatrix();
void initialize();
void printSolution();
void sortVectors(vector<int>&,vector<int>&);
void generateData(int,int);
int methodType; // either backtracking or b&b
int numberOfNodes;
int numberOfEdges;
int optimumCost=INT_MAX;

vector<int> optimumSolution;
vector<vector<int> >costMatrix;

vector<vector<int> >c_lValues;


int costOfPartialSolution(vector<int> partialSolution){
    int res=0;
    for(int i =0;i<partialSolution.size()-1;i++){
        res+=costMatrix[partialSolution[i]][partialSolution[i+1]];
    }
    res+=costMatrix[partialSolution[0]][partialSolution[partialSolution.size()-1]];
    return res;
}


int reduce(vector<vector<int> > mPrime){
    int mPrimeSize=mPrime.size();
    int value=0;
    int min;

    // handle rows
    for(int i=0;i<mPrimeSize;i++){
        min=mPrime[i][0];
        for(int j=1;j<mPrimeSize;j++){
            if(mPrime[i][j]<min){
                min=mPrime[i][j];
            }
        }
        for(int j=0;j<mPrimeSize;j++){
            mPrime[i][j]-=min;
        }
        value+=min;
    }
    // handle columns
    for(int j=0;j<mPrimeSize;j++){
        min=mPrime[0][j];
        for(int i=1;i<mPrimeSize;i++){
            if(mPrime[i][j]<min){
                min=mPrime[i][j];
            }
        }
        for(int i=0;i<mPrimeSize;i++){
            mPrime[i][j]-=min;
        }
        value+=min;
    }
    return value;
}

int reduceBound(vector<int>partialSolution){
    // create mPrime matrix
    if(partialSolution.size()==0){
        return 0;
    }
    vector<vector<int> > mPrime;
    int size=numberOfNodes-partialSolution.size()+1;

    for(int i=0;i<size;i++){
        vector<int> temp;
        for(int j=0;j<size;j++){
            temp.push_back(INT_MAX);
        }
        mPrime.push_back(temp);
    }

    if(partialSolution.size()== numberOfNodes){
        return costOfPartialSolution(partialSolution);
    }
    mPrime[0][0]=INT_MAX;

    // first for loop
    int j=1;
    for(int i=0;i<numberOfNodes;i++){
        if(find(partialSolution.begin(), partialSolution.end(), i) == partialSolution.end()){
            int x_m1=partialSolution[partialSolution.size()-1];
            mPrime[0][j]=costMatrix[x_m1][i];
            j+=1;
        }
    }

    // second for loop
    int i=1;
    for(int j=0;j<numberOfNodes;j++){
        if(find(partialSolution.begin(), partialSolution.end(), j) == partialSolution.end()){
            mPrime[i][0]=costMatrix[j][partialSolution[0]];
            i+=1;
        }
    }

    // third for loop
    i=1;
    for(int p=0;p<numberOfNodes;p++){
        if(find(partialSolution.begin(), partialSolution.end(), p) == partialSolution.end()){
            j=1;
            for(int k=0;k<numberOfNodes;k++){
                if(find(partialSolution.begin(), partialSolution.end(), k) == partialSolution.end()){
                    mPrime[i][j]=costMatrix[p][k];
                    j+=1;
                }
            }
            i+=1;
        }
    }
    int answer=reduce(mPrime);
    // final for loop
    for(int i=1;i<partialSolution.size()-1;i++){
        answer+=costMatrix[partialSolution[i-1]][partialSolution[i]];
    }
    return answer;

}


void backtracking(int level,vector<int> solution){
    if(level==numberOfNodes){
        int currentCost=costOfPartialSolution(solution);
        if(currentCost<optimumCost){
            optimumCost=currentCost;
            optimumSolution=solution;
        }
    }

    if(level==0){
        c_lValues[0].clear();
        c_lValues[0].push_back(0);
    }
    else if(level ==1){
        c_lValues[1].clear();
        for(int i=1;i<numberOfNodes;i++){
            c_lValues[1].push_back(i);
        }
    }
    else{
        vector<int>temp=c_lValues[level-1];
        temp.erase(std::remove(temp.begin(), temp.end(), solution[solution.size()-1]), temp.end());
        c_lValues[level].clear();
        c_lValues[level]=temp;
    }
    int b=reduceBound(solution);

    for(int i=0;i<c_lValues[level].size();i++){
        if(b>=optimumCost){
            return ;
        }
        vector<int> nextRound=solution;
        nextRound.push_back(c_lValues[level][i]);
        backtracking(level+1,nextRound);
    }
}

void branchAndBound(int level, vector<int> solution){
    if(level==numberOfNodes){
        int currentCost=costOfPartialSolution(solution);
        if(currentCost<optimumCost){
            optimumCost=currentCost;
            optimumSolution=solution;
        }
    }
    if(level==0){
        c_lValues[0].clear();
        c_lValues[0].push_back(0);
    }
    else if(level ==1){
        c_lValues[1].clear();
        for(int i=1;i<numberOfNodes;i++){
            c_lValues[1].push_back(i);
        }
    }
    else{
        vector<int>temp=c_lValues[level-1];
        temp.erase(std::remove(temp.begin(), temp.end(), solution[solution.size()-1]), temp.end());
        c_lValues[level].clear();
        c_lValues[level]=temp;
    }
    int  count=0;
    vector<int>nextChoice;
    vector<int>nextBound;
    for(int i=0;i<c_lValues[level].size();i++){
        int x_l=c_lValues[level][i];
        nextChoice.push_back(x_l);
        vector<int> nextRound=solution;
        nextRound.push_back(x_l);
        nextBound.push_back(reduceBound(nextRound));
        count+=1;
    }
    // sort
    sortVectors(nextChoice,nextBound);
    for(int i=0;i<count;i++){
        if(nextBound[i]>=optimumCost){
            return ;
        }
        vector<int> nextSolution=solution;
        nextSolution.push_back(nextChoice[i]);
        branchAndBound(level+1,nextSolution);
    }

}

void sortVectors(vector<int> & choice,vector<int> & bound){
    for(int j=1;j<bound.size();j++){
            int key=bound[j];
            int key2=choice[j];
            int i=j-1;
            while(i>=0 && bound[i]>key){
                bound[i+1]=bound[i];
                choice[i+1]=choice[i];
                i-=1;
            }
            bound[i+1]=key;
            choice[i+1]=key2;
    }
}

int main(){
    vector<int> solution;
    initialize();
    for(int i=0;i<numberOfNodes;i++){
        vector<int> temp;
        c_lValues.push_back(temp);
    }
    if(methodType==1){
        backtracking(0,solution);
        printSolution();
    }
    else{
        branchAndBound(0,solution);
        printSolution();
    }
    //generateData(15,1);
    return 0;
}

void printSolution(){
        int result=0;
        for(int i=0;i<optimumSolution.size()-1;i++){
            result+=costMatrix[optimumSolution[i]][optimumSolution[i+1]];
        }
        result+=costMatrix[optimumSolution[optimumSolution.size()-1]][optimumSolution[0]];
        cout<<result<<endl;
        for(int i=0;i<optimumSolution.size()-1;i++){
            cout<<optimumSolution[i]+1<<" "<<optimumSolution[i+1]+1<<" "<<costMatrix[optimumSolution[i]][optimumSolution[i+1]]<<endl;
        }
        cout<<optimumSolution[optimumSolution.size()-1]+1<<" "<<optimumSolution[0]+1<<" "<<costMatrix[optimumSolution[optimumSolution.size()-1]][optimumSolution[0]]<<endl;
}
void initialize(){
    cin>>methodType;
    cin>>numberOfNodes>>numberOfEdges;

    // initialize matrix values to infinity
    for(int i=0;i<numberOfNodes;i++){
        vector<int> temp;
        for(int j=0;j<numberOfNodes;j++){
            temp.push_back(INT_MAX);
        }
        costMatrix.push_back(temp);
    }
    int counter =0;
    while(counter<numberOfEdges){
        int node1,node2,edgeWeight;
        cin>>node1>>node2>>edgeWeight;
        costMatrix[node1-1][node2-1]=edgeWeight;
        costMatrix[node2-1][node1-1]=edgeWeight;
        counter+=1;
    }
}
