#ifndef LOG_TREE_OLD_CPP
#define LOG_TREE_OLD_CPP

#include <stack>
#include <iostream>
#include "Record.h"
#include "TextFileSource.h"

const string DELIMITER = "###################";

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

int treeTraversing() {

    cout << DELIMITER << "Tree traversing from input.txt started." << DELIMITER << endl;

    typedef tuple<int, double, int, string> recordSubType;
    typedef key<0, 1, 2> keySubType;
    typedef Record<recordSubType, keySubType> RecordType;

    IteratorRecord<recordSubType, keySubType> curNode;
    stack<IteratorRecord<recordSubType, keySubType>> st;
    curNode.level = curNode.treeNumber = 0;
    TextFileSource<RecordType> fileReader("input/input.txt");
    RecordType z = *fileReader.readRecord(), zBuf = z;
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

    cout << DELIMITER << "Tree traversing from input.txt finished." << DELIMITER << endl;

    return 0;
}

#endif