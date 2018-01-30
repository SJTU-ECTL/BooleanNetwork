//
// Created by Patrick Yao on 30/01/2018.
//

#ifndef BOOLEANNETWORK_ECTL_INTERNALS_H
#define BOOLEANNETWORK_ECTL_INTERNALS_H

#include <cinttypes>
#include <functional>

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

namespace ECTL::Internals {

    /* IndirectArrayList defines an array list where the actual array
         * does not store the actual objects. Rather it stores the "handles"
         * or "unique identifiers" of the objects. For example, strings that
         * can be usedto look up objects in a map, or ids to lookup objects
         * in a object pool. */
    template <typename ArrayT, typename ValueT, typename SupplementT>
    class IndirectArrayList {
        const ArrayT*   base;   // Base address of the array
        int32_t         size;   // Maximum size of the array
        SupplementT*    extra;  // Extra information by user

        /* Extra can be used to store pointers to a memory pool, etc
         * The following function translates the array element to the
         * actual object */
        static std::function<ValueT&(const ArrayT*, SupplementT*)> locate;

    public:
        class Generator {
            const ArrayT* curr, end;
            SupplementT*  extra;
            friend class IndirectArrayList<ArrayT, ValueT, SupplementT>;
        protected:
            Generator(const ArrayT* begin, const ArrayT* end, SupplementT* extra);
        public:
            Generator();
            bool          hasEnded() const;
            ValueT&       next();
        };

        class ConstGenerator : private Generator {
            friend class IndirectArrayList<ArrayT, ValueT, SupplementT>;
        protected:
            ConstGenerator(const ArrayT* begin, const ArrayT* end, SupplementT* extra);
        public:
            ConstGenerator() : Generator() {}
            using Generator::hasEnded;
            const ValueT& next();
        };

        IndirectArrayList(const ArrayT* base, int32_t size,
                          SupplementT* extra);
        Generator      generator();
        ConstGenerator generator() const;
    };

    template<typename ArrayT, typename ValueT, typename SupplementT>
    class ConstIndirectArrayList : IndirectArrayList<ArrayT, ValueT, SupplementT> {
        using base_t = IndirectArrayList<ArrayT, ValueT, SupplementT>;
    public:
        using base_t::base_t;
        class ConstGenerator : public base_t::ConstGenerator {
        protected:
            ConstGenerator(const ArrayT* begin, const ArrayT* end, SupplementT* extra);
            ConstGenerator();
        };
        ConstGenerator generator() const;
    };
}


namespace ECTL::Internals {
    template<typename ArrayT, typename ValueT, typename SupplementT>
    IndirectArrayList<ArrayT, ValueT, SupplementT>::IndirectArrayList
            (const ArrayT *base, int32_t size, SupplementT *extra)
            : base(base), size(size), extra(extra) {}

    template<typename ArrayT, typename ValueT, typename SupplementT>
    typename IndirectArrayList<ArrayT, ValueT, SupplementT>::Generator
    IndirectArrayList<ArrayT, ValueT, SupplementT>::generator() {
        return IndirectArrayList::Generator(base, base + size, extra);
    }

    template<typename ArrayT, typename ValueT, typename SupplementT>
    typename IndirectArrayList<ArrayT, ValueT, SupplementT>::ConstGenerator
    IndirectArrayList<ArrayT, ValueT, SupplementT>::generator() const {
        return IndirectArrayList::ConstGenerator(base, base + size, extra);
    }

    template<typename ArrayT, typename ValueT, typename SupplementT>
    IndirectArrayList<ArrayT, ValueT, SupplementT>::Generator::Generator
            (const ArrayT *begin, const ArrayT *end, SupplementT *extra)
            : curr(begin), end(end), extra(extra) {}

    template<typename ArrayT, typename ValueT, typename SupplementT>
    IndirectArrayList<ArrayT, ValueT, SupplementT>::Generator::Generator()
            : curr(nullptr), end(nullptr), extra(nullptr) {}

    template<typename ArrayT, typename ValueT, typename SupplementT>
    bool IndirectArrayList<ArrayT, ValueT, SupplementT>::Generator::hasEnded() const {
        return curr == end;
    }

    template<typename ArrayT, typename ValueT, typename SupplementT>
    ValueT &IndirectArrayList<ArrayT, ValueT, SupplementT>::Generator::next() {
        if (hasEnded()) throw std::runtime_error("Generator reached end.");
        return IndirectArrayList<ArrayT, ValueT, SupplementT>::locate(
                curr++, extra
        );
    }
}

#include "ectl_internals.hpp"

#endif //BOOLEANNETWORK_ECTL_INTERNALS_H
