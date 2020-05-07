#ifndef FILE_READER_H_
#define FILE_READER_H_

#include "TupleUtil.h"
#include "DataSourceStrategy.h"
#include <tuple>
#include <array>
#include <sstream>
#include <fstream>
#include <stdexcept>

template<typename Record>
class TextFileSource;

template<typename... Types, int... Keys>
class TextFileSource<Record<tuple<Types...>, key<Keys...>>>
        : DataSourceStrategy<Record<tuple<Types...>, key<Keys...>>> {
public:
    typedef Record<tuple<Types...>, key<Keys...>> Record;
private:
    ifstream in;
    bool isHasNext = true;
    Record currentRecord;
public:

    explicit TextFileSource(const string &fileName) {
        in.open(fileName);
    }

    Record getCurrentRecord() override {
        return currentRecord;
    }

    Record readRecord() override {

        tuple<Types...> inputTuple;
        if (hasNext()) {
            if (in >> inputTuple) {
                currentRecord = Record(inputTuple);
            } else {
                isHasNext = false;
            }
            return currentRecord;
        }
        throw logic_error("There are no records. User hasNext() to check it before reading.");
    }

    bool hasNext() override {
        return isHasNext;
    }

    virtual ~TextFileSource() {
        in.close();
    }
};

#endif