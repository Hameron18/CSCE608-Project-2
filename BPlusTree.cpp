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

        } else {
            
        }
        
    }

    // Assume we fail
    return false;
}

int main()
{
    pair<string, int> p("This is a test pair ", 7);
    cout << p.first << p.second << endl;


    cout << "Program finished." << endl;
    return 0;
}