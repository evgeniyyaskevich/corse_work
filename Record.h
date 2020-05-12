#ifndef FILE_RECORD_H_
#define FILE_RECORD_H_

#include "TupleUtil.h"
#include <tuple>

using namespace std;

template<typename Tuple, typename Key>
class Record;

template<typename Keys>
class Key;

template<int... Nums>
struct key {
};

template<int... Indexes, typename Tuple>
decltype(auto) extractKeys(Tuple &&t) {
    return make_tuple(get<Indexes>(forward<Tuple>(t))...);
}

template<typename... Types>
class Key<tuple<Types...>> {
public:
    typedef tuple<Types...> Tuple;

    Tuple keys;

    explicit Key(Tuple const &_keys) : keys(_keys) {};

    template<typename KeyType>
    bool operator<(KeyType const &right) {
        return compare(keys, right.keys) < 0;
    }

    template<typename KeyType>
    bool operator>(KeyType const &right) {
        return compare(keys, right.keys) > 0;
    }

    template<typename KeyType>
    bool operator==(KeyType const &right) {
        return compare(keys, right.keys) == 0;
    }
};

template<typename... Types, int... Nums>
class Record<tuple<Types...>, key<Nums...>> {

public:
    typedef tuple<Types...> Tuple;
    Tuple fields;

    Record() = default;

    explicit Record(Tuple _fields) : fields(_fields) {}

    auto key() const {
        auto keyFields = extractKeys<Nums...>(fields);
        return Key<decltype(keyFields)>(keyFields);
    }

    auto keyTuple() const {
        return extractKeys<Nums...>(fields);
    };

    friend ostream &operator<<(ostream &os, Record const &record) {
        return os << record.fields;
    }

    ~Record() = default;
};

#endif