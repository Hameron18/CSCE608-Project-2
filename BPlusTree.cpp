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
        bool insertion(node* ptr, pair<node*, int> p1, pair<node*, int> p2);
        bool deletion(int val);
};

// Constructor
BPlusTree::BPlusTree() { root = nullptr; }

// Destructor
BPlusTree::~BPlusTree() {}

// Returns pointer to root
node* BPlusTree::getRoot() { return root; }

bool BPlusTree::insertion(node* ptr, pair<node*, int> p1, pair<node*, int> p2)
{
    node* p = p1.first;
    int k = p1.second;

    // Check to see if we are creating a root
    if (this->getRoot() == nullptr) {
        // Create root node
        root = new node();
        root->keys[0] =  k;
        root->numKeys = 1;
        root->leaf = true;
    } else {
        // Check if we are at a leaf or non-leaf node
        if (ptr->leaf == true) {
            // Leaf node

            // Find the index of where the new value should be inserted to
            int index;
            for (int i = 0; i < ptr->numKeys; i++) {
                index = i;
                if (ptr->keys[i] > k) {
                    break;
                }
            }

            // Make sure that there is space for the new value (Case 1)
            if (ptr->numKeys < order) {
                for (int i = ptr->numKeys-1; i > index; i--) {
                    // Push larger values forward
                    ptr->keys[i] = ptr->keys[i-1];

                    // No need to change pointers since this is a leaf node and there is room. 
                }

                // Insert value into node
                ptr->keys[index] = k;
            } else {
                // There is no space, create a new leaf and split values -- i.e., overflow (Case 2)
                // Create new node
                node* newLeaf = new node();

                // r = floor[(n+1)/2]
                int r = (order + 1) / 2; // integer truncation

                // Put values k_r+1 through k_n+1 in new leaf
                int j = 0;
                for (int i = r; i < order; i++) {
                    newLeaf->ptrs[j] = ptr->ptrs[i];
                    newLeaf->keys[j] = ptr->keys[i];
                    j++;
                }
                newLeaf->ptrs[j] = ptr->ptrs[order];

                // Put values k0 through kr in old leaf (ptr)


                // Point sequence pointer of new leaf where the old sequence pointer was
                // Point sequence pointer of old leaf to new leaf
                // If ptr is the root:
                    // Create new root with the two children and key k_r+1
                // Else:
                    // Return pair with new leaf and k_r+1
            }

        } else {
            // None leaf node
        }
        
    }

    // Assume we fail
    return false;
}

int main()
{
    // pair<string, int> p("This is a test pair ", 7);
    // cout << p.first << p.second << endl;

    // cout << "7 / 2 int division = " << 7/2 << endl;


    cout << "Program finished." << endl;
    return 0;
}