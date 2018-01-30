#ifndef BOOLEANNETWORK_BNET_H
#define BOOLEANNETWORK_BNET_H

#include "cudd_bnet.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <cinttypes>

typedef std::string BnetNodeID;

const char SOURCE_NAME[] = "SOURCE";
const char SINK_NAME[] = "SINK";

class BnetNode {
private:
    BnetNodeID name_;
    int n_inp_;
    int n_out_;
    std::vector<BnetNodeID> fan_ins_;
    std::vector<BnetNodeID> fan_outs_;
    std::vector<BnetNodeID> truth_table_;
    bool is_onset_;
    bool is_input_;

public:
    explicit BnetNode(const _BnetNode *node, bool add_source_sink = false, int info = -1);

    BnetNode(BnetNodeID name, std::vector<BnetNodeID> inputs,
             std::vector<BnetNodeID> outputs);

    ~BnetNode();

    BnetNodeID getName() const;

    const std::vector<BnetNodeID> &getFanIns() const;

    const std::vector<BnetNodeID> &getFanOuts() const;

    void setFanIns(std::vector<BnetNodeID> fan_ins);

    void setFanOuts(std::vector<BnetNodeID> fan_outs);
};

class BnetNetwork {
private:
    _BnetNetwork *net_;
    std::string file_name_;

    std::vector<BnetNodeID> inputs_;
    std::vector<BnetNodeID> outputs_;
    std::vector<BnetNode *> nodes_;
    std::map<BnetNodeID, BnetNode *> hash_table_;

public:
    explicit BnetNetwork(const std::string &file, bool add_source_sink = false);

    ~BnetNetwork();

    const std::vector<BnetNode *> &getNodesList() const;

    const std::vector<BnetNodeID> &getInputNames() const;

    const std::vector<BnetNodeID> &getOutputNames() const;

    BnetNode *getNodebyName(BnetNodeID name) const;

    void printNetwork(std::string output_file,
                      std::vector<BnetNodeID> const_0_nodes = std::vector<BnetNodeID>(),
                      std::vector<BnetNodeID> const_1_nodes = std::vector<BnetNodeID>()) const;
};

#endif