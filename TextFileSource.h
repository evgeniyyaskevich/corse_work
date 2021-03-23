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

template<typename Record, typename FilteringPolicy = NoFilteringPolicy>
class TextFileSource;

template<typename FilteringPolicy, typename... Types, int... Keys>
class TextFileSource<Record<tuple<Types...>, key<Keys...>>, FilteringPolicy>
        : public DataSourceStrategy<Record<tuple<Types...>, key<Keys...>>, FilteringPolicy> {
public:
    typedef Record<tuple<Types...>, key<Keys...>> RecordType;
    typedef DataSourceStrategy<Record<tuple<Types...>, key<Keys...>>, FilteringPolicy> Source;
private:
    ifstream in;
    string fileName;
    bool isHasNext = true;
    RecordType *currentRecord = nullptr;
    RecordType *nextRecord = nullptr;
public:

    explicit TextFileSource(const string &fileName) : fileName(fileName) {
        in.open(fileName);
        readRecord();
        readRecord();
    }

    explicit TextFileSource(const string &fileName, FilteringPolicy& _filter) : Source(_filter), fileName(fileName) {
        in.open(fileName);
        readRecord();
        readRecord();
    }

    TextFileSource(TextFileSource const &right) : TextFileSource(right.fileName) {}

    RecordType *getCurrentRecord() override {

        if (currentRecord == nullptr) {
            readRecord();
        }
        return currentRecord;
    }

    RecordType *getNextRecord() override {

        return nextRecord;
    }

    RecordType *readRecord() override {

        tuple<Types...> inputTuple;
        currentRecord = nextRecord;
        if (!hasNext()) {
            throw logic_error("There are no records. Use this::hasNext() to check it before reading.");
        }

        bool read, filtered;
        do {
            if (in >> inputTuple) {
                read = true;
                nextRecord = new RecordType(inputTuple);
                filtered = !this->filter.filter(nextRecord);
            } else {
                read = false;
                isHasNext = false;
                nextRecord = nullptr;
            }
        } while (read && filtered);

        return currentRecord;
    }

    bool hasNext() override {
        return isHasNext;
    }

    TextFileSource<RecordType, FilteringPolicy>* makeCopy() override {

        return new TextFileSource<RecordType, FilteringPolicy>(fileName, this->filter);
    }

public:

    virtual ~TextFileSource() {
        in.close();
    }
};

#endif