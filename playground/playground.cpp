#include <iostream>
#include "bnet.h"
#include "sta.h"

int main() {
    BnetNetwork *net;
    std::string blif_file_path(PROJECT_SOURCE_DIR);
    blif_file_path += "/playground/benchmark/C880.blif";
    net = new BnetNetwork(blif_file_path, true);

    KMostCriticalPaths(net, 100, true);

    delete (net);
    return 0;
}
