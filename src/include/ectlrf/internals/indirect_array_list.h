//
// Created by Patrick Yao on 06/02/2018.
//

#ifndef BOOLEANNETWORK_INDIRECT_ARRAY_LIST_H
#define BOOLEANNETWORK_INDIRECT_ARRAY_LIST_H


namespace ECTL::Internals::IndirectArrayList {

    /* IndirectArrayList defines an array list where the actual array
     * does not store the actual objects. Rather it stores the "handles"
     * or "unique identifiers" of the objects. For example, strings that
     * can be used to look up objects in a map, or ids to lookup objects
     * in a object pool. */

    template <
            typename HandleT,      // Type of the handle
            typename PoolT,        // Type of the pool
            typename ValueT        // Type of the object
    > struct Locator {
        static ValueT      locate  (HandleT handle,       PoolT* pool);
    };

    template <
            typename HandleT, typename PoolT, typename ValueT,
            typename LocatorT=Locator<HandleT, const PoolT, ValueT>
    > class Generator {
        const HandleT  *current;
        const HandleT  *end;
        PoolT          *pool;
    public:
        Generator();
        Generator(const HandleT* curr, const HandleT* end, PoolT* pool);
        bool            hasEnded() const;
        ValueT          next();
        void            skip();
        ValueT          curr();
    };

    template <
            typename HandleT, typename PoolT, typename ConstValueT,
            typename ConstGeneratorT=Generator<HandleT, const PoolT, ConstValueT>
    > class ConstIndirectArrayList {
        const HandleT*   base;   // Base address of the array
        int32_t          size;   // Maximum size of the array
        const PoolT*     pool;   // Location of the pool
    public:
        ConstIndirectArrayList(const HandleT* base, int32_t size, const PoolT* pool);
        ConstGeneratorT generator() const;
    };

    template <
            typename HandleT, typename PoolT, typename ValueT,
            typename ConstValueT=const typename std::remove_reference<ValueT>::type &,
            typename GeneratorT=Generator<HandleT, PoolT, ValueT, ConstValueT>,
            typename ConstListT=ConstIndirectArrayList<HandleT, PoolT, ConstValueT>
    > class IndirectArrayList {
        const HandleT*   base;   // Base address of the array
        int32_t          size;   // Maximum size of the array
        PoolT*           pool;  // The pool
    public:
        IndirectArrayList(const HandleT* base, int32_t size, PoolT* pool);
        GeneratorT generator() const;

        // Conversion to Const-ed reference set
        operator ConstListT() const;
    };
}

#include "indirect_array_list.hpp"

#endif //BOOLEANNETWORK_INDIRECT_ARRAY_LIST_H
