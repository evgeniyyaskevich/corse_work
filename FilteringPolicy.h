#ifndef FILTERING_POLICY_H
#define FILTERING_POLICY_H

#include "Record.h"

template<typename Record, typename Conditions>
class FilteringPolicy;

template<typename... Types, int... KeyIndxs, size_t... n, typename... CondFuncObj>
class FilteringPolicy<Record<tuple<Types...>, key<KeyIndxs...>>, tuple<FieldCondition<n, CondFuncObj>...>> {

public:
    typedef Record<tuple<Types...>, key<KeyIndxs...>> Record;
    typedef tuple<FieldCondition<n, CondFuncObj>...> FieldConditionType;

private:
    FieldConditionType conditions;

public:
    FilteringPolicy() = default;
    explicit FilteringPolicy(FieldConditionType _conditions) : conditions(_conditions) {}

    bool filter(Record* record) {

        return isCondition(conditions, record->fields);
    }

};

class NoFilteringPolicy {

public:

    template<typename Record>
    bool filter(Record record) {

        return true;
    }
};

#endif