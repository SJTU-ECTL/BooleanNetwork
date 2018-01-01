#ifndef BOOLEANNETWORK_BNET_H
#define BOOLEANNETWORK_BNET_H

#include "cudd_bnet.h"
#include <string>
#include <vector>
#include <map>

typedef std::string BnetNodeID;

const BnetNodeID SOURCE_NAME = "SOURCE";
const BnetNodeID SINK_NAME = "SINK";

class BnetNode {
private:
    BnetNodeID name;
    int ninp;
    int nout;
    std::vector<BnetNodeID> inputs;
    std::vector<BnetNodeID> outputs;

public:
    explicit BnetNode(const _BnetNode *node, bool add_source_sink = false);

    BnetNode(BnetNodeID name, std::vector<BnetNodeID> inputs,
             std::vector<BnetNodeID> outputs);

    ~BnetNode();

    BnetNodeID getName() const;

    const std::vector<BnetNodeID> &getInputs() const;

    const std::vector<BnetNodeID> &getOutputs() const;
};

class BnetNetwork {
private:
    _BnetNetwork *net;
    std::string file_name;

    std::vector<BnetNodeID> inputs;
    std::vector<BnetNodeID> outputs;
    std::vector<BnetNode *> nodes;
    std::map<BnetNodeID, BnetNode *> hash_table;

public:
    explicit BnetNetwork(const std::string &file, bool add_source_sink = false);

    ~BnetNetwork();

    const std::vector<BnetNode *> &getNodesList() const;

    const std::vector<BnetNodeID> &getInputNames() const;

    const std::vector<BnetNodeID> &getOutputNames() const;

    BnetNode *getNodebyName(BnetNodeID name) const;

    void printNetwork() const;
};

#endif