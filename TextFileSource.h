#ifndef FILE_READER_H_
#define FILE_READER_H_

#include "TupleUtil.h"
#include "DataSourceStrategy.h"
#include <tuple>
#include <array>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <memory>

template<typename Record>
class TextFileSource;

template<typename... Types, int... Keys>
class TextFileSource<Record<tuple<Types...>, key<Keys...>>>
        : DataSourceStrategy<Record<tuple<Types...>, key<Keys...>>> {
public:
    typedef Record<tuple<Types...>, key<Keys...>> Record;
private:
    ifstream in;
    string fileName;
    bool isHasNext = true;
    Record *currentRecord = nullptr;
    Record *nextRecord = nullptr;
public:

    explicit TextFileSource(const string &fileName) : fileName(fileName) {
        in.open(fileName);
        readRecord();
        readRecord();
    }

    TextFileSource(TextFileSource const &right) : TextFileSource(right.fileName) {}

    Record *getCurrentRecord() override {

        if (currentRecord == nullptr) {
            readRecord();
        }
        return currentRecord;
    }

    Record *getNextRecord() override {

        return nextRecord;
    }

    Record *readRecord() override {

        tuple<Types...> inputTuple;
        currentRecord = nextRecord;
        if (hasNext()) {
            if (in >> inputTuple) {
                nextRecord = new Record(inputTuple);
            } else {
                isHasNext = false;
                nextRecord = nullptr;
            }
            return currentRecord;
        }
        throw logic_error("There are no records. Use this::hasNext() to check it before reading.");
    }

    bool hasNext() override {
        return isHasNext;
    }

    TextFileSource<Record>* makeCopy() override {

        return new TextFileSource<Record>(fileName);
    }

public:

    virtual ~TextFileSource() {
        in.close();
    }
};

#endif