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
    ) : curr(curr), end(end), pool(pool) { }

    template<
            typename HandleT, typename PoolT,
            typename ConstValueT, typename ConstLocatorT
    > bool Generator<HandleT, PoolT, ConstValueT, ConstLocatorT>::hasEnded() const {
        return curr == end;
    }

    template<
            typename HandleT, typename PoolT,
            typename ConstValueT, typename ConstLocatorT
    > ConstValueT Generator<HandleT, PoolT, ConstValueT, ConstLocatorT>::next() {
        auto last = curr++;
        return ConstLocatorT::locate(*last, pool);
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
        return ConstGeneratorT(base, base + size, pool);
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
        return GeneratorT(base, base + size, pool);
    }

    template<typename HandleT, typename PoolT, typename ValueT, typename ConstValueT, typename GeneratorT, typename ConstListT>
    IndirectArrayList<HandleT, PoolT, ValueT, ConstValueT, GeneratorT, ConstListT>::operator ConstListT() const {
        return ConstListT(base, size, pool);
    }
}
