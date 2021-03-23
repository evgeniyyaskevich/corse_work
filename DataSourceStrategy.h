#ifndef DATASOURCESTRATEGY_H
#define DATASOURCESTRATEGY_H

#include "Record.h"
#include "FilteringPolicy.h"
#include <tuple>
#include <memory>

using namespace std;

template<typename Record, typename FilteringPolicy = NoFilteringPolicy>
class DataSourceStrategy;

template<typename FilteringPolicy, typename... TupleTypes, int... Indxs>
class DataSourceStrategy<Record<tuple<TupleTypes...>, key<Indxs...>>, FilteringPolicy> {

public:

    typedef std::tuple<TupleTypes...> Tuple;
    typedef key<Indxs...> KeyIndexes;
    typedef Record<Tuple, KeyIndexes> RecordType;

    FilteringPolicy filter = FilteringPolicy();

    DataSourceStrategy() {};
    DataSourceStrategy(FilteringPolicy& _filter) : filter(_filter) {};

    virtual RecordType *getCurrentRecord() = 0;

    virtual RecordType *getNextRecord() = 0;

    virtual RecordType *readRecord() = 0;

    virtual bool hasNext() = 0;

    virtual DataSourceStrategy<RecordType, FilteringPolicy>* makeCopy() = 0;
};

#endif //DATASOURCESTRATEGY_H
