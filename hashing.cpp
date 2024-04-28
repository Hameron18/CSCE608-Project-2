#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include "string.h"

using namespace std;



int main()
{
    pair<string, int>* t;

    t = new pair<string, int>("Test tuple", 1);

    cout << "The values of t are: ";
    cout << t->first << " and " << t->second << endl;

    cout << endl;
    cout << "Program finished." << endl;
    return 0;
}