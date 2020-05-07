#include "Record.h"
#include "TextFileSource.h"
#include "DataSourceStrategy.h"
#include "Iterator.h"
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

    friend ostream& operator<< (ostream& os, IteratorRecord const& rec) {
        os << "Level=" << rec.level << ", treeNumber=" << rec.treeNumber << ", value={" << rec.value << "}";
        return os;
    }
};

int main() {

    typedef tuple<int, int, string, string, int> tupleSubType;
    typedef key<0, 1, 2, 3> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    IteratorRecord<tupleSubType, keySubType> curNode;
    stack<IteratorRecord<tupleSubType, keySubType>> st;
    curNode.level = curNode.treeNumber = 0;
    TextFileSource<RecordType> fileReader("../input1.txt");
    Record<tupleSubType, keySubType> z = fileReader.readRecord(), zBuf = z;
    int maxLevel = 5;

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
                zBuf = fileReader.readRecord();
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

int treeOrder() {

    typedef tuple<int, double, int, string> recordSubType;
    typedef key<0, 1, 2> keySubType;
    typedef Record<recordSubType, keySubType> Record;

    IteratorRecord<recordSubType, keySubType> curNode;
    stack<IteratorRecord<recordSubType, keySubType>> st;
    curNode.level = curNode.treeNumber = 0;
    TextFileSource<Record> fileReader("../input.txt");
    Record z = fileReader.readRecord(), zBuf = z;
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
                zBuf = fileReader.readRecord();
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