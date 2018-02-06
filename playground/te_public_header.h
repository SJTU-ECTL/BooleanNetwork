//
// Created by Patrick Yao on 23/01/2018.
//

#ifndef BOOLEANNETWORK_TE_PUBLIC_HEADER_H
#define BOOLEANNETWORK_TE_PUBLIC_HEADER_H


#include <iostream>
#include <adobe/poly.hpp>
#include <adobe/poly_copyable.hpp>
#include <boost/concept_check.hpp>

template <typename C>
struct CountdownCounter : boost::Assignable<C>
        , boost::CopyConstructible<C>
{
    BOOST_CONCEPT_USAGE(CountdownCounter)
    {
        --c;
        (bool(c));
        bool b = !c;
        (bool(--c));
    }

private:
    C c;
};

// (2) The inner interface

struct CounterIface : adobe::poly_copyable_interface
{
    virtual void decrement() = 0;
    virtual bool done() const = 0;
};

// (3) The inner interface implementation

template <typename T>
struct CounterImpl
        : adobe::optimized_storage_type<T, CounterIface>::type
{
    using base_t = typename
    adobe::optimized_storage_type<T, CounterIface>::type;

    BOOST_CONCEPT_ASSERT((CountdownCounter<T>));

    CounterImpl(T x) : base_t(x) {}
    CounterImpl(adobe::move_from<CounterImpl> x)
            : base_t(adobe::move_from<base_t>(x.source)) {}

    void decrement() override { --this->get(); }
    bool done() const override { return bool(this->get()); }
};

struct Counter : adobe::poly_base<CounterIface, CounterImpl>
{
    using base_t = adobe::poly_base<CounterIface, CounterImpl>;
    using base_t::base_t; // Inherit constructors

    Counter(adobe::move_from<Counter> x)
            : base_t(adobe::move_from<base_t>(x.source)) {}

    Counter& operator--()
    {
        interface_ref().decrement();
        return *this;
    }

    explicit operator bool() const
    {
        return interface_ref().done();
    }
};

typedef adobe::poly<Counter> AnyCounter;


#endif //BOOLEANNETWORK_TE_PUBLIC_HEADER_H
