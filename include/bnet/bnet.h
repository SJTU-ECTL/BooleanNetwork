#include "cudd_bnet.h"
#include <string>

typedef std::string BnetNodeID;

class BooleanNetwork {
private:
    BnetNetwork *net;
public:
    BooleanNetwork(const std::string &file);

    ~BooleanNetwork();

    void printNetwork() const;

    BnetNode *getNodesList() const;

    BnetNode *getNodebyName(const std::string name) const;
};