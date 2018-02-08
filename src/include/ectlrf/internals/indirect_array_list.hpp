//
// Created by Patrick Yao on 30/01/2018.
//

namespace ECTL::Internals::IndirectArrayList {
    template<
            typename HandleT, typename PoolT, typename ValueT
    > ValueT Locator<HandleT, PoolT, ValueT>::locate(HandleT handle, PoolT *pool) {
        throw std::runtime_error("Locator.locate() is not specialized.");
    }

    template<
            typename HandleT, typename PoolT,
            typename ConstValueT, typename ConstLocatorT
    > Generator<HandleT, PoolT, ConstValueT, ConstLocatorT>::Generator()
            : Generator(nullptr, nullptr, nullptr){ }

    template<
            typename HandleT, typename PoolT,
            typename ConstValueT, typename ConstLocatorT
    > Generator<HandleT, PoolT, ConstValueT, ConstLocatorT>::Generator(
            const HandleT *curr, const HandleT *end, PoolT *pool
    ) : current(curr), end(end), pool(pool) { }

    template<
            typename HandleT, typename PoolT,
            typename ConstValueT, typename ConstLocatorT
    > bool Generator<HandleT, PoolT, ConstValueT, ConstLocatorT>::hasEnded() const {
        return current == end;
    }

    template<
            typename HandleT, typename PoolT,
            typename ConstValueT, typename ConstLocatorT
    > ConstValueT Generator<HandleT, PoolT, ConstValueT, ConstLocatorT>::next() {
        auto last = current++;
        return ConstLocatorT::locate(*last, pool);
    }

    template<typename HandleT, typename PoolT, typename ValueT, typename LocatorT>
    void Generator<HandleT, PoolT, ValueT, LocatorT>::skip() {
        current++;
    }

    template<typename HandleT, typename PoolT, typename ValueT, typename LocatorT>
    ValueT Generator<HandleT, PoolT, ValueT, LocatorT>::curr() {
        return LocatorT::locate(*current, pool);
    }

    template<
            typename HandleT, typename PoolT,
            typename ConstValueT, typename ConstGeneratorT
    > ConstIndirectArrayList<HandleT, PoolT, ConstValueT, ConstGeneratorT>::ConstIndirectArrayList(
            const HandleT *base, int32_t size, const PoolT *pool
    ) : base(base), maxSize(size), pool(pool) { }

    template<
            typename HandleT, typename PoolT,
            typename ConstValueT, typename ConstGeneratorT
    > ConstGeneratorT ConstIndirectArrayList<HandleT, PoolT, ConstValueT, ConstGeneratorT>::generator() const {
        return ConstGeneratorT(base, base + maxSize, pool);
    }

    template<typename HandleT, typename PoolT, typename ConstValueT, typename ConstGeneratorT>
    int32_t ConstIndirectArrayList<HandleT, PoolT, ConstValueT, ConstGeneratorT>::size() const {
        return maxSize;
    }

    template<
            typename HandleT, typename PoolT,
            typename ValueT, typename ConstValueT,
            typename GeneratorT, typename ConstListT
    > IndirectArrayList<HandleT, PoolT, ValueT, ConstValueT, GeneratorT, ConstListT>::IndirectArrayList(
            const HandleT *base, int32_t size, PoolT *pool
    ) : base(base), maxSize(size), pool(pool)  { }

    template<
            typename HandleT, typename PoolT,
            typename ValueT, typename ConstValueT,
            typename GeneratorT, typename ConstListT
    > GeneratorT IndirectArrayList<HandleT, PoolT, ValueT, ConstValueT, GeneratorT, ConstListT>::generator() const {
        return GeneratorT(base, base + maxSize, pool);
    }

    template<typename HandleT, typename PoolT, typename ValueT, typename ConstValueT, typename GeneratorT, typename ConstListT>
    IndirectArrayList<HandleT, PoolT, ValueT, ConstValueT, GeneratorT, ConstListT>::operator ConstListT() const {
        return ConstListT(base, size, pool);
    }

    template<typename HandleT, typename PoolT, typename ValueT, typename ConstValueT, typename GeneratorT, typename ConstListT>
    int32_t IndirectArrayList<HandleT, PoolT, ValueT, ConstValueT, GeneratorT, ConstListT>::size() const {
        return maxSize;
    }
}
