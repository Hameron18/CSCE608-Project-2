#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <tuple>
#include "string.h"

using namespace std;

int numDiskReads = 0;
int numDiskWrites = 0;
int freeBlocks = 15;

struct block {
    tuple<string, int, string>* tuples;
    int numTuples;
    int id;
    string relationName;
    block()
    {
        id = -1;
        numTuples = 0;
        tuples = new tuple<string, int, string>[8]();
        relationName = "";
    }
};

pair<int, block*>* mainMemory = new pair<int, block*>[15]();
vector<block*>* disk = new vector<block*>();

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
    int numVals = 50;
    int blockSize = 8;
    
    while (count < numVals) {
        // Create new tuple
        b = new block();
        b->id = _disk->size();
        b->relationName = "S";

        for (int i = 0; i < blockSize; i++) {
            // Create a random key
            randInt = rand() % 10;
            randVal += randInt;

            // Create a random string
            string randString = generateRandomString(5);

            // Input values into tuple
            tuple<string, int, string> t = make_tuple("", randVal, randString);
            b->tuples[i].swap(t);
            // b->tuples[i].first = randString;
            // b->tuples[i].second = randVal;

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

void generateRelationR(vector<block*>* _disk) {
    block* b;
    int randInt = 1;
    int randVal = 10000;
    int count = 0;
    int numVals = 50;
    int blockSize = 8;
    
    while (count < numVals) {
        // Create new tuple
        b = new block();
        b->id = _disk->size();
        b->relationName = "R";

        for (int i = 0; i < blockSize; i++) {
            // Create a random key
            randInt = rand() % 10;
            randVal += randInt;

            // Create a random string
            string randString = generateRandomString(5);

            // Input values into tuple
            tuple<string, int, string> t = make_tuple(randString, randVal, "");
            b->tuples[i].swap(t);
            // b->tuples[i].first = randString;
            // b->tuples[i].second = randVal;

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

// Reads one block from disk
void readFromDisk(int id)
{
    // Increment the number of disk reads
    numDiskReads++;

    // Check if there are any free 
    if (freeBlocks > 0) {
        for (int i = 0; i < 15; i++) {
            if (mainMemory[i].first == 0) {
                mainMemory[i].first = 1;
                *(mainMemory[i].second) = *(disk->at(i));
                freeBlocks--;
                break;
            }

            if (i == 14) {
                cout << "READ ERROR" << endl;
            }
        }
    } else {
        // No free blocks to write to, must free memory
        cout << "NOT ENOUGH SPACE FOR READ" << endl;
        return;
    }
}

// Writes one block to disk;
void writeToDisk(int index)
{
    // Increment the number of disk writes
    numDiskWrites++;

    // Check if memory block at given index has data
    if (mainMemory[index].first == 1) {
        int id = mainMemory[index].second->id;
        if ((id < disk->size()) && (id > 0)) {
            // Block exists in the disk, write data to disk
            *(disk->at(id)) = *(mainMemory[index].second);

            // Free block in main memory
            mainMemory[index].first == 0;
        } else {
            // Block doesn't exist in the disk yet
            block* b = new block();
            b = mainMemory[index].second;
            b->id = disk->size();
            disk->push_back(b);

            // Free block in main memory
            mainMemory[index].first == 0;
        }
    } else {
        // Data at requested block doesn't exist
        cout << "BLOCK DOES NOT HAVE ANY DATA" << endl;
        return;
    }
}

void printMainMemory()
{
    cout << "Contents of Main Memory:" << endl;
    for (int i = 0; i < 15; i++) {
        if (mainMemory[i].first == 0) {
            cout << "Memory Block #" << i << " is empty!" << endl;
            continue;
        }

        cout << "Memory Block #" << i << ":" << endl;
        for (int j = 0; j < mainMemory[i].second->numTuples; j++) {
            cout << "----[" << get<0>(mainMemory[i].second->tuples[j]) << ", " << get<1>(mainMemory[i].second->tuples[j]) << ", " << get<2>(mainMemory[i].second->tuples[j]) << "]" << endl;
        }
    }
    cout << endl;
}

int main()
{
    for (int i = 0; i < 15; i++) {
        mainMemory[i].first = 0;
        mainMemory[i].second = new block();
    }

    // printMainMemory();

    cout << "Generating relations..." << endl;
    generateRelationS(disk);
    generateRelationR(disk);

    cout << "Reading a block from the disk..." << endl;
    readFromDisk(0);

    printMainMemory();

    tuple<string, int, string> t = make_tuple("", get<1>(mainMemory[0].second->tuples[0]), "testing write");
    mainMemory[0].second->tuples[0] = t;

    printMainMemory();

    cout << "Writing a block to the disk..." << endl;
    writeToDisk(0);


    cout << "Printing disk blocks..." << endl;
    for (int i = 0; i < 3; i++) {
        cout << "Block #" << disk->at(i)->id << " for relation " << disk->at(i)->relationName << ":" << endl;
        for (int j = 0; j < disk->at(i)->numTuples; j++) {
            cout << "-- [" << get<0>(disk->at(i)->tuples[j]) << ", " << get<1>(disk->at(i)->tuples[j]) << ", " << get<2>(disk->at(i)->tuples[j]) << "]" << endl;
        }
    }

    cout << endl;
    cout << "Program finished." << endl;
    return 0;
}