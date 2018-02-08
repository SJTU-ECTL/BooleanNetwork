//
// Created by Patrick Yao on 30/01/2018.
//

#ifndef BOOLEANNETWORK_ECTL_INTERNALS_H
#define BOOLEANNETWORK_ECTL_INTERNALS_H

#include <cinttypes>
#include <functional>
#include <exception>

namespace ECTL::Internals {

    typedef union {
        int64_t iVal;
        double  dVal;
        void*   ptr;
        struct { void* p1;   void* p2; } ptrPair;
        struct { int32_t x;  int32_t y; } intPair;
        struct { void* base; int32_t id; int32_t max; } arrayRef;
    } smallBuffer;
}


#endif //BOOLEANNETWORK_ECTL_INTERNALS_H
