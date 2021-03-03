
#ifndef ITERATOR_H
#define ITERATOR_H

#include "Record.h"
#include "DataSourceStrategy.h"

template<typename Record>
class Iterator;

template<typename... TupleTypes, int... Idx>
class Iterator<Record<std::tuple<TupleTypes...>, key<Idx...>>> {

    typedef std::tuple<TupleTypes...> Tuple;
    typedef key<Idx...> KeyIndexes;
    typedef Record<Tuple, KeyIndexes> RecordType;

    int depth;
    int level;
    int treeNumber;
    DataSourceStrategy<RecordType> source;

public:
    virtual bool isLeaf() = 0;
    virtual Iterator* operator++() = 0;
    virtual Iterator* operator++(int) = 0;
    virtual bool operator==(Iterator* iter) = 0;
    virtual bool operator!=(Iterator* iter) = 0;
    virtual Iterator* begin() = 0;
    virtual Iterator* end() = 0;

    template<size_t N>
    auto getField() {
        return get<N>(source.getCurrentRecord().fields);
    }

    RecordType &operator*() {
        return *(source.getCurrentRecord());
    }

    RecordType *operator->() {
        return source.getCurrentRecord();
    }

    int getLevel() { return level; }

    int getDepth() {
        return depth;
    }

    int getTreeNumber() {
        return treeNumber;
    }

    virtual ~Iterator() = default;
};

#endif //ITERATOR_H
