//
// Created by Patrick Yao on 29/01/2018.
//

#ifndef BOOLEANNETWORK_NETLIST_H
#define BOOLEANNETWORK_NETLIST_H

#include <cinttypes>
#include <string>
#include <memory>
#include <functional>

#include "ectl_internals.h"

namespace ECTL {

    // Class interface of a Boolean Network shown in Netlist
    // Implements the idea of a Netlist networks
    // Other possible Networks are "AIG" networks, etc.
    class NetlistNetwork;

    // Class interface of a "node" list
    // Conceptually similar to 'generators' in python
    // Self-contained interator to interator through various node sets / lists
    class NodeGnerator;

    // An Attribute Manager managers an "attribute"
    // An attribute can be a "Node Attribute" or a "Network Attribute", or a
    // mix of both. A node attribute is an attribute that belongs to a speci
    // fic node. A network attribute is a property of a the entire network.
    // e.g.    'Arrival time' is a node attribute
    //         'delay'        is a network attribute
    class AttributeManager;

    // Introducing "Type Erasure" - Runtime value-typed polymorphism
    // Implemented with library 'adobe.poly', part of Adobe's ASL
    // Usage and rational see
    // https://akrzemi1.wordpress.com/2013/12/06/type-erasure-part-ii/
    // Based on research work of
    // https://akrzemi1.files.wordpress.com/2013/10/mpool2007-marcus.pdf

    typedef std::string NodeID;


    class Netlist {
        std::unique_ptr<Internals::_network> pimpl;

        /* PUBLIC TYPES */
    public:
        class Node;
        class NodeSet;
        class ConstNodeSet;

        class Node {
            friend class Nelist;
            Internals::smallBuffer impl;
        public:
            Node() = default;
            std::string  name() const;
            NodeSet      fanins();           // Get the fanin  set of a node
            NodeSet      fanouts();          // Get the fanout set of a node
            ConstNodeSet fanins() const;
            ConstNodeSet fanouts() const;
        };

        class ConstGenerator {
            bool  hasEnded()      const;
            const Node& current() const;
            const Node& next();
        };
        class Generator {
            bool  hasEnded() const;
            Node& current()  const;
            Node& next   ();
            operator ConstGenerator() const;
        };

        class NodeSet {
            friend class Netlist;
            Internals::smallBuffer impl;
        public:
            NodeSet() = default;
            Generator       generator();
            ConstGenerator  generator() const;
            int32_t         size()      const;      // Get number of nodes in the set
            operator ConstNodeSet()     const;
        };


        /* PUBLIC FUNCTIONS */
    public:
        // CTors and loaders
        Netlist();                                      // Default Ctor
        void loadFromBlif(std::string file);            // Load from a blif, path given by $file;

        // The rule of big 5
        Netlist(const Netlist& other);                  // Copy Ctor
        Netlist(Netlist&& other) noexcept;              // Move Ctor
        Netlist& operator=(const Netlist& other);       // Copy assignment
        Netlist& operator=(Netlist&& other);            // Move assignment
        ~Netlist();                                     // Dtor

        ConstNodeSet Inputs()             const;
        ConstNodeSet Outputs()            const;
        ConstNodeSet Nodes()              const;
        int32_t numOfNodes()              const;
        int32_t numOfInputs()             const;
        int32_t numOfOutputs()            const;
    };
};

#endif //BOOLEANNETWORK_NETLIST_H
