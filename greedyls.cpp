//C++ implementation of Jowhari's greedy local search algorithm
#include "LSTree.cpp"
#include <array>
int main(int argc, char* argv[]) {
    std::array<int, 4> arr = {4, 7, 8, 10};
    std::cout << arr.size() << std::endl;
    for(int i = 0; i < arr.size(); i++) {
        std::vector<int> leaves;
        leaves.resize(arr[i]);
        for(int j = 0; j < leaves.size(); j++) {
            leaves[j] = j;
        }
        LSTree(leaves.size(), leaves);
    }
    return 0;
}