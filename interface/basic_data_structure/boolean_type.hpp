#ifndef __BOOLEAN__TYPE__HPP__
#define __BOOLEAN__TYPE__HPP__

/*******************************************************************
 * origin author: Yao Yue (Tripack45)                              *
 * *************************************************************** *
 * FIRST OCCUR IN APPROXIMATE LOGIC SYNTHESIS                      *
 * *************************************************************** *
 * modifier: Su Hang (AnthonySu), Zhou Zhuangzhuang (NathanZhou)   *
 *******************************************************************/

#include <string>

class BoolVariable {

    BoolVariable &copy(const bool &b) {
        // if (b) { state = BoolTRUE; return *this; }
        // state = BoolFALSE;
        state = b ? BoolTRUE : BoolFALSE;
        return *this;
    }

    BoolVariable &copy(const int &i) {
        // if (i) { state = BoolTRUE; return *this; }
        // state = BoolFALSE;
        state = i ? ((i == 1) ? BoolTRUE : DC) : BoolFALSE;
        return *this;
    }

    enum State {
        BoolFALSE, BoolTRUE, DC
    };

    State state;

public:

    BoolVariable() : state(BoolFALSE) {};

    BoolVariable(const BoolVariable &b) = default;

    BoolVariable(BoolVariable &&b) = default;

    explicit BoolVariable(const bool &b) : state(DC) { copy(b); };

    explicit BoolVariable(const int &i) : state(DC) { copy(i); }

    BoolVariable &operator=(const BoolVariable &b) = default;

    BoolVariable &operator=(BoolVariable &&b) = default;

    BoolVariable &operator=(const bool &b) {
        this->copy(b);
        return *this;
    }

    BoolVariable &operator=(const int &i) {
        this->copy(i);
        return *this;
    }

    explicit operator bool() const {
        if (state == BoolTRUE) return true;
    }

    explicit operator int() const {
        if (state == BoolTRUE) return 1;
        if (state == DC) return -1;
        return 0;
    }

    explicit operator const char *() const {
        if (state == BoolTRUE) return "true";
        if (state == DC) return "don_t_care";
        return "false";
    }

    std::string toString() const {
        if (state == BoolTRUE) return "true";
        if (state == DC) return "don_t_care";
        return "false";
    }

};

#endif