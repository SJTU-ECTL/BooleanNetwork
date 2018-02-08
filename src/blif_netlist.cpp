//
// Created by Patrick Yao on 06/02/2018.
//

#include <ectlrf/blif_netlist.h>
#include <cudd_blif/cudd_bnet.h>
#include <cstdio>
#include <ectlrf/internals/common.h>

using namespace ECTL;
using namespace ECTL::Internals::BlifNetlist;
using namespace std;

static FILE *open_file(const char *filename, const char *mode);

static _BnetNode *_getNodebyName(const _BnetNetwork *net, const char* name);

struct ECTL::Internals::BlifNetlist::_network {
    _BnetNetwork *net;
};

BlifNetlist::BlifNetlist() : pimpl(new _network) {
    pimpl->net = nullptr;
}

BlifNetlist::BlifNetlist(const BlifNetlist &other) : pimpl(nullptr) {
    /* Clone the network from $other
     * The mechanism is not implemented yet. */
    _network* clone = nullptr;
    throw std::runtime_error("Not implemented");
    this->pimpl.reset(clone);
}

BlifNetlist::BlifNetlist(BlifNetlist &&other) noexcept = default;

BlifNetlist &BlifNetlist::operator=(const BlifNetlist &other) {
    /* Clone the network from $other
    * The mechanism is not implemented yet. */
    _network* clone = nullptr;
    throw std::runtime_error("Not implemented");
    this->pimpl.reset(clone);
    return *this;
}

BlifNetlist &BlifNetlist::operator=(BlifNetlist &&other) noexcept = default;

BlifNetlist::~BlifNetlist(){
    Bnet_FreeNetwork(pimpl->net);
}

BlifNetlist::ConstNodeRef
BlifNetlist::Locator::locate(void* buffer, const ImplType* impl) {
    auto nodeName = static_cast<const char*>(buffer);
    auto net = impl->net;
    auto nodePtr = _getNodebyName(net, nodeName);
    return ConstNodeRef(nodePtr, impl);
}

BlifNetlist::ConstNodeRef::ConstNodeRef(void *node, const ImplType* pool) {
    this->impl.ptrPair.p1 = node;
    this->impl.ptrPair.p2 = const_cast<void*>(static_cast<const void*>(pool));
}

std::string BlifNetlist::ConstNodeRef::name() const {
    auto node = static_cast<const _BnetNode*>(impl.ptrPair.p1);
    return node->name;
}

BlifNetlist::ConstNodeRefArraySet
BlifNetlist::ConstNodeRef::fanins() const {
    auto node = static_cast<const _BnetNode*>(impl.ptrPair.p1);
    auto net  = static_cast<const ImplType*>(impl.ptrPair.p2);
    return BlifNetlist::ConstNodeRefArraySet(
            (void* const*)node->inputs, node->ninp, net
    );
}

BlifNetlist::ConstNodeRefArraySet
BlifNetlist::ConstNodeRef::fanouts() const {
    auto node = static_cast<const _BnetNode*>(impl.ptrPair.p1);
    auto net  = static_cast<const ImplType*>(impl.ptrPair.p2);
    return BlifNetlist::ConstNodeRefArraySet(
            (void* const*)node->outputs, node->nfo, net
    );
}

BlifNetlist::ConstNodeRefArraySet
BlifNetlist::Inputs() const {
    auto net = pimpl->net;
    return BlifNetlist::ConstNodeRefArraySet(
            (void* const*)net->inputs, net->npis, pimpl.get()
    );
}

BlifNetlist::ConstNodeRefArraySet BlifNetlist::Outputs() const {
    auto net = pimpl->net;
    return BlifNetlist::ConstNodeRefArraySet(
            (void* const*)net->outputs, net->npos, pimpl.get()
    );
}


void BlifNetlist::loadFromBlif(std::string file) {
    FILE *fp;
    fp = open_file(file.c_str(), "r");
    pimpl->net = Bnet_ReadNetwork(fp, 0);
    fclose(fp);
}

FILE *open_file(const char *filename, const char *mode) {
    FILE *fp;
    if (strcmp(filename, "-") == 0) {
        return mode[0] == 'r' ? stdin : stdout;
    } else if ((fp = fopen(filename, mode)) == NULL) {
        perror(filename);
        exit(1);
    }
    return (fp);
}

static _BnetNode *_getNodebyName(const _BnetNetwork *net,
                                 const char* name) {
    _BnetNode *node = nullptr;
    if (st_lookup(net->hash, name, (void **) &node)) return node;
    throw std::runtime_error("Node name doesn't exist");
}