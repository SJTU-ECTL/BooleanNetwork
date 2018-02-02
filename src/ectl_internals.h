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
            typename ValueT,       // Type of the actual object
    > struct Locator {
        static ValueT      locate  (HandleT handle,       PoolT* pool);
    };

    template <
            typename HandleT,      // Type of the handle
            typename PoolT,        // Type of the pool
            typename ConstValueT,  // Type of the const object
    > struct ConstLocator {
        static ConstValueT locate  (HandleT handle, const PoolT* pool);
    };

    template <
            typename HandleT, typename PoolT, typename ConstValueT
    > class ConstGenerator {
        const HandleT  *curr;
        const HandleT  *end;
        const PoolT    *pool;
    public:
        ConstGenerator();
        ConstGenerator(const HandleT* curr, const HandleT* end, const PoolT* pool);
        bool            hasEnded() const;
        ConstValueT     next();
    };

    template <
            typename HandleT, typename PoolT, typename ValueT,
            typename ConstValueT=const std::remove_reference<ValueT>::type&,
            typename ConstGeneratorT=ConstGenerator<HandleT, PoolT, ConstValueT>
    > class Generator {
        const HandleT  *curr;
        const HandleT  *end;
              PoolT    *pool;
    public:
        Generator();
        Generator(const HandleT* curr, const HandleT* end, PoolT* pool);
        bool        hasEnded() const;
        ValueT      next();

        /* Conversion to Const-ed version */
        operator ConstGenerator() const;
    };

    template <
            typename HandleT, typename PoolT, typename ConstValueT,
            typename ConstGeneratorT=ConstGenerator<HandleT, PoolT, ConstValueT>
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
            typename ConstValueT=const std::remove_reference<ValueT>::type &,
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

namespace ECTL::Internals::IndirectArrayList {
    template<
            typename HandleT, typename PoolT, typename ValueT, typename ConstValueT
    > ValueT Locator<HandleT, PoolT, ValueT>::locate(HandleT handle, PoolT *pool) {
        throw std::runtime_error("Locator.locate() is not specialized.")
    }

    template<typename HandleT, typename PoolT, typename ValueT, typename ConstValueT>
    ConstValueT ConstLocator<HandleT, PoolT, ConstValueT>::locate(HandleT handle, const PoolT *pool) {
        throw std::runtime_error("ConstLocator.locate() is not specialized.")
    }

    template<
            typename HandleT, typename PoolT, typename ConstValueT
    > ConstGenerator<HandleT, PoolT, ConstValueT>::ConstGenerator()
            : ConstGenerator(nullptr, nullptr, nullptr){ }

    template<
            typename HandleT, typename PoolT, typename ConstValueT
    > ConstGenerator<HandleT, PoolT, ConstValueT>::ConstGenerator(
            const HandleT *curr, const HandleT *end, const PoolT *pool
    ) : curr(curr), end(end), pool(pool) { }

    template<
            typename HandleT, typename PoolT, typename ConstValueT
    > bool ConstGenerator<HandleT, PoolT, ConstValueT>::hasEnded() const {
        return curr == end;
    }

    template<
            typename HandleT, typename PoolT, typename ConstValueT
    > ConstValueT ConstGenerator<HandleT, PoolT, ConstValueT>::next() {
        return ConstLocator<HandleT, PoolT, ConstValueT>::locate(curr, pool);
    }

    template<
            typename HandleT, typename PoolT,
            typename ValueT, typename ConstValueT, typename ConstGeneratorT
    > Generator<HandleT, PoolT, ValueT, ConstValueT, ConstGeneratorT>::Generator()
            : Generator(nullptr, nullptr, nullptr) { };

    template<
            typename HandleT, typename PoolT,
            typename ValueT, typename ConstValueT, typename ConstGeneratorT
    > Generator<HandleT, PoolT, ValueT, ConstValueT, ConstGeneratorT>::Generator(
            const HandleT *curr, const HandleT *end, PoolT *pool
    ) : curr(curr), end(end), pool(pool) { };

    template<
            typename HandleT, typename PoolT,
            typename ValueT, typename ConstValueT, typename ConstGeneratorT
    > bool Generator<HandleT, PoolT, ValueT, ConstValueT, ConstGeneratorT>::hasEnded() const {
        return curr == end;
    }

    template<
            typename HandleT, typename PoolT, typename ValueT, typename ConstValueT, typename ConstGeneratorT
    > ValueT Generator<HandleT, PoolT, ValueT, ConstValueT, ConstGeneratorT>::next() {
        return Locator<HandleT, PoolT, ValueT>::locate(curr, pool);
    }

    template<
            typename HandleT, typename PoolT,
            typename ValueT, typename ConstValueT, typename ConstGeneratorT
    > Generator<HandleT, PoolT, ValueT, ConstValueT, ConstGeneratorT>::operator ConstGenerator() const {
        return ConstGenerator(curr, end, pool);
    }

    template<
            typename HandleT, typename PoolT,
            typename ConstValueT, typename ConstGeneratorT
    > ConstIndirectArrayList<HandleT, PoolT, ConstValueT, ConstGeneratorT>::ConstIndirectArrayList(
            const HandleT *base, int32_t size, const PoolT *pool
    ) : base(base), size(size), pool(pool) { }

    template<
            typename HandleT, typename PoolT,
            typename ConstValueT, typename ConstGeneratorT
    > ConstGeneratorT ConstIndirectArrayList<HandleT, PoolT, ConstValueT, ConstGeneratorT>::generator() const {
        return ConstGenerator(base, base + size, pool);
    }

    template<
            typename HandleT, typename PoolT,
            typename ValueT, typename ConstValueT,
            typename GeneratorT, typename ConstListT
    > IndirectArrayList<HandleT, PoolT, ValueT, ConstValueT, GeneratorT, ConstListT>::IndirectArrayList(
            const HandleT *base, int32_t size, PoolT *pool
    ) : base(base), size(size), pool(pool)  { }

    template<
            typename HandleT, typename PoolT,
            typename ValueT, typename ConstValueT,
            typename GeneratorT, typename ConstListT
    > GeneratorT IndirectArrayList<HandleT, PoolT, ValueT, ConstValueT, GeneratorT, ConstListT>::generator() const {
        return Generator(base, base + size, pool);
    }

    template<typename HandleT, typename PoolT, typename ValueT, typename ConstValueT, typename GeneratorT, typename ConstListT>
    IndirectArrayList<HandleT, PoolT, ValueT, ConstValueT, GeneratorT, ConstListT>::operator ConstListT() const {
        return ConstListT(base, size, pool);
    }
}

#include "ectl_internals.hpp"

#endif //BOOLEANNETWORK_ECTL_INTERNALS_H
