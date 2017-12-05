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

void BooleanNetwork::PrintNetwork() {
    Bnet_PrintNetwork(net);
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