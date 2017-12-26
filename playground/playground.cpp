
#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>
#include <map>
#include <deque>
#include "bnet.h"

//const int INF = std::numeric_limits<int>::max();

const int INF = 100000;

struct STA;

std::vector<BnetNodeID> topologicalSort(const BnetNetwork *net);

std::map<BnetNodeID, STA> staticTimingAnalysis(const BnetNetwork *net);


int main() {
    BnetNetwork *net;
    std::string blif_file_path(PROJECT_SOURCE_DIR);
    blif_file_path += "/playground/benchmark/alu4.blif";
    net = new BnetNetwork(blif_file_path);
    net->printNetwork();
    std::vector<BnetNodeID> sortedNodes = topologicalSort(net);
    std::cout << "Topological Order of Nodes: ";
    for (auto &i : sortedNodes)
        std::cout << i << ' ';
    std::cout<<std::endl;
    std::map<BnetNodeID, STA> STAResult = staticTimingAnalysis(net);
    delete (net);
    return 0;
}

struct STA {
    int AT;      /**< Arrival Time */
    int RAT;     /**< Required Arrival Time */
    int Slack;   /**< Slack = RAT – AT */
    STA(int AT, int RAT, int Slack) : AT(AT), RAT(RAT), Slack(Slack) {}
};

/**
 * @brief get the topological order of nodes in BooleanNetwork
 * @return the vector of nodes(name) in topological order
 */

std::vector<BnetNodeID> topologicalSort(const BnetNetwork *net) {
    std::vector<BnetNodeID> sortedNodes;
    std::deque<BnetNodeID> nodesQueue;
    std::map<BnetNodeID, int> inDegrees;

    for (auto node:net->getNodesList()) {
        inDegrees[node->getName()] = (int) node->getInputs().size();
        if (node->getInputs().empty())
            nodesQueue.emplace_back(node->getName());
    }

    while (!nodesQueue.empty()) {
        BnetNodeID id = nodesQueue.front();
        nodesQueue.pop_front();

        const BnetNode *node = net->getNodebyName(id);
        sortedNodes.emplace_back(node->getName());
        for (const auto &output:node->getOutputs()) {
            inDegrees[output] -= 1;
            if (inDegrees[output] == 0)
                nodesQueue.push_back(output);
        }
    }

    return sortedNodes;
}

/**
 * @brief
 * @return
 */

std::map<BnetNodeID, STA> staticTimingAnalysis(const BnetNetwork *net) {
    std::vector<BnetNodeID> sortedNodes = topologicalSort(net);
    std::map<BnetNodeID, STA> STAResult;
    /* Initialize STAResult*/
    for (auto it = sortedNodes.begin(); it != sortedNodes.end(); ++it) {
        const BnetNode *node = net->getNodebyName(*it);
        STA sta(-1 * INF, INF, 0);
        if (node->getInputs().empty()) sta.AT = 0;
        if (node->getOutputs().empty()) sta.RAT = 0;
        STAResult.insert(std::pair<BnetNodeID, STA>(*it, sta));
    }
    /* Update AT*/
    for (auto it = sortedNodes.begin(); it != sortedNodes.end(); ++it) {
        if (!net->getNodebyName(*it)->getInputs().empty())
            STAResult.at(*it).AT = -1 * INF;
        for (const auto &input:net->getNodebyName(*it)->getInputs())
            STAResult.at(*it).AT = std::max(STAResult.at(*it).AT, STAResult.at(input).AT + 1);
    }
    /* Update RAT*/
    for (auto rit = sortedNodes.rbegin(); rit != sortedNodes.rend(); ++rit) {
        if (!net->getNodebyName(*rit)->getOutputs().empty())
            STAResult.at(*rit).RAT = INF;
        for (const auto &output:net->getNodebyName(*rit)->getOutputs())
            STAResult.at(*rit).RAT = std::min(STAResult.at(*rit).RAT, STAResult.at(output).RAT - 1);
    }

    /* Update Slack*/
    for (auto node:net->getNodesList()) {
        STAResult.at(node->getName()).Slack = STAResult.at(node->getName()).RAT - STAResult.at(node->getName()).AT;
        std::cout<<node->getName()<<"="<<STAResult.at(node->getName()).Slack<<"   ";
    }
}