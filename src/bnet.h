#include "cudd_bnet.h"
#include <string>

class BooleanNetwork {
private:
    BnetNetwork *net;
public:
    BooleanNetwork(const std::string &file);

    ~BooleanNetwork();

    void PrintNetwork();
};