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
        int search(int* ptr, int x);
        int rangeSearch(int* ptr, int x, int y);
        bool insertion(node* ptr, int val);
        bool deletion(int val);
};

// Constructor
BPlusTree::BPlusTree() { root = nullptr; }

// Destructor
BPlusTree::~BPlusTree() {}

// Returns pointer to root
node* BPlusTree::getRoot() { return root; }

bool BPlusTree::insertion(node* ptr, int val)
{
    // Check to see if we are creating a root
    if (this->getRoot() == nullptr) {
        // Create root node
        root = new node();
        root->keys[0] =  val;
        root->numKeys = 1;
        root->leaf = true;
    } else {
        // Check if we are adding to root node or if we need to traverse
        
    }

    // Assume we fail
    return false;
}

int main()
{

    return 0;
}