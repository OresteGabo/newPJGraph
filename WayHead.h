//
// Created by oreste on 23/12/24.
//

#ifndef NEWPJGRAPH_WAYHEAD_H
#define NEWPJGRAPH_WAYHEAD_H


#include <cstdint>
#include <vector>

class WayHead {
public:
    WayHead(uint32_t startNodeId, uint32_t endNodeId);
    void addConnectedWay(uint32_t wayId);
    const std::vector<uint32_t>& getConnectedWays() const;

private:
    uint32_t startNodeId;
    uint32_t endNodeId;
    std::vector<uint32_t> connectedWays;
};



#endif //NEWPJGRAPH_WAYHEAD_H
