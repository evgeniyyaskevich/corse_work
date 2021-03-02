#include "Record.h"
#include "TextFileSource.h"
#include "DataSourceStrategy.h"
#include "Iterator.h"
#include "Tree.h"
#include "LogTreeTraversing.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stack>
#include <sstream>
#include <vector>
#include <assert.h>

using namespace std;

void printInfo(string info) {
    cout << "###################" << info << "###################" << endl;
}

typedef tuple<int, int, string, string, int> tupleSubType;
typedef key<0, 1, 2, 3> keySubType;
typedef Record<tupleSubType, keySubType> RecordType;

int conditionTest() {

    printInfo("ConditionTest started.");

    TextFileSource<RecordType> fileReader("input/input1.txt");

    auto field0Cond = [](int courseNumber) { return courseNumber < 3; };
    auto field3Cond = [](string subjectValue) { return subjectValue != "MA"; };
    auto field4Cond = [](int markValue) { return markValue == 10; };
    auto condTuple = make_tuple(make_fieldCondition<0, decltype(field0Cond)>(field0Cond),
                                make_fieldCondition<3, decltype(field3Cond)>(field3Cond),
                                make_fieldCondition<4, decltype(field4Cond)>(field4Cond));

    vector<RecordType *> records;
    while (fileReader.hasNext()) {
        if (isCondition(condTuple, fileReader.readRecord()->fields)) {
            records.push_back(fileReader.getCurrentRecord());
        }
    }

    assert(records.size() == 1);
    RecordType::Tuple fields = records.at(0)->fields;
    assert(get<0>(fields) == 1);
    assert(get<1>(fields) == 2);
    assert(get<2>(fields) == "Myatin");
    assert(get<3>(fields) == "MO");

    printInfo("ConditionTest finished.");
    return 0;
}

void iteratorUsing() {

    printInfo("input1.txt traversing by iterator started.");

    TextFileSource<RecordType> fileReader("input/input1.txt");
    Tree<TextFileSource<RecordType>> tree(fileReader);

    printInfo("First option result:");
    auto it = tree.begin();
    while (it.hasNext()) {
        cout << *it << ", depth=" << it.getDepth() << ", isLeaf=" << it.isLeaf() << ", groupNumber="
             << it.getField<1>() << endl;
        ++it;
    }

    printInfo("Second option result:");
    for (auto iter = tree.begin(); iter != tree.end(); ++iter) {
        cout << *iter << ", depth=" << iter.getDepth() << ", isLeaf=" << iter.isLeaf() << ", groupNumber "
             << iter.getField<1>() << endl;
    }

    printInfo("input1.txt traversing by iterator finished.");
}

void filterTest() {

    printInfo("Filter test started.");

    TextFileSource<RecordType> fileReader("input/input1.txt");
    auto field0Cond = [](int courseNumber) { return courseNumber < 3; };
    auto field3Cond = [](string subjectValue) { return subjectValue == "MO"; };
    auto field4Cond = [](int markValue) { return markValue == 10; };
    auto condTuple = make_tuple(make_fieldCondition<0, decltype(field0Cond)>(field0Cond),
                                make_fieldCondition<3, decltype(field3Cond)>(field3Cond),
                                make_fieldCondition<4, decltype(field4Cond)>(field4Cond));

    FilteringPolicy<RecordType, decltype(condTuple)> filter(condTuple);
    Tree<TextFileSource<RecordType>, decltype(filter)> tree(fileReader, filter);
    for (auto iter = tree.begin(); iter != tree.end(); ++iter) {
        if (iter.isFiltered()) {
            assert(iter.getField<0>() < 3);
            assert(iter.getField<3>() == "MO");
            assert(iter.getField<4>() == 10);
            cout << *iter << ", depth=" << iter.getDepth() << ", isLeaf=" << iter.isLeaf() << ", groupNumber "
                 << iter.getField<1>() << endl;
        }
    }

    printInfo("Filter test finished.");
}

void allLevelSumExample() {

    typedef tuple<int, int, int, int> tupleSubType;
    typedef key<0, 1, 2> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    TextFileSource<RecordType> fileReader("input/input3.txt");
    Tree<TextFileSource<RecordType>> tree(fileReader);

    int sum[3]{0, 0, 0};
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        if (it.isLeaf()) {
            int i = it.getDepth();
            sum[i - 1] += it.getField<3>();
            cout << "Leaf: " << *it << endl;
        } else {
            int i = it.getDepth();
            cout << "Level=" << it.getDepth() << ", key=" << (*it).keyTuple() << ", sum=" << sum[i] << endl;
            if (i > 0) {
                sum[i - 1] += sum[i];
            }
            sum[i] = 0;
        }
    }
}

void averageMark() {

    typedef tuple<int, string, string, int> tupleSubType;
    typedef key<0, 1, 2> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    TextFileSource<RecordType> fileReader("input/input4.txt");
    Tree<TextFileSource<RecordType>> tree(fileReader);

    int sum = 0, count = 0;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        if (it.isLeaf()) {
            sum += it.getField<3>();
            ++count;
        } else if (it.getDepth() == 2) {
            cout << it.getField<1>() << " - " << (double) sum / count << endl;
            sum = count = 0;
        }
    }
}

int main() {

    filterTest();
    conditionTest();
    treeTraversing();
    iteratorUsing();

    allLevelSumExample();
    averageMark();


    return 0;
}