#include "cudd_bnet.h"
#include <string>
#include <vector>
#include <map>

typedef std::string BnetNodeID;

class BnetNode {
private:
    BnetNodeID name;
    int ninp;
    int nout;
    std::vector<BnetNodeID> inputs;
    std::vector<BnetNodeID> outputs;

public:
    explicit BnetNode(const _BnetNode *node);

    ~BnetNode();

    BnetNodeID getName() const;

    const std::vector<BnetNodeID> &getInputs() const;

    const std::vector<BnetNodeID> &getOutputs() const;
};

class BnetNetwork {
private:
    _BnetNetwork *net;

    std::vector<BnetNodeID> inputs;
    std::vector<BnetNodeID> outputs;
    std::vector<BnetNode *> nodes;
    std::map<BnetNodeID, BnetNode *> hashTable;

public:
    explicit BnetNetwork(const std::string &file);

    ~BnetNetwork();

    const std::vector<BnetNode *> &getNodesList() const;

    BnetNode *getNodebyName(BnetNodeID name) const;

    void printNetwork() const;
};