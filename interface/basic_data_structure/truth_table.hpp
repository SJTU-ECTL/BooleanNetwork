#ifndef __TRUTH__TABLE__HPP__
#define __TRUTH__TABLE__HPP__

/*******************************************************************
 * origin author: Yao Yue (Tripack45)                              *
 * *************************************************************** *
 * FIRST OCCUR IN APPROXIMATE LOGIC SYNTHESIS                      *
 * *************************************************************** *
 * modifier: Su Hang (AnthonySu), Zhou Zhuangzhuang (NathanZhou)   *
 *******************************************************************/

#include <vector>
#include <iostream>
#include "boolean_type.hpp"
#include "exception.hpp"

class TruthTable {

    int inputSize;

    std::vector<std::string> name;

    std::vector<BoolVariable> data;

    void setByVector(std::vector<int> &v) {
        if (v.size() != this->size()) {
            std::cerr << "Truth Table: ERROR VEC ASSIGNMENT\n"
                      << "VEC SIZE DO NOT MATCH\n";
            throw ExceptionInvalidVectorAssignment{};
        }
        this->data.clear();
        for (int i = 0; i < size(); i++)
            this->data.emplace_back(v[i]);
    }

    void setNameByVector(std::vector<std::string> &name) {
        for (int i = 0; i < name.size(); ++i) {
            this->name[i] = name[i];
            if (i == inputSize) break;
        }
    }

public:

    explicit TruthTable(int _inputSize) {
        if(_inputSize < 0) {
            std::cerr << "INPUT TABLE SIZE ERROR, SMALLER THAN 0\n";
            throw ExceptionTruthTableInvalidSize();
        }
        this->inputSize = _inputSize;
        for (int i = 0; i < inputSize; ++i)
            name.push_back("in" + std::to_string(i));
    }

    TruthTable(int _inputSize, std::vector<int> &v) {
        if(_inputSize < 0) {
            std::cerr << "INPUT TABLE SIZE ERROR, SMALLER THAN 0\n";
            throw ExceptionTruthTableInvalidSize();
        }
        this->inputSize = _inputSize;
        setByVector(v);
        for (int i = 0; i < inputSize; ++i)
            name.push_back("in" + std::to_string(i));
    }

    TruthTable(const TruthTable &) = default;

    TruthTable(TruthTable &&) = default;

    TruthTable &operator=(const TruthTable &) = default;

    TruthTable &operator=(TruthTable &&) = default;

    const BoolVariable &operator[](int i) const {
        return data.at(size_t(i));
    }

    BoolVariable &operator[](int i) { return data.at(size_t(i)); }

    int numInput() { return this->inputSize; }

    int size() { return 1 << inputSize; }

    std::string &getName(int i) { return this->name.at(size_t(i)); }

    void print() {
        std::cout << "Inputs: " << std::endl;
        for (auto &n : this->name) std::cout << n << "\t";
        std::cout << std::endl << "Table: " << std::endl;
        for (auto &i : this->data) std::cout << (int) i << " ";
        std::cout << std::endl;
    }

    std::string outName;

};

#endif