#include "bnet.h"

int main() {
    BooleanNetwork *net;
    std::string blif_file_path (PROJECT_SOURCE_DIR);
    blif_file_path += "/playground/benchmark/alu4.blif";
    net = new BooleanNetwork(blif_file_path);
    net->PrintNetwork();
    delete (net);
    return 0;
}