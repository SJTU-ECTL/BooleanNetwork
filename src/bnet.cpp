#include "bnet.h"

static FILE *open_file(const char *filename, const char *mode);

BooleanNetwork::BooleanNetwork(const std::string &file) {
    FILE *fp;
    fp = open_file(file.c_str(), "r");
    net = Bnet_ReadNetwork(fp, 0);
    fclose(fp);
}

BooleanNetwork::~BooleanNetwork() {
    Bnet_FreeNetwork(net);
}

/**
 * @brief print the BooleanNetwork(for testing purpose)
 */

void BooleanNetwork::printNetwork() const {
    Bnet_PrintNetwork(net);
}

/**
 * @brief get the linked list of nodes in BnetNetwork
 * @return the linked list of nodes in BnetNetwork
 */

BnetNode *BooleanNetwork::getNodesList() const {
    return net->nodes;
}

/**
 * @brief get the BnetNode in BnetNetwork by its name
 * @return the BnetNode
 */

BnetNode *BooleanNetwork::getNodebyName(const std::string name) const {
    BnetNode *node = nullptr;
    if (st_lookup(net->hash, name.c_str(), (void **) &node))
        return node;
    else
        return nullptr;
}

static FILE *open_file(const char *filename, const char *mode) {
    FILE *fp;
    if (strcmp(filename, "-") == 0) {
        return mode[0] == 'r' ? stdin : stdout;
    } else if ((fp = fopen(filename, mode)) == NULL) {
        perror(filename);
        exit(1);
    }
    return (fp);
}