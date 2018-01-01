#ifndef BOOLEANNETWORK_STA_H
#define BOOLEANNETWORK_STA_H

#include <iostream>
#include <queue>
#include <deque>
#include "bnet.h"

struct PartialPath {
    std::vector<BnetNodeID> path;
    int slack;

    PartialPath(BnetNodeID node, int slack, std::vector<BnetNodeID>
    path = std::vector<BnetNodeID>()) {
        this->path = std::move(path);
        this->path.emplace_back(node);
        this->slack = slack;
    }
};

std::vector<BnetNodeID> TopologicalSort(const BnetNetwork *net);

std::map<BnetNodeID, int> StaticTimingAnalysis(const BnetNetwork *net);

void WorstPathReporting(const BnetNetwork *net, std::map<BnetNodeID, int> &slack, int num = 10);

#endif
