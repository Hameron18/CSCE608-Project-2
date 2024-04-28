#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include "string.h"

using namespace std;

struct block {
    pair<string, int>* tuples;
    int numTuples;
    int id;
    string relationName;
    block()
    {
        id = -1;
        numTuples = 0;
        tuples = new pair<string, int>[8]();
        relationName = "";
    }
};

string generateRandomString(int l)
{
    string characters = "ACBDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    string result = "";
    int randInt;

    for (int i = 0; i < l; i++) {
        randInt = rand() % characters.length();
        result += characters[randInt];
    }

    // cout << "Result = " << result << endl;

    return result;
}

void generateRelationS(vector<block*>* _disk)
{
    block* b;
    int randInt = 1;
    int randVal = 10000;
    int count = 0;
    int numVals = 5000;
    int blockSize = 8;
    
    while (count < numVals) {
        // Create new tuple
        b = new block();
        b->id = _disk->size();
        b->relationName = "S";

        for (int i = 0; i < blockSize; i++) {
            // Create a random key
            randInt = rand() % 20;
            randVal += randInt;

            // Create a random string
            string randString = generateRandomString(5);

            // Input values into tuple
            b->tuples[i].first = randString;
            b->tuples[i].second = randVal;

            // Increment count and number of tuples in block
            b->numTuples++;
            count++;

            // Check limit
            if (count == numVals) {
                break;
            }
        }

        // Push new block to the disk;
        _disk->push_back(b);
    }
}

int main()
{
    block* mainMemory = new block[15]();
    vector<block*>* disk = new vector<block*>();

    generateRelationS(disk);

    cout << "Printing blocks..." << endl;
    for (int i = 0; i < disk->size(); i++) {
        cout << "Block #" << disk->at(i)->id << " for relation " << disk->at(i)->relationName << ":" << endl;
        for (int j = 0; j < disk->at(i)->numTuples; j++) {
            cout << "-- [" << disk->at(i)->tuples[j].first << ", " << disk->at(i)->tuples[j].second << "]" << endl;
        }
    }

    cout << endl;
    cout << "Program finished." << endl;
    return 0;
}