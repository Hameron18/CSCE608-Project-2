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
int numTuplesInS = 5000;
int numTuplesInR = 1000;
vector<int> usedKeys;
vector<int> sharedKeys;
vector<int> joinKeys;

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
            if (randInt == 0) { randInt++; }
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

// Generates relation R(A, B) from used keys
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
            sharedKeys.push_back(randVal);

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

// Generates relation R(A, B) with keys between 20,000 and 30,000
void generateRelationR2(vector<block*>* _disk) {
    block* b;
    int randInt = 1;
    int randVal = 20000;
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
            randInt = rand() % 10;
            if (randInt == 0) { randInt++; }
            randVal += randInt;

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
        // cout << "NOT ENOUGH SPACE IN REQUESTED BLOCK" << endl;
        
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
        // cout << "Memory Block #" << index << " is already clear!" << endl;
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
void onePassNaturalJoin(int _bucketID)
{
    // Load entire bucket of R into memory
    for (int i = 0; i < disk->size(); i++) {
        if ((disk->at(i)->inBucket == true) && (disk->at(i)->bucketID == _bucketID) && (disk->at(i)->relationName == "R")) {
            readFromDisk(disk->at(i)->id);
        }
    }

    // For each S block in Si bucket
        // For each block in bucket Ri
            // Compare tuples in S block with tuples in R
            // If a tuple has the same B-value
                // If no space in the output block
                    // Write output block to disk
                    // Create new output block
                // Join tuples and put in th output block 
    
    for (int i = 0; i < disk->size(); i++) {
        if ((disk->at(i)->inBucket == true) && (disk->at(i)->bucketID == _bucketID) && (disk->at(i)->relationName == "S")) {
            readToMemBlock(disk->at(i)->id, M-2);

            int count = 0;
            int bound;
            while ((mainMemory[count].first == 1) && (count < 13)) {
                bound = mainMemory[count].second->numTuples;
                for (int j = 0; j < bound; j++) {
                    tuple<string, int, string> t = mainMemory[count].second->tuples[j];
                    for (int k = 0; k < mainMemory[M-2].second->numTuples; k++) {
                        if (get<1>(t) == get<1>(mainMemory[M-2].second->tuples[k])) {
                            // Need to join tuples in output block
                            if (mainMemory[M-1].first == 1) {
                                if (mainMemory[M-1].second->numTuples < 8) {
                                    // There is room in output block
                                    // Join tuples
                                    tuple<string, int, string> t1 = make_tuple(get<0>(t), get<1>(t), get<2>(mainMemory[M-2].second->tuples[k]));
                                    joinKeys.push_back(get<1>(t));

                                    // Add to output
                                    mainMemory[M-1].second->tuples[mainMemory[M-1].second->numTuples] = t1;
                                    mainMemory[M-1].second->numTuples++;
                                } else {
                                    // There is NO room in existing output block
                                    // Write old output block to disk
                                    mainMemory[M-1].second->relationName = "Z";
                                    writeToDisk(M-1);

                                    // Join tuples
                                    tuple<string, int, string> t1 = make_tuple(get<0>(t), get<1>(t), get<2>(mainMemory[M-2].second->tuples[k]));
                                    joinKeys.push_back(get<1>(t));

                                    // Create new memory block
                                    block b;
                                    b.inBucket = true;
                                    b.bucketID = _bucketID;
                                    b.relationName = "Z";
                                    b.tuples[0] = t1;
                                    b.numTuples++;

                                    mainMemory[M-1].first = 1;
                                    *(mainMemory[M-1].second) = b;
                                    freeBlocks--;
                                }
                            } else {
                                // Need to create output block
                                // Join tuples
                                tuple<string, int, string> t1 = make_tuple(get<0>(t), get<1>(t), get<2>(mainMemory[M-2].second->tuples[k]));
                                joinKeys.push_back(get<1>(t));

                                // Create block
                                block b;
                                b.inBucket = true;
                                b.bucketID = _bucketID;
                                b.relationName = "Z";
                                b.tuples[0] = t1;
                                b.numTuples++;
                                
                                // Set block
                                mainMemory[M-1].first = 1;
                                *(mainMemory[M-1].second) = b;
                                freeBlocks--;
                            }
                        }
                    }
                }
                count++;
            }

            // break;
        }
    }

    writeToDisk(M-1);

    // printMainMemory();
    clearMainMemory();
}

// A two-pass natural join operation implementation
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
    clearMainMemory();

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

    // Clear main memory
    clearMainMemory();

    // For each bucket index i
        // call the one pass algorithm on the R_i bucket and S_i bucket
    for (int i = 0; i < M-1; i++) {
        onePassNaturalJoin(i);
        // break;
    }
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
    cout << endl;

    cout << "Initial number of reads: " << numDiskReads << endl;
    cout << "Initial number of writes: " << numDiskWrites << endl;
    cout << endl;

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

    cout << "Starting two-pass hash-based natural join..." << endl;
    twoPassHashingNaturalJoin();
    cout << endl;

    
    int randInt;
    int randVal = 0;
    int key;
    bool end1 = false;
    bool end2 = false;
    bool end3 = false;
    
    cout << "Choosing 20 random tuples from join: (R --> S --> Joined)" << endl;
    for (int i = 0; i < 20; i++) {
        // Pick a random index for a shared key
        randInt = rand() % 10;
        if (randInt == 0) { randInt++; }
        randVal += randInt;
        key = sharedKeys.at(randVal);

        // Display tuple in R
        for (int j = 0; j < disk->size(); j++) {
            end1 = false;
            if ((disk->at(j)->inBucket == true) && (disk->at(j)->relationName == "R")) {
                for (int k = 0; k < disk->at(j)->numTuples; k++) {
                    if (get<1>(disk->at(j)->tuples[k]) == key) {
                        cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "] ";
                        end1 = true;
                        break;
                    }
                }
            }
            if (end1 == true) { break; }
        }

        // Display tuple in S
        for (int j = 0; j < disk->size(); j++) {
            end2 = false;
            if ((disk->at(j)->inBucket == true) && (disk->at(j)->relationName == "S")) {
                for (int k = 0; k < disk->at(j)->numTuples; k++) {
                    if (get<1>(disk->at(j)->tuples[k]) == key) {
                        cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "] ";
                        end2 = true;
                        break;
                    }
                }
            }
            if (end2 == true) { break; }
        }

        // Display the tuple
        for (int j = 0; j < disk->size(); j++) {
            end3 = false;
            if ((disk->at(j)->inBucket == true) && (disk->at(j)->relationName == "Z")) {
                for (int k = 0; k < disk->at(j)->numTuples; k++) {
                    if (get<1>(disk->at(j)->tuples[k]) == key) {
                        cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "]" << endl;
                        end3 = true;
                        break;
                    }
                }
            }
            if (end3 == true) { break; }
        }
    }
    cout << endl;

    // cout << "Printing all tuples from join: (R --> S --> Joined)" << endl;
    // for (int i = 0; i < joinKeys.size(); i++) {
    //     // Pick an index for a shared key
    //     key = joinKeys.at(i);

    //     // Display tuple in R
    //     for (int j = 0; j < disk->size(); j++) {
    //         end1 = false;
    //         if ((disk->at(j)->inBucket == true) && (disk->at(j)->relationName == "R")) {
    //             for (int k = 0; k < disk->at(j)->numTuples; k++) {
    //                 if (get<1>(disk->at(j)->tuples[k]) == key) {
    //                     cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "] ";
    //                     end1 = true;
    //                     break;
    //                 }
    //             }
    //         }
    //         if (end1 == true) { break; }
    //     }

    //     // Display tuple in S
    //     for (int j = 0; j < disk->size(); j++) {
    //         end2 = false;
    //         if ((disk->at(j)->inBucket == true) && (disk->at(j)->relationName == "S")) {
    //             for (int k = 0; k < disk->at(j)->numTuples; k++) {
    //                 if (get<1>(disk->at(j)->tuples[k]) == key) {
    //                     cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "] ";
    //                     end2 = true;
    //                     break;
    //                 }
    //             }
    //         }
    //         if (end2 == true) { break; }
    //     }

    //     // Display the tuple
    //     for (int j = 0; j < disk->size(); j++) {
    //         end3 = false;
    //         if ((disk->at(j)->inBucket == true) && (disk->at(j)->relationName == "Z")) {
    //             for (int k = 0; k < disk->at(j)->numTuples; k++) {
    //                 if (get<1>(disk->at(j)->tuples[k]) == key) {
    //                     cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "]" << endl;
    //                     end3 = true;
    //                     break;
    //                 }
    //             }
    //         }
    //         if (end3 == true) { break; }
    //     }
    // }
    // cout << endl;


    // cout << "Printing disk blocks..." << endl;
    // for (int i = 0; i < disk->size(); i++) {
    //     // if (disk->at(i)->inBucket == true) {
    //         cout << "Block #" << disk->at(i)->id << " for relation " << disk->at(i)->relationName << ":" << endl;
    //         for (int j = 0; j < disk->at(i)->numTuples; j++) {
    //             cout << "-- [" << get<0>(disk->at(i)->tuples[j]) << ", " << get<1>(disk->at(i)->tuples[j]) << ", " << get<2>(disk->at(i)->tuples[j]) << "]" << endl;
    //         }
    //     // }
    // }
    // cout << endl;
    // for (int i = 0; i < disk->size(); i++) {
    //     if ((disk->at(i)->inBucket == true) && (disk->at(i)->relationName == "R")) {
    //         cout << "First block ID for R bucket = " << disk->at(i)->id << endl;
    //         break;
    //     }
    // }
    // cout << "Printing buckets of S..." << endl;
    // for (int i = 0; i < M-1; i++) {
    //     cout << "-------" << endl;
    //     cout << "Blocks for bucket #" << i << ":" << endl;
    //     cout << "-------" << endl;
    //     for (int j = 0; j < disk->size(); j++) {
    //         if ((disk->at(j)->inBucket == true) && (disk->at(j)->bucketID == i) && (disk->at(j)->relationName == "S")) {
    //             cout << "Block #" << disk->at(j)->id << " for relation " << disk->at(j)->relationName << ":" << endl;
    //             for (int k = 0; k < disk->at(j)->numTuples; k++) {
    //                 cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "]" << endl;
    //             }
    //         }
    //     }
    // }
    // cout << endl;
    // cout << "Printing buckets of R..." << endl;
    // for (int i = 0; i < M-1; i++) {
    //     cout << "-------" << endl;
    //     cout << "Blocks for bucket #" << i << ":" << endl;
    //     cout << "-------" << endl;
    //     for (int j = 0; j < disk->size(); j++) {
    //         if ((disk->at(j)->inBucket == true) && (disk->at(j)->bucketID == i) && (disk->at(j)->relationName == "R")) {
    //             cout << "Block #" << disk->at(j)->id << " for relation " << disk->at(j)->relationName << ":" << endl;
    //             for (int k = 0; k < disk->at(j)->numTuples; k++) {
    //                 cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "]" << endl;
    //             }
    //         }
    //     }
    // }
    // cout << endl;

    // cout << "Printing buckets of Z..." << endl;
    // for (int i = 0; i < M-1; i++) {
    //     cout << "-------" << endl;
    //     cout << "Blocks for bucket #" << i << ":" << endl;
    //     cout << "-------" << endl;
    //     for (int j = 0; j < disk->size(); j++) {
    //         if ((disk->at(j)->inBucket == true) && (disk->at(j)->bucketID == i) && (disk->at(j)->relationName == "Z")) {
    //             cout << "Block #" << disk->at(j)->id << " for relation " << disk->at(j)->relationName << ":" << endl;
    //             for (int k = 0; k < disk->at(j)->numTuples; k++) {
    //                 cout << "-- [" << get<0>(disk->at(j)->tuples[k]) << ", " << get<1>(disk->at(j)->tuples[k]) << ", " << get<2>(disk->at(j)->tuples[k]) << "]" << endl;
    //             }
    //         }
    //     }
    // }
    // cout << endl;

    cout << "Total number of reads: " << numDiskReads << endl;
    cout << "Total number of writes: " << numDiskWrites << endl;

    cout << endl;
    cout << "Program finished." << endl;
    return 0;
}