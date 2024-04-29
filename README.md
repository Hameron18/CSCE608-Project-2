# CSCE608-Project-2

In order to run either of these code examples, they need to be compiled first with a C++ compiler using a command such as "g++ BPlusTree.cpp" or "g++ hashing.cpp"
Afterwards, simply run the command "./a.out" and the respective example will run.
The B+ Tree code will automatically run for you without any issues, but the hashing file requires some changes in order to test both types of data generation.
The default way for using the hashing implementation will use the method of data generation where all search keys in R are also found in S
If a user would like to switch this, they simply need to change the call to "generateRelationR()" on line 576 to "generateRelationR2()". Additionally, lines 624-677 should be commented out and lines 679-729 should be uncommented in order to accomodate this input change.
The number of values for each of the relations can be changed at the top of the file in lines 14 and 15