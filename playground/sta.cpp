#include "sta.h"

static const int INF = 100000;

std::vector<BnetNodeID> TopologicalSort(const BnetNetwork *net) {
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

std::map<BnetNodeID, int> StaticTimingAnalysis(const BnetNetwork *net) {
    std::vector<BnetNodeID> sortedNodes = TopologicalSort(net);
    std::map<BnetNodeID, int> arrival_time;
    std::map<BnetNodeID, int> required_arrival_time;
    std::map<BnetNodeID, int> slack;
    /* Initialization */
    for (auto it = sortedNodes.begin(); it != sortedNodes.end(); ++it) {
        arrival_time.insert(std::pair<BnetNodeID, int>(*it, -1 * INF));
        required_arrival_time.insert(std::pair<BnetNodeID, int>(*it, INF));
        slack.insert(std::pair<BnetNodeID, int>(*it, 0));
    }
    int max_at = -1 * INF;
    /* Update at */
    arrival_time.at(SOURCE_NAME) = 0;
    for (auto it = sortedNodes.begin(); it != sortedNodes.end(); ++it) {
        for (const auto &input:net->getNodebyName(*it)->getInputs())
            arrival_time.at(*it) = std::max(arrival_time.at(*it), arrival_time.at(input) + 1);
        if (arrival_time.at(*it) > max_at)
            max_at = arrival_time.at(*it);
    }
    /* Update rat */
    required_arrival_time.at(SINK_NAME) = max_at;
    for (auto rit = sortedNodes.rbegin(); rit != sortedNodes.rend(); ++rit) {
        for (const auto &output:net->getNodebyName(*rit)->getOutputs())
            required_arrival_time.at(*rit) = std::min(
                    required_arrival_time.at(*rit), required_arrival_time.at(output) - 1);
    }
    /* Update slack */
    for (auto node:net->getNodesList())
        slack.at(node->getName()) = required_arrival_time.at(node->getName())
                                    - arrival_time.at(node->getName());
    return slack;
}

void WorstPathReporting(const BnetNetwork *net, std::map<BnetNodeID, int> &slack, int num) {
    auto comp = [](PartialPath a, PartialPath b) { return a.slack > b.slack; };
    std::priority_queue<PartialPath, std::vector<PartialPath>, decltype(comp)> min_heap(comp);

    PartialPath source_node(SOURCE_NAME, slack.at(SOURCE_NAME));
    min_heap.push(source_node);

    while (!min_heap.empty() && num > 0) {
        PartialPath path_t = min_heap.top();
        min_heap.pop();
        if (path_t.path.back() == SINK_NAME) {
            num--;
            std::cout << "Delay: " << path_t.path.size() - 2 << "\t";
            for (const auto &node : path_t.path)
                std::cout << node << "^" << slack.at(node) << " ";
            std::cout << std::endl;
        } else {
            BnetNode *node_t = net->getNodebyName(path_t.path.back());
            for (const auto &successor : node_t->getOutputs()) {
                PartialPath new_path(successor, slack.at(successor), path_t.path);
                min_heap.push(new_path);
            }
        }
    }
}
