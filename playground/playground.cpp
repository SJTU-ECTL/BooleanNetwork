#include <iostream>
#include "bnet.h"
#include "sta.h"

int main() {
    BnetNetwork *net;
    std::string blif_file_path(PROJECT_SOURCE_DIR);
    blif_file_path += "/playground/benchmark/C17.blif";
    net = new BnetNetwork(blif_file_path, true);
//    net->printNetwork();

    std::vector<BnetNodeID> sorted_nodes = TopologicalSort(net);
    std::cout << "Topological Order of Nodes: ";
    for (auto &i : sorted_nodes)
        std::cout << i << ' ';
    std::cout << std::endl;

    std::map<BnetNodeID, int> slack = StaticTimingAnalysis(net);
    for (const auto &node:sorted_nodes)
        std::cout << node << "=" << slack.at(node) << "   ";
    std::cout << std::endl;

    WorstPathReporting(net, slack, 10);

    delete (net);
    return 0;
}
