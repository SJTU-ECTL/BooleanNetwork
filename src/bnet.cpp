#include "bnet.h"

static FILE *open_file(const char *filename, const char *mode);

static _BnetNode *_getNodebyName(const _BnetNetwork *net, BnetNodeID name);


BnetNode::BnetNode(const _BnetNode *node, bool add_source_sink) {
    name.assign(node->name);
    ninp = node->ninp;
    nout = node->nfo;
    for (int i = 0; i < ninp; ++i)
        inputs.emplace_back(node->inputs[i]);
    for (int i = 0; i < nout; ++i)
        outputs.emplace_back(node->outputs[i]);
    if(add_source_sink) {
        if(ninp==0) {
            ninp++;
            inputs.emplace_back(SOURCE_NAME);
        }
        if(nout==0) {
            nout++;
            outputs.emplace_back(SINK_NAME);
        }
    }
}

BnetNode::BnetNode(BnetNodeID name, std::vector<BnetNodeID> inputs,
                   std::vector<BnetNodeID> outputs) {
    this->ninp = (int) inputs.size();
    this->nout = (int) outputs.size();
    this->name = std::move(name);
    this->inputs = std::move(inputs);
    this->outputs = std::move(outputs);
}

BnetNode::~BnetNode() = default;

BnetNodeID BnetNode::getName() const {
    return name;
}

const std::vector<BnetNodeID> &BnetNode::getInputs() const {
    return inputs;
}

const std::vector<BnetNodeID> &BnetNode::getOutputs() const {
    return outputs;
}

BnetNetwork::BnetNetwork(const std::string &file, bool add_source_sink) {
    file_name = file;
    FILE *fp;
    fp = open_file(file.c_str(), "r");
    net = Bnet_ReadNetwork(fp, 0);
    fclose(fp);

    for (int i = 0; i < net->npis; ++i)
        inputs.emplace_back(net->inputs[i]);

    for (int i = 0; i < net->npos; ++i)
        outputs.emplace_back(net->outputs[i]);

    for (const _BnetNode *t = net->nodes; t != nullptr; t = t->next) {
        auto node = new BnetNode(t, add_source_sink);
        nodes.emplace_back(node);
        hash_table.insert(std::pair<BnetNodeID, BnetNode *>(node->getName(), node));
    }

    if (add_source_sink) {
        auto source = new BnetNode(SOURCE_NAME, std::vector<BnetNodeID>(), inputs);
        nodes.emplace_back(source);
        hash_table.insert(std::pair<BnetNodeID, BnetNode *>(source->getName(), source));

        auto sink = new BnetNode(SINK_NAME, outputs, std::vector<BnetNodeID>());
        nodes.emplace_back(sink);
        hash_table.insert(std::pair<BnetNodeID, BnetNode *>(sink->getName(), sink));
    }
}

BnetNetwork::~BnetNetwork() {
    Bnet_FreeNetwork(net);
    while (!nodes.empty()) {
        delete nodes.back();
        nodes.pop_back();
    }
}

/**
 * @brief print the BooleanNetwork(for testing purpose)
 */

void BnetNetwork::printNetwork() const {
    Bnet_PrintNetwork(net);
}

/**
 * @brief get the linked list of nodes in BnetNetwork
 * @return the linked list of nodes in BnetNetwork
 */
const std::vector<BnetNode *> &BnetNetwork::getNodesList() const {
    return nodes;
}

const std::vector<BnetNodeID> &BnetNetwork::getInputNames() const {
    return inputs;
}

const std::vector<BnetNodeID> &BnetNetwork::getOutputNames() const {
    return outputs;
}

/**
 * @brief get the BnetNode in BnetNetwork by its name
 * @return the BnetNode
 */

BnetNode *BnetNetwork::getNodebyName(BnetNodeID name) const {
    if (hash_table.find(name) != hash_table.end()) {
        return hash_table.at(name);
    } else
        return nullptr;
}

static _BnetNode *_getNodebyName(const _BnetNetwork *net, BnetNodeID name) {
    _BnetNode *node = nullptr;
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