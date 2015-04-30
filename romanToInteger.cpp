#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <iostream> 
#include <queue>

using std::string;
using std::cout;
using std::queue;

char* stringToCharArray(string s);
int convertToInt(char val);
int romanToInt(string s);
int sumQueue(queue<int> **aQueue);


int main(int argc, char** argv ){
    string input = argv[1];
    cout << romanToInt(input) << std::endl;
}

int convertToInt(char val){
    switch(val){
        case 'I':
            return 1;
            break;
        case 'V':
            return 5;
            break;
        case 'X':
            return 10;
            break;
        case 'L':
            return 50;
            break;
        case 'C':
            return 100;
            break;
        case 'D':
            return 500;
            break;
        case 'M':
            return 1000;
            break;
        default:
            return 0;
    }
}

int romanToInt(string s){
    char* romanArray = stringToCharArray(s);
    queue<int>* operationMaster = new queue<int>;
    int localMax = 0;
    int sum = 0;
    for(int i = 0; romanArray[i]; i++){
        if(convertToInt(romanArray[i]) > localMax){
            localMax = convertToInt(romanArray[i]);
            operationMaster->push(localMax);
            continue;
        }
        sum += sumQueue(&operationMaster);
        localMax = convertToInt(romanArray[i]);
        operationMaster->push(localMax);
    }
    sum += sumQueue(&operationMaster);
    return sum;
}

int sumQueue(queue<int> **aQueue){
    int subtract = 0;
    int result = 0;
    while(!(*aQueue)->empty()){
        if((*aQueue)->size() == 1){
            result = (*aQueue)->front() - subtract;
            (*aQueue)->pop();
            break;
        }
        subtract += (*aQueue)->front();
        (*aQueue)->pop();
    }
    return result;
}

char* stringToCharArray(string s){
    char *a=new char[s.size()+1];
    a[s.size()]=0;
    memcpy(a,s.c_str(),s.size());
    return a;
}