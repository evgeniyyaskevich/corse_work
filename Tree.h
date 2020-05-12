#ifndef COURSEPROJECTMAIN_TREE_H
#define COURSEPROJECTMAIN_TREE_H

#include "Record.h"

template<typename DataSourcePolicy>
class Tree;

template<
        template<class> class DataSourcePolicy,
//        template<class> class FilteringPolicy = NoFilteringPolicy,
        typename... Types, int... KeyIndxs>
class Tree<DataSourcePolicy<Record<tuple<Types...>, key<KeyIndxs...>>>> {

    typedef std::tuple<Types...> Tuple;
    typedef key<KeyIndxs...> KeyIndexes;
    typedef Record<Tuple, KeyIndexes> Record;
    typedef DataSourcePolicy<Record> Source;

    Source& source;

    class TreeIterator {

        Source* source;
        int keyCount = sizeof...(KeyIndxs);
        int depth = keyCount;
    public:

        explicit TreeIterator(Source *_source) {
            source = _source;
        };

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

        bool hasNext() {

            return depth != 0 || source->getNextRecord() != nullptr;
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

        Record operator*() {
            return *source->getCurrentRecord();
        }

        Record *operator->() {
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

    TreeIterator begin() {
        return TreeIterator(source.makeCopy());
    }

    TreeIterator end() {
        return TreeIterator(nullptr);
    }
};

#endif //COURSEPROJECTMAIN_TREE_H
