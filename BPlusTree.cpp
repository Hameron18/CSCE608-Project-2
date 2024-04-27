#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include "string.h"

using namespace std;
 
string treeType = "dense";
int order = 3;

struct node {
    int* keys;
    int numKeys;
    node** ptrs;
    bool leaf;
    node()
    {
        keys = new int[order];
        numKeys = 0;
        ptrs = new node*[order+1]();
        for (int i = 0; i < order+1; i++) {
            ptrs[i] = nullptr;
        }
        leaf = false;
    }
};

class BPlusTree {
    private:
        node* root;

    public:
        BPlusTree();
        ~BPlusTree();
        node* getRoot();
        void setRoot(node* _root);
        pair<node*, int> search(node* ptr, int x);
        int rangeSearch(int* ptr, int x, int y);
        bool insert(int val);
        bool Delete(int val);
        bool insertion(node* ptr, pair<node*, int>* p1, pair<node*, int>* p2);
        bool deletion(node* ptr, pair<node*, int>* p1, bool* belowMin);
        void printRoot();
        void printNode(node* _node);
        void printTree(node* _node, int level);
};

// Constructor
BPlusTree::BPlusTree() { root = nullptr; }

// Destructor
BPlusTree::~BPlusTree() {}

// Returns pointer to root
node* BPlusTree::getRoot() { return root; }

// Sets pointer to root
void BPlusTree::setRoot(node* _root) { root = _root; }

// Returns pointer and index of key in leaf node if found, returns -1 and nullptr if not found
pair<node*, int> BPlusTree::search(node* ptr, int x)
{
    if (ptr->leaf == true) {
        for (int i = 0; i < ptr->numKeys; i++) {
            if (ptr->keys[i] == x) {
                return pair<node*, int>(ptr, i);
            }
        }
    } else {
        for (int i = 0; i < ptr->numKeys; i++) {
            if (ptr->keys[i] > x) {
                return search(ptr->ptrs[i], x);
            }
        }
        return search(ptr->ptrs[ptr->numKeys], x);
    }
    

    // Assume we fail
    return pair<node*, int>(nullptr, -1);
}

bool BPlusTree::insert(int val)
{
    pair<node*, int>* p1 = new pair<node*, int>(nullptr, val);
    pair<node*, int>* p2 = new pair<node*, int>(nullptr, 0);
    return insertion(this->getRoot(), p1, p2);
}

bool BPlusTree::Delete(int val)
{
    pair<node*, int>* p = new pair<node*, int>(nullptr, val);
    bool* bm = new bool(false);
    return deletion(this->getRoot(), p, bm);
}

bool BPlusTree::deletion(node* ptr, pair<node*, int>* p1, bool* belowMin)
{
    node* p = p1->first;
    int k = p1->second;

    // Check if ptr is a leaf (Case 1)
    if (ptr->leaf == true) {
        // Delete k from ptr
        bool found = false;
        for (int i = 0; i < ptr->numKeys; i++) {
            if (ptr->keys[i] == k) {
                // Key was found
                found = true;

                // Loop through remaining values and shift backwards
                for (int j = i+1; j < ptr->numKeys; j++) {
                    ptr->keys[j-1] = ptr->keys[j];
                }
                ptr->numKeys--;
                
                // Stop checking for key
                break;
            }
        }

        // If key was not found, return false
        if (found == false) {
            *belowMin = false;
            return false;
        }

        // If ptr has at least floor((n+1)/2) pointers OR ptr is the root, return belowMin = false
        int r = floor((order+1)/2);
        if ((ptr == this->getRoot()) || (ptr->numKeys >= r)) {
            *belowMin = false;
            return true;
        } else {
            *belowMin = true;
            return true;
        }
    } else { // Deleting at a non-leaf (Case 2)
        // Find key in ptr s.t. k < ki
        int index = 0;
        for (int i = 0; i < ptr->numKeys; i++) {
            if (k < ptr->keys[i]) {
                break;
            }
            index++;
        }

        // Delete at pointer for ki with a new belowMin variable
        bool* bm = new bool(false);
        bool success = deletion(ptr->ptrs[index], p1, bm);

        // Calculate minimum number of pointers
        int minPointers = ceil((order+1)/2);

        if (*bm == false) { // The deleted node still had enough keys after deletion
            *belowMin = false;
            if (success == true) {
                return true;
            } else {
                return false;
            }
        } else if (index == 0) { // The deleted node did not have enough keys left after deletion
            // Current node is the left most node, check right sibling only
            if ((ptr->ptrs[index+1]->numKeys + 1) > minPointers) {
                // Right sibling can share
                // Move one pointer-key pair from sibling to pi
                
            }

        } else if (index == ptr->numKeys) {
            // Current node is the right most node, check left sibling only
            if ((ptr->ptrs[index-1]->numKeys + 1) > minPointers) {
                // Left sibling can share
                // Move one pointer-key pair from sibling to pi
            }

        } else if ((ptr->ptrs[index-1]->numKeys + 1) > minPointers) {
            // Left sibling has extra pointers

        } else if ((ptr->ptrs[index+1]->numKeys + 1) > minPointers) {
            // Right sibling has extra pointers

        } else { // No siblings have extra pointers to share
            // Combine pi with an adjacent sibling of pi into a single node
            // If ptr is the root with only one pointer
            if ((ptr == this->getRoot()) && (ptr->numKeys == 1)) { // TODO: DOUBLE CHECK, this could be wrong
                ptr = ptr->ptrs[index];
                *belowMin = false;
                return success;
            } 

            // If ptr has at least the minimum number of pointers OR ptr is the root, return belowMin = false
            if ((ptr->numKeys+1 >= minPointers) || (ptr == this->getRoot())) {
                *belowMin = false;
                return success;
            } else {
                *belowMin = true;
                return success;
            }
        }
    }

    return false;
}

bool BPlusTree::insertion(node* ptr, pair<node*, int>* p1, pair<node*, int>* p2)
{
    node* p = p1->first;
    int k = p1->second;

    // Check to see if we are creating a root
    if (root == nullptr) {
        // Create root node
        root = new node();
        root->keys[0] =  k;
        root->numKeys = 1;
        root->leaf = true;
        return true;
    } else {
        // Check if we are at a leaf or non-leaf node
        if (ptr->leaf == true) {
            // Leaf node
            // Make sure that there is space for the new value (Case 1)
            if (ptr->numKeys < order) {
                // Find the index of where the new value should be inserted to
                int index = 0;
                for (int i = 0; i < ptr->numKeys; i++) {
                    if (ptr->keys[i] > k) {
                        break;
                    }
                    index++;
                }
                // cout << "Index for new value = " << index << endl;                

                // Move keys around to fit new key
                for (int i = ptr->numKeys; i > index; i--) {
                    // Push larger values forward
                    ptr->keys[i] = ptr->keys[i-1];
                }

                // Insert value into node
                ptr->keys[index] = k;

                // Increase node size
                ptr->numKeys++;

                // Return p' = nullptr, k' = 0
                p2->first = nullptr;
                p2->second = 0;
                return true;
            } else {
                // There is no space, create a new leaf and split values -- i.e., overflow (Case 2)
                // Find where new value should go
                int index = 0;
                for (int i = 0; i < ptr->numKeys; i++) {
                    if (ptr->keys[i] > k) {
                        break;
                    }
                    index++;
                }
                // cout << "Index for new value = " << index << endl;

                // Create new list to hold all values
                int* list = new int[order+1];

                // Transfer keys to new list
                for (int i = 0; i < index; i++) {
                    list[i] = ptr->keys[i];
                }
                list[index] = k;
                for (int i = index+1; i < order+1; i++) {
                    list[i] = ptr->keys[i-1];
                }

                cout << "list = [";
                for (int i = 0; i < order+1; i++) {
                    cout << list[i] << ", ";
                }
                cout << "]" << endl;

                // Create new node
                node* newLeaf = new node();
                newLeaf->leaf = true;

                // Set r = floor[(n+1)/2]
                int r = (order + 1) / 2; // integer truncation
                // cout << "r = " << r << endl;

                // Put values k_r+1 through k_n+1 in new leaf
                int j = 0;
                for (int i = r; i <= order; i++) {
                    newLeaf->keys[j] = list[i];
                    newLeaf->numKeys++;
                    j++;
                }

                // Change pointer directions for linked leaves
                newLeaf->ptrs[order] = ptr->ptrs[order];
                ptr->ptrs[order] = newLeaf;

                // Put values k0 through kr in old leaf (ptr)
                ptr->numKeys = 0;
                for (int i = 0; i < r; i++) {
                    ptr->keys[i] = list[i];
                    ptr->numKeys++;
                }

                // If ptr is the root:
                    // Create new root with the two children and key k_r+1
                if (ptr == root) {
                    node* newRoot = new node();
                    newRoot->leaf = false;
                    newRoot->keys[0] = newLeaf->keys[0];
                    newRoot->numKeys++;
                    newRoot->ptrs[0] = ptr;
                    newRoot->ptrs[1] = newLeaf;
                    root = newRoot;
                } else {
                    p2->first = newLeaf;
                    p2->second = newLeaf->keys[0];
                }
                
                return true;
            } 

        } else {
            // Non-leaf node
            // Find a key k_i where k_i-1 <= k < k_i
            int index = 0;
            for (int i = 0; i < ptr->numKeys; i++) {
                if (ptr->keys[i] > k) {
                    break;
                }
                index++;
            }

            // Insert with p_i, and new pair with p'' and k''
            pair<node*, int>* p__ = new pair<node*, int>(nullptr, 0);
            insertion(ptr->ptrs[index], p1, p__);

            // If p'' = nullptr
                // return k' = 0 and p' = nullptr
            if (p__->first == nullptr) {
                p2->first = nullptr;
                p2->second = 0;
                return true;
            } else if (ptr->numKeys < order) {
                // Else if there is room in ptr (Case 3)
                    // insert k'' into pt and return k' = 0 and p' = nullptr
                index = 0;
                for (int i = 0; i < ptr->numKeys; i++) {
                    if (ptr->keys[i] > p__->second) {
                        break;
                    }
                    index++;
                }

                // Move keys around to fit new key
                for (int i = ptr->numKeys; i > index; i--) {
                    // Push larger values forward
                    ptr->keys[i] = ptr->keys[i-1];

                    // Push pointers with keys (no sequence pointers)
                    ptr->ptrs[i+1] = ptr->ptrs[i];
                }

                // Insert value into node
                ptr->keys[index] = p__->second;
                ptr->ptrs[index+1] = p__->first;

                // Increase node size
                ptr->numKeys++;

                // Return p' = nullptr, k' = 0
                p2->first = nullptr;
                p2->second = 0;
                return true;
                
            } else {
                // Must split internal node, there is no room
                // Find where new value should go
                index = 0;
                for (int i = 0; i < ptr->numKeys; i++) {
                    if (ptr->keys[i] > p__->second) {
                        break;
                    }
                    index++;
                }
                // cout << "Index for new value = " << index << endl;

                // Create new list to hold values and pointers
                int* list = new int[order+1];
                node** plist = new node*[order+2];

                // Rearrange contnet in ptr and (p'', k'') into new list
                for (int i = 0; i < index; i++) {
                    list[i] = ptr->keys[i];
                    plist[i] = ptr->ptrs[i];
                }
                list[index] = p__->second;
                plist[index] = ptr->ptrs[index];
                plist[index+1] = p__->first;
                for (int i = index+1; i < order+1; i++) {
                    list[i] = ptr->keys[i-1];
                    plist[i+1] = ptr->ptrs[i];
                }

                // Create new node for p''
                node* newInternalNode = new node();
                newInternalNode->leaf = false;

                // Set r = ceil[(n+1)/2]
                int r = ((order + 1) / 2) + 1; // integer truncation
                // cout << "r = " << r << endl;

                // Put half of the list into new node
                int j = 0;
                for (int i = r; i <= order; i++) {
                    newInternalNode->keys[j] = list[i];
                    newInternalNode->numKeys++;
                    newInternalNode->ptrs[j] = plist[i];
                    j++;
                }
                newInternalNode->ptrs[j] = plist[order+1];

                // Put other half of list in ptr
                ptr->numKeys = 0;
                for (int i = 0; i < r-1; i++) {
                    ptr->keys[i] = list[i];
                    ptr->numKeys++;
                    ptr->ptrs[i] = plist[i];
                }
                ptr->ptrs[r-1] = plist[r-1];

                // If ptr is the root, create new root w/ children ptr and newInternalNode
                if (ptr == root) {
                    node* newRoot = new node();
                    newRoot->leaf = false;
                    newRoot->keys[0] = list[r-1];
                    newRoot->numKeys++;
                    newRoot->ptrs[0] = ptr;
                    newRoot->ptrs[1] = newInternalNode;
                    root = newRoot;
                } else {
                    p2->first = newInternalNode;
                    p2->second = list[r-1];
                }

                return true;
            }
        }
    }

    // Assume we fail
    return false;
}

void BPlusTree::printRoot()
{
    int* ks = root->keys;
    int n = root->numKeys;
    
    cout << "[";
    for (int i = 0; i < n; i++) {
        cout << ks[i] << ", ";
    }
    cout << "]" << endl;
}

void BPlusTree::printNode(node* _node)
{
    // cout << "Printing..." << endl;
    int* ks = _node->keys;
    int n = _node->numKeys;

    cout << "[";
    for (int i = 0; i < n; i++) {
        cout << ks[i] << ", ";
    }
    cout << "]" << endl;
    // cout << "Finished printing." << endl;
}

void BPlusTree::printTree(node* _node, int level)
{
    if (_node != nullptr) {
        for (int i = 0; i < level; i++) {
            cout << "--";
        }
        cout << "[";
        for (int i = 0; i < _node->numKeys-1; i++) {
            cout << _node->keys[i] << ", ";
        }
        cout << _node->keys[_node->numKeys-1] << "]" << endl;

        if (_node->leaf == false) {
            for (int i = 0; i < _node->numKeys+1; i++) {
                printTree(_node->ptrs[i], level+1);
            }
        }
    }
}

int getSmallestKey(node* subtree)
{   
    if (subtree->leaf == false) {
        return getSmallestKey(subtree->ptrs[0]);
    } else {
        return subtree->keys[0];
    }

    // Assume we fail
    return -1;
}

node* sparseTreeHelper(BPlusTree* newTree, vector<node*> v, int level) 
{
    cout << "Generating internal nodes for level = " << level << endl;

    // Create new list for internal parent nodes
    vector<node*> v2;

    // Create internal parent nodes for leafs
    int r = ceil((order+1.0) / 2.0); // min number of pointers for non-leaf is ceil[(n+1)/2]
    int count = 0;
    int size = v.size();

    if ((size - r) < r) {
        // Creating root
        // cout << "Creating root..." << endl;
        if (v.size() == 1) {
            return v[0];
        }
        r = size;
    }

    while (count < size) {
        // Create parent node
        node* newParent = new node();
        newParent->leaf = false;

        // Add pointers to new parent node
        for (int i = 0; i < r; i++) {
            newParent->ptrs[i] = v[count];
            if (i != 0) {
                newParent->keys[i-1] = getSmallestKey(v[count]);
                newParent->numKeys++;
            }
            count++;
        }
        
        // If there is not enough children left for a new parent, append remaining children to current parent
        if ((count != size) && ((size - count) < r)) {
            // Calculate new index in leaf
            int offset = newParent->numKeys;
            int bound = size - count;
            
            // Add remaining children to parent
            for (int i = 0; i < bound; i++) {
                newParent->ptrs[i+1+offset] = v[count];
                newParent->keys[i+offset] = getSmallestKey(v[count]);
                newParent->numKeys++;
                count++;
            }
        }

        // Add parent nodes to new list
        v2.push_back(newParent);
    }

    // // Prints parent nodes
    // for (int i = 0; i < v2.size(); i++) {
    //     cout << "-------------" << endl;
    //     // for (int j = 0; j < v2[i]->numKeys; j++) {
    //     //     cout << v2[i]->keys[j] << endl;
    //     // }
    //     newTree->printTree(v2[i], 0);
    //     cout << "-------------" << endl;
    // }

    if (v2.size() == 1) {
        cout << "Returning root" << endl;
        return v2[0];
    }

    return sparseTreeHelper(newTree, v2, level+1);
}

BPlusTree* generateSparseTree(int numVals)
{   
    // Create new B+ Tree
    BPlusTree* newTree = new BPlusTree();

    // Create list to store initial leaf nodes
    vector<node*> v;

    // Calculate number of values per leaf
    int r = (order + 1) / 2;
    int count = 0;
    int randInt = 1;
    int randVal = 1;
    int size;

    // Generate initial leaf nodes
    while (count < numVals) {
        // Create new node
        node* newNode = new node();
        newNode->leaf = true;

        // Add random values to node
        for (int i = 0; i < r; i++) {
            if (count == numVals) { break; }
            randInt = rand() % 20;
            if (randInt == 0) { randInt++; }
            randVal += randInt;
            newNode->keys[i] = randVal;
            newNode->numKeys++;
            count++;
        }

        // If there is not enough values left for a new leaf, append remaining values to current leaf
        if ((count != numVals) && ((numVals - count) < r)) {
            // Calculate new index in leaf
            int offset = newNode->numKeys;
            int bound = numVals - count;

            // Add remaining values to leaf
            for (int i = 0; i < bound; i++) {
                randInt = rand() % 20;
                if (randInt == 0) { randInt++; }
                randVal += randInt;
                newNode->keys[i+offset] = randVal;
                newNode->numKeys++;
                count++;
            }
        }

        // Add node to list and connect sequence pointers
        size = v.size();
        if (size == 0) {
            v.push_back(newNode);
        } else {
            v[size-1]->ptrs[order] = newNode;
            v.push_back(newNode);
        }
    }

    // Create internal parent nodes for internal nodes, repeat until root is made
    cout << "Creating internal nodes..." << endl;
    newTree->setRoot(sparseTreeHelper(newTree, v, 0));
    newTree->printTree(newTree->getRoot(), 0);

    return newTree;
}

node* denseTreeHelper(BPlusTree* newTree, vector<node*> v, int level)
{
    cout << "Generating internal nodes for level = " << level << endl;

    // Create new list for internal parent nodes
    vector<node*> v2;

    // Create internal parent nodes for leafs
    int r = ceil((order+1.0) / 2.0); // min number of pointers for non-leaf is ceil[(n+1)/2]
    int count = 0;
    int size = v.size();
    int limit = order+1;
    bool extraParent;
    node* sparseParent;

    if ((size - r) < r) {
        // Creating root
        // cout << "Creating root..." << endl;
        if (v.size() == 1) {
            return v[0];
        }
        limit = size;
    }

    while (count < size) {
        // Create parent node
        node* newParent = new node();
        newParent->leaf = false;

        if ((size - count) < limit) {
            limit = size - count;
        }

        // Add pointers to new parent node
        for (int i = 0; i < limit; i++) {
            newParent->ptrs[i] = v[count];
            if (i != 0) {
                newParent->keys[i-1] = getSmallestKey(v[count]);
                newParent->numKeys++;
            }
            count++;
        } 

        extraParent = false;

        // If there is not enough children left for a new parent, take children from previous parent and create new parent
        if ((count != size) && ((size - count) < r)) {
            // Calculate number of needed children to take from previous parent
            int n = r - (size - count);

            // Create new parent
            sparseParent = new node();

            // Transfer children from previous parent to current parent
            int j = 0;
            for (int i = order-n; i < order; i++) {
                if (j != 0) {
                    sparseParent->keys[j-1] = newParent->keys[i];
                    sparseParent->numKeys++;
                }
                sparseParent->ptrs[j] = newParent->ptrs[i+1];
                newParent->numKeys--;
                j++;
            }

            // Calculate new index in leaf
            int offset = n-1;
            int bound = size - count;
            
            // Add remaining values to sparse parent
            for (int i = 0; i < bound; i++) {
                sparseParent->ptrs[i+1+offset] = v[count];
                sparseParent->keys[i+offset] = getSmallestKey(v[count]);
                sparseParent->numKeys++;
                count++;
            }

            // Set extra parent flag
            extraParent = true;
        }

        // Add parent nodes to new list
        v2.push_back(newParent);

        // Add sparse parent if it exists
        if (extraParent == true) {
            v2.push_back(sparseParent);
        }
    }

    // // Prints parent nodes
    // for (int i = 0; i < v2.size(); i++) {
    //     cout << "-------------" << endl;
    //     // for (int j = 0; j < v2[i]->numKeys; j++) {
    //     //     cout << v2[i]->keys[j] << endl;
    //     // }
    //     newTree->printTree(v2[i], 0);
    //     cout << "-------------" << endl;
    // }

    if (v2.size() == 1) {
        cout << "Returning root" << endl;
        return v2[0];
    }

    return denseTreeHelper(newTree, v2, level+1);
}

BPlusTree* generateDenseTree(int numVals)
{
    // Create new B+ Tree
    BPlusTree* newTree = new BPlusTree();

    // Create list to store initial leaf nodes
    vector<node*> v;

    // Calculate number of values per leaf
    int r = (order + 1) / 2;
    int count = 0;
    int randInt = 1;
    int randVal = 1;
    int size;
    bool extraLeaf;

    // Generate initial leaf nodes
    while (count < numVals) {
        // Create new node
        node* newNode = new node();
        newNode->leaf = true;

        // Add random values to node
        for (int i = 0; i < order; i++) {
            if (count == numVals) { break; }
            randInt = rand() % 20;
            if (randInt == 0) { randInt++; }
            randVal += randInt;
            newNode->keys[i] = randVal;
            newNode->numKeys++;
            count++;
        }

        extraLeaf = false;

        // If there is not enough values left for a new leaf, take values from previous leaf and create new leaf
        if ((count != numVals) && ((numVals - count) < r)) {
            // Calculate number of needed values to take from previous leaf
            int n = r - (numVals - count);

            // Create new leaf
            node* sparseLeaf = new node();
            sparseLeaf->leaf = true;

            // Transfer nodes from previous leaf to current leaf
            int j = 0;
            for (int i = order-n; i < order; i++) {
                sparseLeaf->keys[j] = newNode->keys[i];
                sparseLeaf->numKeys++;
                newNode->numKeys--;
                j++;
            }

            // Calculate new index in leaf
            int offset = n;
            int bound = numVals - count;

            // Add remaining values to sparese leaf
            for (int i = 0; i < bound; i++) {
                randInt = rand() % 20;
                if (randInt == 0) { randInt++; }
                randVal += randInt;
                sparseLeaf->keys[i+offset] = randVal;
                sparseLeaf->numKeys++;
                count++;
            }

            // Make newNode point to sparseLeaf
            newNode->ptrs[order] = sparseLeaf;

            // Set extra leaf flag
            extraLeaf = true;
        }

        // Add node to list and connect sequence pointers
        size = v.size();
        if (size == 0) {
            v.push_back(newNode);
        } else {
            v[size-1]->ptrs[order] = newNode;
            v.push_back(newNode);
        }

        // Add extra leaf, if necessary
        if (extraLeaf == true) {
            v.push_back(v[size]->ptrs[order]);
        }
    }

    // for (int i = 0; i < v.size(); i++) {
    //     cout << "-----------" << endl;
    //     newTree->printTree(v[i], 0);
    //     cout << "-----------" << endl;
    // }

    // Create internal parent nodes for internal nodes, repeat until root is made
    cout << "Creating internal nodes..." << endl;
    newTree->setRoot(denseTreeHelper(newTree, v, 0));
    newTree->printTree(newTree->getRoot(), 0);

    return newTree;
}

int main()
{
    /*
    // BPlusTree tree;
    
    // bool one = tree.insert(17);
    // // cout << "Insertion attempt #1: " << one << endl;
    // bool two = tree.insert(11);
    // // cout << "Insertion attempt #2: " << two << endl;
    // bool three = tree.insert(23);
    // // cout << "Insertion attempt #3: " << three << endl;


    // bool four = tree.insert(5);
    // // cout << "Insertion attempt #4: " << four << endl;
    // bool five = tree.insert(3);
    // // cout << "Insertion attempt #5: " << five << endl;
    // bool six = tree.insert(31);
    // // cout << "Insertion attempt #6: " << six << endl;



    // bool seven = tree.insert(2);
    // // cout << "Insertion attempt #7: " << seven << endl;
    // bool eight = tree.insert(29);
    // // cout << "Insertion attempt #8: " << eight << endl;



    // bool nine = tree.insert(50);
    // // cout << "Insertion attempt #9: " << nine << endl;
    // bool ten = tree.insert(51);
    // // cout << "Insertion attempt #10: " << ten << endl;
    // bool eleven = tree.insert(12);
    // // cout << "Insertion attempt #11: " << eleven << endl;
    // bool twelve = tree.insert(13);
    // // cout << "Insertion attempt #12: " << twelve << endl;
    // bool thirteen = tree.insert(14);
    // // cout << "Insertion attempt #13: " << thirteen << endl;
    // bool fourteen = tree.insert(15);
    // // cout << "Insertion attempt #14: " << fourteen << endl;


    
    // tree.insert(6);
    // tree.insert(7);
    // tree.insert(8);
    // tree.insert(9);
    // tree.insert(52);
    // tree.insert(53);
    // tree.insert(54);
    // tree.insert(55);
    // tree.insert(56);



    // tree.insert(57);
    // // tree.insert(58);
    // // tree.insert(59);
    // // tree.insert(60);



    // cout << endl;
    // tree.printTree(tree.getRoot(), 0);
    // cout << endl;


    // // Testing search function
    // int searchKey = 56;
    // pair<node*, int> p = tree.search(tree.getRoot(), searchKey);

    // if (p.second != -1) {
    //     cout << "Key with value " << searchKey << " found in leaf at index " << p.second << endl;
    //     cout << endl;
    // } else {
    //     cout << "Key with value " << searchKey << " not found in tree!" << endl;
    //     cout << endl;
    // }
    */

    // int input;
    // cin >> input;
    // generateSparseTree(input);

    generateDenseTree(20);    

    cout << "Program finished." << endl;
    return 0;
}