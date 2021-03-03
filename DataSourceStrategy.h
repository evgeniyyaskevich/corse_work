#ifndef DATASOURCESTRATEGY_H
#define DATASOURCESTRATEGY_H

#include "Record.h"
#include <tuple>
#include <memory>

using namespace std;

template<typename Record>
class DataSourceStrategy;

template<typename... TupleTypes, int... Indxs>
class DataSourceStrategy<Record<tuple<TupleTypes...>, key<Indxs...>>> {
public:

    typedef std::tuple<TupleTypes...> Tuple;
    typedef key<Indxs...> KeyIndexes;
    typedef Record<Tuple, KeyIndexes> RecordType;

    virtual RecordType *getCurrentRecord() = 0;

    virtual RecordType *getNextRecord() = 0;

    virtual RecordType *readRecord() = 0;

    virtual bool hasNext() = 0;

    virtual DataSourceStrategy<RecordType>* makeCopy() = 0;
};

#endif //DATASOURCESTRATEGY_H
