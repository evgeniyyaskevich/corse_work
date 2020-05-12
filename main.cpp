#include "Record.h"
#include "TextFileSource.h"
#include "DataSourceStrategy.h"
#include "Iterator.h"
#include "Tree.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stack>
#include <sstream>

using namespace std;

template<class RecordSubType, class KeySubType>
struct IteratorRecord {
    int level = 0;
    int treeNumber = 0;
    Record<RecordSubType, KeySubType> value{};

    friend ostream &operator<<(ostream &os, IteratorRecord const &rec) {
        os << "Level=" << rec.level << ", treeNumber=" << rec.treeNumber << ", value={" << rec.value << "}";
        return os;
    }
};

int conditionTest() {
    typedef tuple<int, int, string, string, int> tupleSubType;
    typedef key<0, 1, 2, 3> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    TextFileSource<RecordType> fileReader("../input1.txt");
    Record<tupleSubType, keySubType> z = *fileReader.readRecord(), zBuf = z;

    auto field0 = [](int x) { return x > 2; };
    auto field3 = [](string x) { return x != "MA"; };
    auto condTuple = make_tuple(make_fieldCondition<0, decltype(field0)>(field0),
                                make_fieldCondition<3, decltype(field3)>(field3));

    if (isCondition(condTuple, z.fields)) {
        cout << z << endl;
    }
    while (fileReader.hasNext()) {
        z = *fileReader.readRecord();
        if (isCondition(condTuple, z.fields)) {
            cout << z << endl;
        }
    }
    return 0;
}

int treeTraversing() {

    typedef tuple<int, double, int, string> recordSubType;
    typedef key<0, 1, 2> keySubType;
    typedef Record<recordSubType, keySubType> Record;

    IteratorRecord<recordSubType, keySubType> curNode;
    stack<IteratorRecord<recordSubType, keySubType>> st;
    curNode.level = curNode.treeNumber = 0;
    TextFileSource<Record> fileReader("../input.txt");
    Record z = *fileReader.readRecord(), zBuf = z;
    int maxLevel = 4;

    int k = 0;
    while (true) {
        while (curNode.level <= maxLevel) {
            curNode.value = z;
            st.push(curNode);
            ++curNode.level;
            curNode.treeNumber = 1;
        }

        if (st.empty()) {
            break;
        }

        curNode = st.top();
        st.pop();
        if (curNode.level == maxLevel) {
            //print info about the leaf
            cout << "Leaf: " << curNode << endl;

            if (fileReader.hasNext()) {
                zBuf = *fileReader.readRecord();
                k = 1;
                customCompare(z.fields, zBuf.fields, k);
            } else {
                k = -1;
            }

            if (k == maxLevel) {
                z = zBuf;
                ++curNode.treeNumber;
            } else {
                curNode.level = maxLevel + 1;
            }
        } else {
            //print info about the inner node
            cout << "Inner node: " << "Level=" << curNode.level << " treeNumber=" << curNode.treeNumber << endl;
            if (curNode.level == k) {
                z = zBuf;
                ++curNode.treeNumber;
            } else {
                curNode.level = maxLevel + 1;
            }
        }
    }

    return 0;
}

int main() {
    typedef tuple<int, int, string, string, int> tupleSubType;
    typedef key<0, 1, 2, 3> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    IteratorRecord<tupleSubType, keySubType> curNode;
    stack<IteratorRecord<tupleSubType, keySubType>> st;
    TextFileSource<RecordType> fileReader("../input1.txt");

    Tree<TextFileSource<RecordType>> tree(fileReader);
    auto it = tree.begin();
    while (it.hasNext()) {
        cout << *it << ", depth=" << it.getDepth() << ", isLeaf=" << it.isLeaf() << ", 1st field equals "
             << it.getField<1>() << endl;
        ++it;
    }

    cout << "___________Second option of using iter:_________" << endl;

    for(auto iter = tree.begin(); iter != tree.end(); ++iter) {
        cout << *iter << ", depth=" << iter.getDepth() << ", isLeaf=" << iter.isLeaf() << ", 1st field equals "
             << iter.getField<1>() << endl;
    }

    cout << "____Condition test_____" << endl;
    conditionTest();
    cout << "______Tree traversing call______" << endl;
    treeTraversing();
    return 0;
}