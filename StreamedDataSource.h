#ifndef COURSEPROJECT_STREAMED_DS_H
#define COURSEPROJECT_STREAMED_DS_H

template<typename InputDataSource, typename Condition, typename Composer, typename OutRecordType>
auto map(InputDataSource& ds, Condition& cond, Composer& comp, OutRecordType rType) {

    return ComputedDataSource<OutRecordType, decltype(ds), decltype(cond), decltype(comp)>(ds, cond, comp);
}

#endif