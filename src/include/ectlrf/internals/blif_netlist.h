//
// Created by Patrick Yao on 06/02/2018.
//

#ifndef BOOLEANNETWORK_BLIF_NETLIST_H
#define BOOLEANNETWORK_BLIF_NETLIST_H

#include "common.h"

namespace ECTL::Internals::BlifNetlist {
    // Introducint the PImpl Patter
    // http://oliora.github.io/2015/12/29/pimpl-and-rule-of-zero.html
    // Nessecity of explitly declaring the dtor:
    // https://stackoverflow.com/questions/9020372/how-do-i-use-unique-ptr-for-pimpl
    struct _network;
}

#endif //BOOLEANNETWORK_BLIF_NETLIST_H
