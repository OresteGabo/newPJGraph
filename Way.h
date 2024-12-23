//
// Created by oreste on 23/12/24.
//

#ifndef NEWPJGRAPH_WAY_H
#define NEWPJGRAPH_WAY_H


#include <cstdint>
#include <QString>

class Way {
public:
    Way(uint64_t id, const QString &tag)
            : id(id), tag(tag) {}

    [[nodiscard]] auto getId() const { return id; }
    [[nodiscard]] auto getTag() const { return tag; }

private:
    uint64_t id;
    QString tag;
};



#endif //NEWPJGRAPH_WAY_H
