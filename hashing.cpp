#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <tuple>
#include "string.h"

using namespace std;

int numDiskReads = 0;
int numDiskWrites = 0;
int M = 15;
int freeBlocks = 15;
int numTuplesInS = 200;
int numTuplesInR = 40;
vector<int> usedKeys;

struct block {
    tuple<string, int, string>* tuples;
    int numTuples;
    int id;
    string relationName;
    bool inBucket;
    int bucketID;
    block()
    {
        id = -1;
        numTuples = 0;
        tuples = new tuple<string, int, string>[8]();
        relationName = "";
        inBucket = false;
        bucketID = -1;
    }
};

pair<int, block*>* mainMemory = new pair<int, block*>[M]();
vector<block*>* disk = new vector<block*>();

// Generates a random string for data values
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

// Generates Relation S(B, C)
void generateRelationS(vector<block*>* _disk)
{
    block* b;
    int randInt = 1;
    int randVal = 10000;
    int count = 0;
    int numVals = numTuplesInS;
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
            usedKeys.push_back(randVal);

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

// Generates relation R(A, B)
void generateRelationR(vector<block*>* _disk) {
    block* b;
    int randInt = 1;
    int randVal = 1;
    int count = 0;
    int numVals = numTuplesInR;
    int blockSize = 8;
    
    while (count < numVals) {
        // Create new tuple
        b = new block();
        b->id = _disk->size();
        b->relationName = "R";

        for (int i = 0; i < blockSize; i++) {
            // Pick a random key that was used in relation R
            randInt = rand() % usedKeys.size();
            randVal = usedKeys.at(randInt);

            // Create a random string
            string randString = generateRandomString(5);

            // Input values into tuple
            tuple<string, int, string> t = make_tuple(randString, randVal, "");
            b->tuples[i].swap(t);

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

// Writes one block to disk;
void writeToDisk(int index)
{
    // Increment the number of disk writes
    numDiskWrites++;

    // Check if memory block at given index has data
    if ((mainMemory[index].first == 1) && (mainMemory[index].second->numTuples > 0)) {
        int id = mainMemory[index].second->id;
        if ((id < disk->size()) && (id >= 0)) {
            // Block exists in the disk, write data to disk
            *(disk->at(id)) = *(mainMemory[index].second);

            // Free block in main memory
            block bk;
            mainMemory[index].first == 0;
            *(mainMemory[index].second) = bk;
            freeBlocks++;
        } else {
            // Block doesn't exist in the disk yet
            block* b = new block();
            *b = *(mainMemory[index].second);
            b->id = disk->size();
            disk->push_back(b);

            // Free block in main memory
            block bk;
            mainMemory[index].first == 0;
            *(mainMemory[index].second) = bk;
            freeBlocks++;
        }
    } else {
        // Data at requested block doesn't exist
        cout << "BLOCK DOES NOT HAVE ANY DATA" << endl;
        numDiskWrites--;
        return;
    }
}

// Read one block to a specific memory block
void readToMemBlock(int id, int memIndex)
{
    // Increment the number of disk reads
    numDiskReads++;

    // Check if there are any free 
    if (mainMemory[memIndex].first == 0) {
        mainMemory[memIndex].first = 1;
        *(mainMemory[memIndex].second) = *(disk->at(id));
        freeBlocks--;
    } else {
        // No free blocks to write to, must free memory
        cout << "NOT ENOUGH SPACE IN REQUESTED BLOCK" << endl;
        
        // Write block to the disk
        // writeToDisk(memIndex);
        mainMemory[memIndex].first = 0;
        mainMemory[memIndex].second = new block();
        freeBlocks++;

        // Read in new block from disk
        mainMemory[memIndex].first = 1;
        *(mainMemory[memIndex].second) = *(disk->at(id));
        freeBlocks--;

        return;
    }
}

// Reads one block from disk
void readFromDisk(int id)
{
    // Increment the number of disk reads
    numDiskReads++;

    // Check if there are any free 
    if (freeBlocks > 0) {
        for (int i = 0; i < M; i++) {
            if (mainMemory[i].first == 0) {
                mainMemory[i].first = 1;
                *(mainMemory[i].second) = *(disk->at(id));
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
        numDiskReads--;
        return;
    }
}

// Prints the contents of the main memory
void printMainMemory()
{
    cout << "Contents of Main Memory:" << endl;
    for (int i = 0; i < M; i++) {
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

// Clears a single block of memory
void clearMemoryBlock(int index)
{
    if (mainMemory[index].first == 1) {
        mainMemory[index].first = 0;
        mainMemory[index].second = new block();
        freeBlocks++;
    } else {
        cout << "Memory Block #" << index << " is already clear!" << endl;
    }
}

// Clears the main memory
void clearMainMemory()
{
    for (int i = 0; i < M; i++) {
        clearMemoryBlock(i);
    }
}

// Returns the bucket a tuple belongs to
int hashFunction(int key)
{
    return key % (M-1);
}

// A one-pass natural join operation implementation
void onePassNaturalJoin(int memIndex)
{
    
}

void twoPassHashingNaturalJoin() 
{
    int bucketNum;
    int diskSize = disk->size();
    int readBlock = 14;

    // Clear the main memory in preparation to hold M-1 buckets of S
    clearMainMemory();

    // Set buckets in main memory
    for (int i = 0; i < M-1; i++) {
        mainMemory[i].second->inBucket = true;
        mainMemory[i].second->bucketID = i;
        mainMemory[i].second->relationName = "S";
    }

    // Hash each tuple in S
    for (int i = 0; i < diskSize; i++) {
        if (disk->at(i)->relationName == "S") {
            // Read in disk block to memory block 15
            readToMemBlock(i, readBlock);

            for (int j = 0; j < mainMemory[readBlock].second->numTuples; j++) {
                bucketNum = hashFunction(get<1>(mainMemory[readBlock].second->tuples[j]));

                // Check if memory block is empty
                if (mainMemory[bucketNum].first == 1) {
                    // Check if there is space for tuple in the bucket's block
                    if (mainMemory[bucketNum].second->numTuples < 8) {
                        // There is space in block
                        mainMemory[bucketNum].second->tuples[mainMemory[bucketNum].second->numTuples] = mainMemory[readBlock].second->tuples[j];
                        mainMemory[bucketNum].second->numTuples++;
                    } else {
                        // There is NOT space in block
                        // Write bucket block to disk
                        writeToDisk(bucketNum);

                        // Create new block for memory block
                        block b;
                        b.inBucket = true;
                        b.bucketID = bucketNum;
                        b.relationName = "S";
                        b.tuples[0] = mainMemory[readBlock].second->tuples[j];
                        b.numTuples++;

                        mainMemory[bucketNum].first = 1;
                        *(mainMemory[bucketNum].second) = b;
                        freeBlocks--;
                    }
                } else {
                    // Memory block is empty, create a new block
                    block b;
                    b.inBucket = true;
                    b.bucketID = bucketNum;
                    b.relationName = "S";
                    b.tuples[0] = mainMemory[readBlock].second->tuples[j];
                    b.numTuples++;

                    mainMemory[bucketNum].first = 1;
                    *(mainMemory[bucketNum].second) = b;
                    freeBlocks--;
                }
            }
        }
    }

    // Write all remaining buckets for S back to disk
    for (int i = 0; i < M-1; i++) {
        writeToDisk(i);
    }

    // Clear read block in main memory
    clearMemoryBlock(M-1);

    // Set buckets in main memory
    for (int i = 0; i < M-1; i++) {
        mainMemory[i].second->inBucket = true;
        mainMemory[i].second->bucketID = i;
        mainMemory[i].second->relationName = "R";
    }

    // Hash each tuple in R
    for (int i = 0; i < diskSize; i++) {
        if (disk->at(i)->relationName == "R") {
            // Read in disk block to memory block 15
            readToMemBlock(i, readBlock);

            for (int j = 0; j < mainMemory[readBlock].second->numTuples; j++) {
                bucketNum = hashFunction(get<1>(mainMemory[readBlock].second->tuples[j]));

                // Check if memory block is empty
                if (mainMemory[bucketNum].first == 1) {
                    // Check if there is space for tuple in the bucket's block
                    if (mainMemory[bucketNum].second->numTuples < 8) {
                        // There is space in block
                        mainMemory[bucketNum].second->tuples[mainMemory[bucketNum].second->numTuples] = mainMemory[readBlock].second->tuples[j];
                        mainMemory[bucketNum].second->numTuples++;
                    } else {
                        // There is NOT space in block
                        // Write bucket block to disk
                        writeToDisk(bucketNum);

                        // Create new block for memory block
                        block b;
                        b.inBucket = true;
                        b.bucketID = bucketNum;
                        b.relationName = "R";
                        b.tuples[0] = mainMemory[readBlock].second->tuples[j];
                        b.numTuples++;

                        mainMemory[bucketNum].first = 1;
                        *(mainMemory[bucketNum].second) = b;
                        freeBlocks--;
                    }
                } else {
                    // Memory block is empty, create a new block
                    block b;
                    b.inBucket = true;
                    b.bucketID = bucketNum;
                    b.relationName = "R";
                    b.tuples[0] = mainMemory[readBlock].second->tuples[j];
                    b.numTuples++;

                    mainMemory[bucketNum].first = 1;
                    *(mainMemory[bucketNum].second) = b;
                    freeBlocks--;
                }
            }
        }
    }

    // Write all remaining buckets for R back to disk
    for (int i = 0; i < M-1; i++) {
        writeToDisk(i);
    }

    // Clear read block in main memory
    clearMemoryBlock(M-1);

    // For each bucket index i
        // call the one pass algorithm on the R_i bucket and S_i bucket
}

int main()
{
    for (int i = 0; i < M; i++) {
        mainMemory[i].first = 0;
        mainMemory[i].second = new block();
    }

    // printMainMemory();

    cout << "Generating relations..." << endl;
    generateRelationS(disk);
    generateRelationR(disk);

    cout << "Initial number of reads: " << numDiskReads << endl;
    cout << "Initial number of writes: " << numDiskWrites << endl;

    /*cout << "Reading from the disk..." << endl;
    readFromDisk(0);
    for (int i = 1; i < M; i++) {
        readFromDisk(i);
    }

    // printMainMemory();

    cout << "Editing main memory..." << endl;
    tuple<string, int, string> t = make_tuple("", get<1>(mainMemory[0].second->tuples[0]), "testing write");
    mainMemory[0].second->tuples[0] = t;

    clearMemoryBlock(1);
    tuple<string, int, string> t1 = make_tuple("", 13, "new tuple");
    mainMemory[1].first = 1;
    mainMemory[1].second->tuples[0] = t1;
    if (mainMemory[1].second->numTuples != 8) {
        mainMemory[1].second->numTuples++;
    }
    mainMemory[1].second->relationName = "A";
    freeBlocks--;

    // printMainMemory();

    cout << "Writing to the disk..." << endl;
    writeToDisk(0);
    writeToDisk(1);
    writeToDisk(2);*/

    cout << "Starting two-pass natural join..." << endl;
    twoPassHashingNaturalJoin();


    // cout << "Printing disk blocks..." << endl;
    // for (int i = 0; i < disk->size(); i++) {
    //     if (disk->at(i)->inBucket == true) {
    //         cout << "Block #" << disk->at(i)->id << " for relation " << disk->at(i)->relationName << ":" << endl;
    //         for (int j = 0; j < disk->at(i)->numTuples; j++) {
    //             cout << "-- [" << get<0>(disk->at(i)->tuples[j]) << ", " << get<1>(disk->at(i)->tuples[j]) << ", " << get<2>(disk->at(i)->tuples[j]) << "]" << endl;
    //         }
    //     }
    // }
    // cout << endl;

    cout << "Printing buckets of S..." << endl;
    for (int i = 0; i < M-1; i++) {
        cout << "-------" << endl;
        cout << "Blocks for bucket #" << i << ":" << endl;
        cout << "-------" << endl;
        for (int j = 0; j < disk->size(); j++) {
            if ((disk->at(j)->inBucket == true) && (disk->at(j)->bucketID == i) && (disk->at(j)->relationName == "S")) {
                cout << "Block #" << disk->at(j)->id << " for relation " << disk->at(j)->relationName << ":" << endl;
                for (int k = 0; k < disk->at(j)->numTuples; k++) {
                    cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "]" << endl;
                }
            }
        }
    }
    cout << endl;

    cout << "Printing buckets of R..." << endl;
    for (int i = 0; i < M-1; i++) {
        cout << "-------" << endl;
        cout << "Blocks for bucket #" << i << ":" << endl;
        cout << "-------" << endl;
        for (int j = 0; j < disk->size(); j++) {
            if ((disk->at(j)->inBucket == true) && (disk->at(j)->bucketID == i) && (disk->at(j)->relationName == "R")) {
                cout << "Block #" << disk->at(j)->id << " for relation " << disk->at(j)->relationName << ":" << endl;
                for (int k = 0; k < disk->at(j)->numTuples; k++) {
                    cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "]" << endl;
                }
            }
        }
    }
    cout << endl;

    cout << "Total number of reads: " << numDiskReads << endl;
    cout << "Total number of writes: " << numDiskWrites << endl;

    cout << endl;
    cout << "Program finished." << endl;
    return 0;
}