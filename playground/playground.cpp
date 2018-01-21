#include <iostream>
#include "bnet.h"
#include "sta.h"

int main() {
    BnetNetwork *net;
    std::string blif_file_path(PROJECT_SOURCE_DIR);
    blif_file_path += "/playground/benchmark/C17.blif";

    std::string del_blif_file_path(PROJECT_SOURCE_DIR);
    del_blif_file_path += "/playground/C17_del.blif";

    net = new BnetNetwork(blif_file_path, true);

    //KMostCriticalPaths(net, 10, true);

    std::vector<BnetNodeID> const_0_nodes;
    const_0_nodes.emplace_back("11GAT(5)");

    std::vector<BnetNodeID> const_1_nodes;
    const_1_nodes.emplace_back("23GAT(9)");

    net->printNetwork(del_blif_file_path, const_0_nodes, const_1_nodes);

    delete (net);
    return 0;
}
