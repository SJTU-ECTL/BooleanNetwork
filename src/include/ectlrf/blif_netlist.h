//
// Created by Patrick Yao on 06/02/2018.
//

#ifndef BOOLEANNETWORK_BLIF_NETLIST_H
#define BOOLEANNETWORK_BLIF_NETLIST_H

#include "internals/common.h"
#include "internals/blif_netlist.h"
#include "internals/indirect_array_list.h"

#include <string>

namespace ECTL {

    /* The BlifNetlist representation is a READ ONLY representation
     * To modify the network, this representation has to be transformed
     * into library internal representations. */

    class BlifNetlist {
        std::unique_ptr<Internals::BlifNetlist::_network> pimpl;

        /* PRIVATE TYPES */
    private:
        typedef Internals::BlifNetlist::_network ImplType;
        typedef Internals::smallBuffer           smallBuffer;
        struct  Locator;         // Look up function, for book keeping

        /* PUBLIC TYPES */
    public:
        class ConstNodeRef;             // Const-referential type
        class ConstIOGenerator;         // Generator<Const NodeRef>
        class ConstNodeRefArraySet;     // Const-referential Container


        /* PUBLIC FUNCTIONS */
    public:
        // CTors and loaders
        BlifNetlist(); // Default Ctor
        void loadFromBlif(std::string file);            // Load from a blif, path given by $file;

        // The rule of big 5
        BlifNetlist(const BlifNetlist& other);                  // Copy Ctor
        BlifNetlist(BlifNetlist&& other) noexcept;              // Move Ctor
        BlifNetlist& operator=(const BlifNetlist& other);       // Copy assignment
        BlifNetlist& operator=(BlifNetlist&& other) noexcept;   // Move assignment
        ~BlifNetlist();                                         // Dtor

        ConstNodeRefArraySet Inputs()             const;
        ConstNodeRefArraySet Outputs()            const;
        ConstNodeRefArraySet Nodes()              const;
        int32_t numOfNodes()                      const;
        int32_t numOfInputs()                     const;
        int32_t numOfOutputs()                    const;
    };

    struct BlifNetlist::Locator {
        using ConstNodeRef = BlifNetlist::ConstNodeRef;
        using smallBuffer  = Internals::smallBuffer;
        using network      = ImplType;
        static ConstNodeRef locate(void* , const network*);
    };

    class BlifNetlist::ConstNodeRef {
        friend class BlifNetlist::Locator;
        Internals::smallBuffer impl;
        ConstNodeRef(void* node, const ImplType* pool);
    public:
        ConstNodeRef() = default;
        std::string              name()     const;
        ConstNodeRefArraySet     fanins()   const;
        ConstNodeRefArraySet     fanouts()  const;
    };

    class BlifNetlist::ConstIOGenerator
            : Internals::IndirectArrayList::Generator<
                    void*, const ImplType, ConstNodeRef, Locator
            > {
    public:
        using base_t = Internals::IndirectArrayList::Generator<
                void*, const ImplType, ConstNodeRef, Locator
        >;
        using base_t::Generator;
        using base_t::hasEnded;
        using base_t::next;
        using base_t::curr;
        using base_t::skip;
    };

    class BlifNetlist::ConstNodeRefArraySet
        : Internals::IndirectArrayList::ConstIndirectArrayList <
                void*, ImplType, ConstNodeRef, ConstIOGenerator
        > {
    public:
        using base_t = Internals::IndirectArrayList::ConstIndirectArrayList <
                void*, ImplType, ConstNodeRef, ConstIOGenerator
        >;
        using base_t::ConstIndirectArrayList;
        using base_t::generator;
        using base_t::size;
    };

}



#endif //BOOLEANNETWORK_BLIF_NETLIST_H
