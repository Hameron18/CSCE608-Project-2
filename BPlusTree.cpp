#include <iostream>
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
        int search(node* ptr, int x);
        int rangeSearch(int* ptr, int x, int y);
        bool insert(int val);
        bool insertion(node* ptr, pair<node*, int>* p1, pair<node*, int>* p2);
        bool deletion(int val);
        void printRoot();
        void printNode(node* _node);
        void printTree(node* _node);
};

// Constructor
BPlusTree::BPlusTree() { root = nullptr; }

// Destructor
BPlusTree::~BPlusTree() {}

// Returns pointer to root
node* BPlusTree::getRoot() { return root; }

bool BPlusTree::insert(int val)
{
    pair<node*, int>* p1 = new pair<node*, int>(nullptr, val);
    pair<node*, int>* p2 = new pair<node*, int>(nullptr, 0);
    return insertion(this->getRoot(), p1, p2);
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
                // Else
                    // There is no room in ptr, must split internal node (Case 4)
                    // Rearrange content int ptr and k'' into new list
                    // Create new node at p''
                    // Put half of list into new node
                    // Put other half in ptr
                    // If ptr is the root
                        // Create new root with children ptr and p'' (w/ key kr)
                    // Else
                        // Return p' = p'' and k' = kr

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

void BPlusTree::printTree(node* _node)
{
    if (_node != nullptr) {
        cout << "[";
        for (int i = 0; i < _node->numKeys; i++) {
            cout << _node->keys[i] << ", ";
        }
        cout << "]" << endl;

        if (_node->leaf == false) {
            for (int i = 0; i < _node->numKeys+1; i++) {
                printTree(_node->ptrs[i]);
            }
        }
    }
}

int main()
{
    BPlusTree tree;
    
    bool one = tree.insert(17);
    // cout << "Insertion attempt #1: " << one << endl;
    bool two = tree.insert(11);
    // cout << "Insertion attempt #2: " << two << endl;
    bool three = tree.insert(23);
    // cout << "Insertion attempt #3: " << three << endl;

    // tree.printRoot();
    // cout << endl;

    bool four = tree.insert(5);
    // cout << "Insertion attempt #4: " << four << endl;
    bool five = tree.insert(3);
    // cout << "Insertion attempt #5: " << five << endl;
    bool six = tree.insert(31);
    // cout << "Insertion attempt #6: " << six << endl;

    // cout << "Root leaf status: " << tree.getRoot()->leaf << endl;
    // tree.printRoot();
    // for (int i = 0; i < tree.getRoot()->numKeys+1; i++) {
    //     tree.printNode(tree.getRoot()->ptrs[i]);
    // }
    // cout << endl;

    bool seven = tree.insert(2);
    // cout << "Insertion attempt #7: " << seven << endl;
    bool eight = tree.insert(29);
    // cout << "Insertion attempt #8: " << eight << endl;

    // tree.printRoot();
    // for (int i = 0; i < tree.getRoot()->numKeys+1; i++) {
    //     // cout << "i = " << i << endl;
    //     tree.printNode(tree.getRoot()->ptrs[i]);
    // }
    // cout << endl;

    bool nine = tree.insert(50);
    // cout << "Insertion attempt #9: " << nine << endl;
    bool ten = tree.insert(51);
    // cout << "Insertion attempt #10: " << ten << endl;
    bool eleven = tree.insert(12);
    // cout << "Insertion attempt #11: " << eleven << endl;
    bool twelve = tree.insert(13);
    // cout << "Insertion attempt #12: " << twelve << endl;
    bool thirteen = tree.insert(14);
    // cout << "Insertion attempt #13: " << thirteen << endl;
    bool fourteen = tree.insert(15);
    // cout << "Insertion attempt #14: " << fourteen << endl;
    tree.insert(6);
    tree.insert(7);
    tree.insert(8);
    tree.insert(9);
    tree.insert(52);
    tree.insert(53);
    tree.insert(54);
    tree.insert(55);
    tree.insert(56);

    cout << endl;
    // tree.printRoot();
    // for (int i = 0; i < tree.getRoot()->numKeys+1; i++) {
    //     // cout << "i = " << i << endl;
    //     tree.printNode(tree.getRoot()->ptrs[i]);
    // }
    // for (int i = 0; i < tree.getRoot()->ptrs[0]->numKeys+1; i++) {
    //     // cout << "i = " << i << endl;
    //     tree.printNode(tree.getRoot()->ptrs[0]->ptrs[i]);
    // }
    // for (int i = 0; i < tree.getRoot()->ptrs[1]->numKeys+1; i++) {
    //     // cout << "i = " << i << endl;
    //     tree.printNode(tree.getRoot()->ptrs[1]->ptrs[i]);
    // }
    // for (int i = 0; i < tree.getRoot()->ptrs[2]->numKeys+1; i++) {
    //     // cout << "i = " << i << endl;
    //     tree.printNode(tree.getRoot()->ptrs[2]->ptrs[i]);
    // }
    // for (int i = 0; i < tree.getRoot()->ptrs[3]->numKeys+1; i++) {
    //     // cout << "i = " << i << endl;
    //     tree.printNode(tree.getRoot()->ptrs[3]->ptrs[i]);
    // }
    // cout << endl;

    tree.insert(57);

    tree.printTree(tree.getRoot());
    cout << endl;

    cout << "Program finished." << endl;
    return 0;
}