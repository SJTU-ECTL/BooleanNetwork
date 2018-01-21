#include <iostream>
#include "bnet.h"
#include "sta.h"

int main() {
    BnetNetwork *net;
    std::string blif_file_path(PROJECT_SOURCE_DIR);
    blif_file_path += "/playground/benchmark/C17.blif";
    net = new BnetNetwork(blif_file_path, true);

    //KMostCriticalPaths(net, 10, true);

    std::vector<BnetNodeID> del_nodes;
    del_nodes.emplace_back("11GAT(5)");
    del_nodes.emplace_back("23GAT(9)");

    std::string del_blif_file_path(PROJECT_SOURCE_DIR);
    del_blif_file_path += "/playground/C17_del.blif";
    net->printNetwork(del_blif_file_path, del_nodes);

    delete (net);
    return 0;
}
