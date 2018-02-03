//
// Created by Patrick Yao on 30/01/2018.
//

#ifndef BOOLEANNETWORK_ECTL_INTERNALS_H
#define BOOLEANNETWORK_ECTL_INTERNALS_H

#include <cinttypes>
#include <functional>
#include <exception>

namespace ECTL::Internals {
    // Introducint the PImpl Patter
    // http://oliora.github.io/2015/12/29/pimpl-and-rule-of-zero.html
    // Nessecity of explitly declaring the dtor:
    // https://stackoverflow.com/questions/9020372/how-do-i-use-unique-ptr-for-pimpl
    struct _network;

    typedef union {
        int64_t iVal;
        double  dVal;
        void*   ptr;
        struct { void* p1;   void* p2; } ptrPair;
        struct { int32_t x;  int32_t y; } intPair;
        struct { void* base; int32_t id; int32_t max; } arrayRef;
    } smallBuffer;
}

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
            typename HandleT, typename PoolT, typename ConstValueT,
            typename ConstLocatorT=Locator<HandleT, const PoolT, ConstValueT>
    > class Generator {
        const HandleT  *curr;
        const HandleT  *end;
              PoolT    *pool;
    public:
        Generator();
        Generator(const HandleT* curr, const HandleT* end, PoolT* pool);
        bool            hasEnded() const;
        ConstValueT     next();
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

#include "ectl_internals.hpp"

#endif //BOOLEANNETWORK_ECTL_INTERNALS_H
