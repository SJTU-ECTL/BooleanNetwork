#include "bnet.h"

const int SOURCE_ID = 0;
const int SINK_ID = 1;

static FILE *open_file(const char *filename, const char *mode);

static _BnetNode *_getNodebyName(const _BnetNetwork *net, BnetNodeID name);

BnetNode::BnetNode(const _BnetNode *node, bool add_source_sink, int info) {
    name_.assign(node->name);
    n_inp_ = node->ninp;
    n_out_ = node->nfo;
    for (int i = 0; i < n_inp_; ++i)
        fan_ins_.emplace_back(node->inputs[i]);
    for (int i = 0; i < n_out_; ++i)
        fan_outs_.emplace_back(node->outputs[i]);
    if (add_source_sink) {
        if (info == SOURCE_ID) {
            n_inp_++;
            fan_ins_.emplace_back(SOURCE_NAME);
        }
        if (info == SINK_ID) {
            n_out_++;
            fan_outs_.emplace_back(SINK_NAME);
        }
    }
    BnetTabline *f = node->f;
    while (f != nullptr) {
        truth_table_.emplace_back(f->values);
        f = f->next;
    }
    is_onset_ = node->polarity == 0;
    is_input_ = node->type == BNET_INPUT_NODE;
}

BnetNode::BnetNode(BnetNodeID name, std::vector<BnetNodeID> inputs,
                   std::vector<BnetNodeID> outputs) {
    n_inp_ = (int) inputs.size();
    n_out_ = (int) outputs.size();
    name_ = std::move(name);
    fan_ins_ = std::move(inputs);
    fan_outs_ = std::move(outputs);
}

BnetNode::~BnetNode() = default;

BnetNodeID BnetNode::getName() const {
    return name_;
}

const std::vector<BnetNodeID> &BnetNode::getFanIns() const {
    return fan_ins_;
}

const std::vector<BnetNodeID> &BnetNode::getFanOuts() const {
    return fan_outs_;
}

void BnetNode::setFanIns(std::vector<BnetNodeID> fan_ins) {
    fan_ins_ = std::move(fan_ins);
}

void BnetNode::setFanOuts(std::vector<BnetNodeID> fan_outs) {
    fan_outs_ = std::move(fan_outs);
}

BnetNetwork::BnetNetwork(const std::string &file, bool add_source_sink) {
    file_name_ = file;
    FILE *fp;
    fp = open_file(file.c_str(), "r");
    net_ = Bnet_ReadNetwork(fp, 0);
    fclose(fp);

    for (int i = 0; i < net_->npis; ++i)
        inputs_.emplace_back(net_->inputs[i]);

    for (int i = 0; i < net_->npos; ++i)
        outputs_.emplace_back(net_->outputs[i]);

    for (const _BnetNode *t = net_->nodes; t != nullptr; t = t->next) {
        int info;
        if (std::find(inputs_.begin(), inputs_.end(), std::string(t->name)) != inputs_.end())
            info = SOURCE_ID;
        else if (std::find(outputs_.begin(), outputs_.end(), std::string(t->name)) != outputs_.end())
            info = SINK_ID;
        else
            info = -1;
        auto node = new BnetNode(t, add_source_sink, info);
        nodes_.emplace_back(node);
        hash_table_.insert(std::pair<BnetNodeID, BnetNode *>(node->getName(), node));
    }

    if (add_source_sink) {
        auto source = new BnetNode(SOURCE_NAME, std::vector<BnetNodeID>(), inputs_);
        nodes_.emplace_back(source);
        hash_table_.insert(std::pair<BnetNodeID, BnetNode *>(source->getName(), source));

        auto sink = new BnetNode(SINK_NAME, outputs_, std::vector<BnetNodeID>());
        nodes_.emplace_back(sink);
        hash_table_.insert(std::pair<BnetNodeID, BnetNode *>(sink->getName(), sink));
    }
}

BnetNetwork::~BnetNetwork() {
    Bnet_FreeNetwork(net_);
    while (!nodes_.empty()) {
        delete nodes_.back();
        nodes_.pop_back();
    }
}

void BnetNetwork::printNetwork(std::string output_file, std::vector<BnetNodeID> deleted_nodes) const {
    freopen(output_file.c_str(), "w", stdout);
    Bnet_PrintNetwork(net_, std::move(deleted_nodes));
    fclose(stdout);
}

const std::vector<BnetNode *> &BnetNetwork::getNodesList() const {
    return nodes_;
}

const std::vector<BnetNodeID> &BnetNetwork::getInputNames() const {
    return inputs_;
}

const std::vector<BnetNodeID> &BnetNetwork::getOutputNames() const {
    return outputs_;
}

BnetNode *BnetNetwork::getNodebyName(BnetNodeID name) const {
    if (hash_table_.find(name) != hash_table_.end()) {
        return hash_table_.at(name);
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