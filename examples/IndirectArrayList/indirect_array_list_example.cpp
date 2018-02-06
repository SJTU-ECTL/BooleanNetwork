//
// Created by Patrick Yao on 30/01/2018.
//
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <ectlrf/internals/indirect_array_list.h>

using namespace std;
using namespace ECTL::Internals;


class StringBuffer {
    vector<string> buffer;
    vector<int> subBuffer1;
    vector<int> subBuffer2;
public:

    explicit StringBuffer(int n) {
        for (int i = 0; i < n; i++)
            buffer.push_back("String"s + to_string(i));
    }

    void pushBuffer1(int i) {
        assert(i < buffer.size());
        subBuffer1.push_back(i);
    }

    void pushBuffer2(int i) {
        assert(i < buffer.size());
        subBuffer2.push_back(i);
    }

    void print() const {
        int count = 0;
        for(const auto& str : buffer) {
            cout << str << '\t';
            if (count == 5) {
                cout << endl;
                count = 0;
            }
            count++;
        }
    }

    struct ConstLocator {
        static const string& locate(int handle, const vector<string>* pool);
    };

    // First, const genrator class
    // A const generate, will use integers (index) as handle
    //                   draw from a vector<string> pool
    //                   and returns const references to string
    class ConstGenerator :
            IndirectArrayList::Generator<
                    int, const vector<string>, const string&, ConstLocator
            > {
    public:
        using base_t = IndirectArrayList::Generator<
                int, const vector<string>, const string&, ConstLocator
        >;
        using base_t::Generator; // Expose both CTors
        using base_t::hasEnded;       // Expose hasEnded();
        using base_t::next;           // Expose next()
    };

    // Next, a const reference set class
    class ConstArrayList :
            IndirectArrayList::ConstIndirectArrayList<
                    int, vector<string>, const string&, ConstGenerator
            > {
    public:
        using base_t = IndirectArrayList::ConstIndirectArrayList<
                int, vector<string>, const string&, ConstGenerator
        >;
        using base_t::ConstIndirectArrayList; // Expose CTor
        using base_t::generator;              // Expose Generator;
    };

    ConstArrayList getBuffer1() const {
        return ConstArrayList(this->subBuffer1.data(), subBuffer1.size(), &buffer);
    }

    ConstArrayList getBuffer2() const {
        return ConstArrayList(this->subBuffer2.data(), subBuffer2.size(), &buffer);
    }
};

const string &StringBuffer::ConstLocator::locate(int handle, const vector<string> *pool) {
    return pool->at(handle);
}

int main() {
    StringBuffer buffer(30);
    buffer.print();
    buffer.pushBuffer1(10);
    buffer.pushBuffer1(20);
    buffer.pushBuffer2(1);
    buffer.pushBuffer2(11);
    buffer.pushBuffer2(1);
    auto l1 = buffer.getBuffer1();
    auto l2 = buffer.getBuffer2();

    cout << std::endl;
    for (auto g = l1.generator(); !g.hasEnded(); )
        cout << "buffer1 => " << g.next() << endl;
    for (auto g = l2.generator(); !g.hasEnded(); )
        cout << "buffer2 => " << g.next() << endl;

}