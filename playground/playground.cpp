#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include "bnet.h"

std::vector<BnetNodeID> topologicalSort(BooleanNetwork *net);

int main() {
    BooleanNetwork *net;
    std::string blif_file_path(PROJECT_SOURCE_DIR);
    blif_file_path += "/playground/benchmark/C17.blif";
    net = new BooleanNetwork(blif_file_path);

    std::vector<BnetNodeID> sortedNodes;
    sortedNodes = topologicalSort(net);

    std::cout << "Topological Order of Nodes: ";
    for (auto &i : sortedNodes) {
        std::cout << i << ' ';
    }

    delete (net);
    return 0;
}

std::vector<BnetNodeID> topologicalSort(BooleanNetwork *net) {
    std::vector<BnetNodeID> sortedNodes;
    std::deque<BnetNodeID> nodesQueue;
    std::map<BnetNodeID, int> inDegrees;

    for (BnetNode *t = net->getNodesList(); t != nullptr; t = t->next) {
        inDegrees[t->name] = t->ninp;
        if (t->ninp == 0)
            nodesQueue.emplace_back(std::string(t->name));
    }

    while (!nodesQueue.empty()) {
        BnetNodeID id = nodesQueue.front();
        nodesQueue.pop_front();

        BnetNode *t = net->getNodebyName(id);
        sortedNodes.emplace_back(t->name);
        for (int i = 0; i < t->nfo; i++) {
            BnetNodeID outnode = t->outputs[i];
            inDegrees[outnode] -= 1;
            if (inDegrees[outnode] == 0)
                nodesQueue.push_back(outnode);
        }
    }

    return sortedNodes;
}