//
// Created by Patrick Yao on 29/01/2018.
//
#include "cudd_bnet.h"
#include "netlist.h"
#include <exception>

using namespace ECTL;
using namespace ECTL::Internals;
using namespace std;

static FILE *open_file(const char *filename,
                       const char *mode);
static _BnetNode *_getNodebyName(const _BnetNetwork *net,
                                 const char* name);

struct ECTL::Internals::_network {
    _BnetNetwork *net ;
};

Netlist::Netlist() : pimpl(new _network) {
    pimpl->net = nullptr;
}

void Netlist::loadFromBlif(std::string file) {
    FILE *fp;
    fp = open_file(file.c_str(), "r");
    pimpl->net = Bnet_ReadNetwork(fp, 0);
    fclose(fp);
}

Netlist::Netlist(const Netlist &other) : pimpl(nullptr) {
    /* Clone the network from $other
     * The mechanism is not implemented yet. */
    _network* clone = nullptr;
    throw std::runtime_error("Not implemented");
    this->pimpl.reset(clone);
}

Netlist::Netlist(Netlist &&other) noexcept = default;

Netlist &Netlist::operator=(const Netlist &other) {
    /* Clone the network from $other
    * The mechanism is not implemented yet. */
    _network* clone = nullptr;
    throw std::runtime_error("Not implemented");
    this->pimpl.reset(clone);
    return *this;
}

Netlist &Netlist::operator=(Netlist &&other) = default;

Netlist::~Netlist(){
    Bnet_FreeNetwork(pimpl->net);
}

Netlist::ConstNodeSet Netlist::Inputs() const {
    Netlist::ConstNodeSet set{};
    set.impl.arrayRef.base = pimpl->net->inputs;
    set.impl.arrayRef.max  = pimpl->net->ninputs;
    set.impl.arrayRef.id   = 0;
    return set;
}

Netlist::ConstNodeSet Netlist::Outputs() const {
    Netlist::ConstNodeSet set{};
    set.impl.arrayRef.base = pimpl->net->outputs;
    set.impl.arrayRef.max  = pimpl->net->noutputs;
    set.impl.arrayRef.id   = 0;
    return set;
}

int32_t Netlist::numOfNodes() const {
    throw std::runtime_error("Not Implemented");
    return -1;
}

int32_t Netlist::numOfInputs() const {
    return pimpl->net->noutputs;
}

int32_t Netlist::numOfOutputs() const {
    return pimpl->net->ninputs;
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

Netlist::ConstGenerator Netlist::ConstNodeSet::generator() const {
    return Netlist::ConstGenerator();
}

int32_t Netlist::ConstNodeSet::size() const {
    return impl.arrayRef.max;
}

std::string Netlist::Node::name() const {
    _BnetNode* node = static_cast<_BnetNode*>(impl.ptr);
    return string(node->name);
}

Netlist::NodeSet Netlist::Node::fanins() {
    static_assert(sizeof(ConstNodeSet) == sizeof(NodeSet));
    auto set = static_cast<const Netlist::Node*>(this)->fanins();
    void *tmp = static_cast<void*>(&set);
    return *static_cast<NodeSet*>(tmp);
}

Netlist::NodeSet Netlist::Node::fanouts() {
    static_assert(sizeof(ConstNodeSet) == sizeof(NodeSet));
    auto set = static_cast<const Netlist::Node*>(this)->fanouts();
    void *tmp = static_cast<void*>(&set);
    return *static_cast<NodeSet*>(tmp);
}

Netlist::ConstNodeSet Netlist::Node::fanins() const {
    auto name = static_cast<const char*>(impl.ptrPair.p1);
    auto net = static_cast<_BnetNetwork*>(impl.ptrPair.p2);
    _BnetNode* node = _getNodebyName(net, name);
    Netlist::ConstNodeSet set{};
    set.impl.arrayRef.base = node->inputs;
    set.impl.arrayRef.max  = node->ninp;
    set.impl.arrayRef.id   = 0;
    return set;
}

Netlist::ConstNodeSet Netlist::Node::fanouts() const {
    auto name = static_cast<const char*>(impl.ptrPair.p1);
    auto net = static_cast<_BnetNetwork*>(impl.ptrPair.p2);
    _BnetNode* node = _getNodebyName(net, name);
    Netlist::ConstNodeSet set{};
    set.impl.arrayRef.base = node->outputs;
    set.impl.arrayRef.max  = node->nfo;
    set.impl.arrayRef.id   = 0;
    return set;
}

static _BnetNode *_getNodebyName(const _BnetNetwork *net,
                                 const char* name) {
    _BnetNode *node = nullptr;
    if (st_lookup(net->hash, name, (void **) &node)) return node;
    throw std::runtime_error("Node name doesn't exist");
}

