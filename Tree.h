#ifndef COURSEPROJECTMAIN_TREE_H
#define COURSEPROJECTMAIN_TREE_H

#include "Record.h"
#include "FilteringPolicy.h"

template<typename DataSourcePolicy, typename FilteringPolicy = NoFilteringPolicy>
class Tree;

template<
        template<class> class DataSourcePolicy,
        typename FilteringPolicy,
        typename... Types, int... KeyIndxs>
class Tree<DataSourcePolicy<Record<tuple<Types...>, key<KeyIndxs...>>>, FilteringPolicy> {

    typedef std::tuple<Types...> Tuple;
    typedef key<KeyIndxs...> KeyIndexes;
    typedef Record<Tuple, KeyIndexes> RecordType;
    typedef DataSourcePolicy<RecordType> Source;
    typedef FilteringPolicy FilterType;

    Source& source;
    FilteringPolicy filter = FilterType();

    class TreeIterator {

        Source* source;
        FilterType& filter;
        int keyCount = sizeof...(KeyIndxs);
        int depth = keyCount;
    public:

        explicit TreeIterator(Source *_source, FilterType& _filter): source(_source), filter(_filter) {}

        void next() {
            if (depth >= 0) {
                --depth;
                if (source->hasNext()) {
                    auto key = source->getCurrentRecord()->keyTuple();
                    auto nextKey = source->getNextRecord()->keyTuple();
                    int diff = 0;
                    customCompare(key, nextKey, diff);
                    if (diff == depth) {
                        source->readRecord();
                        depth = keyCount;
                    }
                }
            }
        }

        bool isFiltered() {

            return filter.filter(source->getCurrentRecord());
        }

        bool hasNext() {

            return source != nullptr && (depth != 0 || source->getNextRecord() != nullptr);
        }

        bool isLeaf() {
            return depth == keyCount;
        }

        int getDepth() {
            return depth;
        }

        template<size_t N>
        auto getField() {
            return get<N>(source->getCurrentRecord()->fields);
        }

        RecordType operator*() {
            return *source->getCurrentRecord();
        }

        RecordType *operator->() {
            return source->getCurrentRecord();
        }

        TreeIterator operator++() {
            next();
            return *this;
        }

        bool operator!=(const TreeIterator &right) {
            if (source == nullptr && right.depth == -1 && (right.source == nullptr || !right.source->hasNext())) {
                return false;
            }
            if (right.source == nullptr && depth == -1 && (source == nullptr || !source->hasNext())) {
                return false;
            }
            if (source == nullptr || right.source == nullptr) {
                return source != right.source;
            }
            return source->getCurrentRecord() != right.source->getCurrentRecord() || depth != right.depth;
        }

        TreeIterator operator++(int) {
            TreeIterator ret = *this;
            next();
            return ret;
        }
    };

public:

    explicit Tree(Source &_source) : source(_source) {}

    explicit Tree(Source&_source, FilteringPolicy& _filter): source(_source), filter(_filter) {};

    TreeIterator begin() {
        return TreeIterator(source.makeCopy(), filter);
    }

    TreeIterator end() {
        return TreeIterator(nullptr, filter);
    }
};

#endif //COURSEPROJECTMAIN_TREE_H
