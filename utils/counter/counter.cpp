//
// Created by tripack on 17-3-26.
//

#include <counter/counter.h>
#include <unordered_map>
#include <iostream>
#include <exception>

using namespace ECTL::Utils;

using std::string;
using std::unordered_map;
using std::cout;
using std::endl;


static unordered_map<string, int32_t> globalCounter;

bool LoopCounter::exist(const std::string &name) {
    if (globalCounter.find(name) == globalCounter.end()) {
        cout << "Counter '" << name << "' does not exist!" << endl;
        return false;
    } else {
        return true;
    }
}

LoopCounter::LoopCounter(std::string name_, bool inc)
        : name(std::move(name_)) {
    if (inc) {
        globalCounter[name] ++;
    } else {
        // Make sure the counter does exist, no matter whether it
        // should be increamented.
        globalCounter.try_emplace(name, 0);
    }
}

int32_t LoopCounter::value() const {
    // Note this function is member function
    // Normally there is no way to delete the counter
    // with the contructed object, so this should always
    // work. Otherwise the user is doing something wired
    return globalCounter.at(name);
}

void LoopCounter::reset() const {
    // See comments in non-static LoopCounter::value()
    globalCounter.at(name) = 0;
}

int32_t LoopCounter::step() const {
    // See comments in non-static LoopCounter::value()
    return ++globalCounter.at(name);
}


LoopCounter::operator bool() const {
    return true;
}


int32_t LoopCounter::step(const std::string &name) {
    /* Unlike the member function versions, these static versions
     * will create the counter if it doesn't exist already. This
     * behavior is intended. Since users may choose to query a counter
     * after a zero-round for loop
     *
     * - map::try_emplace(key, value)
     *     - Do nothing if key already exists
     *     - Sets value to value if not exist //*/
    globalCounter.try_emplace(name, 0);
    return ++globalCounter.at(name);
}

int32_t LoopCounter::value(const std::string &name) {
    // See static LoopCounter::step()
    globalCounter.try_emplace(name, 0);
    return globalCounter.at(name);
}

void LoopCounter::reset(const std::string &name) {
    // See static LoopCounter::step()
    globalCounter[name] = 0;
}

void LoopCounter::clear(const std::string &name) {
    globalCounter.erase(name);
}

void LoopCounter::show(const std::string &name) {
    globalCounter.try_emplace(name, 0);
    auto value = globalCounter.at(name);
    cout << "Counter '" << name << "' counts " << value <<  "." << endl;
}

void LoopCounter::showall(const std::string &name) {
    for (auto& pair : globalCounter)
        cout << "Counter '" << pair.first << "' counts "
             << pair.second << "." << endl;
}
