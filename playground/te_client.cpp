//
// Created by Patrick Yao on 23/01/2018.
//
#include "te_public_header.h"



// (5) The interface





// (6) Another counter for testing

struct LoggingCounter
{
    int c = 2;
    explicit operator bool () const { return c; }

    LoggingCounter& operator--()
    {
        std::cout << "decremented\n";
        --c;
        return *this;
    }
};

void  test_counter()
{
    AnyCounter counter1 {2};  // bind to int (initially 2)
    assert (counter1);        // still counting
    assert (--counter1);      // still counting (1)
    AnyCounter counter2 = counter1;
    // counter2 (int) counts from 1
    --counter1;
    assert (!counter1);       // done
    assert (counter2);        // counter2 still 1
    assert (!--counter2);     // counter2 also done

    counter1 = AnyCounter{LoggingCounter{}};
    // reset with a different type
    assert (counter1);        // 2
    --counter1;
    assert (counter1);        // 1
    --counter1;
    assert (!counter1);       // 0
}

int main()
{
    test_counter();
}

BOOST_CONCEPT_ASSERT((CountdownCounter<int>));
BOOST_CONCEPT_ASSERT((CountdownCounter<LoggingCounter>));
BOOST_CONCEPT_ASSERT((CountdownCounter<AnyCounter>));