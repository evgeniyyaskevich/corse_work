#include "Record.h"
#include "TextFileSource.h"
#include "DataSourceStrategy.h"
#include "Tree.h"
#include "LogTreeTraversing.h"
#include "ComputedDataSource.h"
#include "StreamedDataSource.h"
#include <iostream>
#include <fstream>
#include <stack>
#include <sstream>
#include <vector>
#include <assert.h>

using namespace std;

void printInfo(string info) {
    cout << "###################" << info << "###################" << endl;
}

// typedef tuple<int, int, string, string, int> tupleSubType;
// typedef key<0, 1, 2, 3> keySubType;
// typedef Record<tupleSubType, keySubType> RecordType;

typedef tuple<int, string, string, int> tupleSubType;
typedef key<0, 1, 2> keySubType;
typedef Record<tupleSubType, keySubType> RecordType;

void iteratorUsing() {

    typedef tuple<int, string, string, int> tupleSubType;
    typedef key<0, 1, 2> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    printInfo("input1.txt traversing by iterator started.");

    TextFileSource<RecordType> fileReader("input/input1.txt");
    Tree<TextFileSource<RecordType>> tree(fileReader);

    // Способ использования итератора 1:
    auto it = tree.begin();
    while (it.hasNext()) {
        cout << *it << ", depth=" << it.getDepth();
        cout << ", isLeaf=" << it.isLeaf();
        cout << ", groupNumber "<< it.getField<0>() << endl;
        ++it;
    }

    // Способ использования итератора 2:
    for (auto iter = tree.begin(); iter != tree.end(); ++iter) {
        cout << *iter << ", depth=" << iter.getDepth();
        cout << ", isLeaf=" << iter.isLeaf();
        cout << ", groupNumber "<< iter.getField<0>() << endl;
    }

    printInfo("input1.txt traversing by iterator finished.");
}

void conditionTest() {

    printInfo("ConditionTest started.");

    // Объект источника данных, параметризованный типом записи, получающий на вход имя текстового файла для чтения.
    TextFileSource<RecordType> fileReader("input/input1.txt");

    // Функциональные объкты, которым должны удовлетворять определённые поля входной записи
    auto field0Cond = [](int courseNumber) { return courseNumber < 3; };
    auto field2Cond = [](string subjectValue) { return subjectValue != "MA"; };
    auto field3Cond = [](int markValue) { return markValue < 7; };

    // Формирование кортежа условий, накладываемых на конкретные поля записи. Элемент кортежа: номер записи, условие.
    auto condTuple = make_tuple(make_fieldCondition<0, decltype(field0Cond)>(field0Cond),
                                make_fieldCondition<2, decltype(field2Cond)>(field2Cond),
                                make_fieldCondition<3, decltype(field3Cond)>(field3Cond));

    vector<RecordType *> records;
    // Чтение записей из источника данных и отсеивание удовлетворяющих условию.
    while (fileReader.hasNext()) {
        if (isCondition(condTuple, fileReader.readRecord()->fields)) {
            records.push_back(fileReader.getCurrentRecord());
        }
    }

    // Проверка полученных результатов
    assert(records.size() == 1);
    RecordType::Tuple fields = records.at(0)->fields;
    assert(get<0>(fields) == 1);
    assert(get<1>(fields) == "Pashkevich");
    assert(get<2>(fields) == "GA");
    assert(get<3>(fields) == 10);

    printInfo("ConditionTest finished.");
}

void treeFilterTest() {

    printInfo("Filter test started.");

    TextFileSource<RecordType> fileReader("input/input1.txt");

    auto field0Cond = [](int courseNumber) { return courseNumber < 3; };
    auto field2Cond = [](string subjectValue) { return subjectValue != "MA"; };
    auto field3Cond = [](int markValue) { return markValue < 7; };

    // Создадим кортеж условий для полей записи
    // Имеем следующий интерфейс: make_fieldCondition<номер поля, тип функционального объекта>(функц. объект)
    auto condTuple = make_tuple(make_fieldCondition<0, decltype(field0Cond)>(field0Cond),
                                make_fieldCondition<2, decltype(field2Cond)>(field2Cond),
                                make_fieldCondition<3, decltype(field3Cond)>(field3Cond));

    FilteringPolicy<RecordType, decltype(condTuple)> filter(condTuple);

    Tree<TextFileSource<RecordType>, decltype(filter)> tree(fileReader, filter);
    for (auto iter = tree.begin(); iter != tree.end(); ++iter) {
            assert(iter.getField<0>() < 3);
            assert(iter.getField<2>() != "MA");
            assert(iter.getField<3>() < 7);
            cout << *iter << ", depth=" << iter.getDepth();
            cout << ", isLeaf=" << iter.isLeaf();
            cout << ", groupNumber "<< iter.getField<0>() << endl;
    }

    printInfo("Filter test finished.");
}

void dataSourceFilterTest() {

    printInfo("DataSourceFilter test started.");

    auto field0Cond = [](int courseNumber) { return courseNumber < 3; };
    auto field2Cond = [](string subjectValue) { return subjectValue != "MA"; };
    auto field3Cond = [](int markValue) { return markValue < 7; };

    // Создадим кортеж условий для полей записи
    // Имеем следующий интерфейс: make_fieldCondition<номер поля, тип функционального объекта>(функц. объект)
    auto condTuple = make_tuple(make_fieldCondition<0, decltype(field0Cond)>(field0Cond),
                                make_fieldCondition<2, decltype(field2Cond)>(field2Cond),
                                make_fieldCondition<3, decltype(field3Cond)>(field3Cond));

    FilteringPolicy<RecordType, decltype(condTuple)> filter1(condTuple);
    TextFileSource<RecordType, decltype(filter1)> reader("input/input1.txt", filter1);
    Tree<TextFileSource<RecordType, decltype(filter1)>, decltype(filter1)> tree(reader, filter1);

    for (auto iter = tree.begin(); iter != tree.end(); ++iter) {
        assert(iter.getField<0>() < 3);
        assert(iter.getField<2>() != "MA");
        assert(iter.getField<3>() < 7);
        cout << *iter << ", depth=" << iter.getDepth();
        cout << ", isLeaf=" << iter.isLeaf();
        cout << ", groupNumber " << iter.getField<1>() << endl;
    }

    printInfo("DataSourceFilter test finished.");
}

void allLevelSumExample() {

    typedef tuple<int, int, int, int> tupleSubType;
    typedef key<0, 1, 2> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    TextFileSource<RecordType> fileReader("input/input3.txt");
    Tree<TextFileSource<RecordType>> tree(fileReader);

    // Создадим массив для хранения суммы на каждом уровне.
    int sum[3]{0, 0, 0};
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        if (it.isLeaf()) { 
            // Получим глубину текущей вершины в дереве (номер уровня).
            int i = it.getDepth();
            // Прибавим значение поля к сумме на нужном уровне
            sum[i - 1] += it.getField<3>();
            cout << "Leaf: " << *it << endl;
        } else {
            int i = it.getDepth();
            cout << "Level=" << it.getDepth() << ", key=" << (*it).keyTuple() << ", sum=" << sum[i] << endl;
            // Прибавим к сумме на уровне выше, сумму предыдущего уровня.
            if (i > 0) {
                sum[i - 1] += sum[i];
            }
            // Обнулим значение на текущем уровне для последующих подсчётов.
            sum[i] = 0;
        }
    }
}

void averageMark() {

    // Объявим тип записи, как демонстрировалось в других примерах.
    typedef tuple<int, string, string, int> tupleSubType;
    typedef key<0, 1, 2> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    // Опишем источник данных.
    TextFileSource<RecordType> fileReader("input/input4.txt");
    //Создадим объект класса Tree.
    Tree<TextFileSource<RecordType>> tree(fileReader);

    int sum = 0, count = 0;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        if (it.isLeaf()) { //Если вершина - лист
            // Прибавим значение оценки(поле номер 3) в общую сумму
            sum += it.getField<3>();
            // Увеличим счётчик количества оценок.
            ++count;
        } else if (it.getDepth() == 2) { // Если глубина текущей вершины == 2 (уровень фамилии студента).
            // Выведем фамилию студента(поле номер 1) и средний балл.
            cout << it.getField<1>() << " - " << (double) sum / count << endl;
            // Сбросим значения счётчиков, ибо следующие вершины будут относится к следующему студенту.
            sum = count = 0;
        }
    }
}

void allLevelSumExampleWithComputedSource() {

    typedef tuple<int, int, int, int, int> tupleSubType;
    typedef key<0, 1, 2, 3> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    typedef tuple<int, int, int, int> OutTupleSubType;
    typedef key<0, 1, 2> OutKeySubType;
    typedef Record<OutTupleSubType, OutKeySubType> OutRecordType;

    TextFileSource<RecordType> fileReader("input/input5.txt");

    auto composeCondition = [](auto r1, auto r2) {
        if (r1 == nullptr || r2 == nullptr) {
            return true;
        } else {
            auto r1Fields = r1->fields;
            auto r2Fields = r2->fields;
            return get<0>(r1Fields) == get<0>(r2Fields)
                    && get<1>(r1Fields) == get<1>(r2Fields)
                    && get<2>(r1Fields) == get<2>(r2Fields);
        }
    };
    
    auto composer = [] (vector<RecordType*>* records) {

        auto sum = 0;
        for (auto r : *records) {
            sum += get<4>(r->fields);
        }

        auto fields = records->at(0)->fields;
        auto resultTuple = make_tuple(
            get<0>(fields),
            get<1>(fields),
            get<2>(fields),
            sum
        );

        return new OutRecordType(resultTuple);
    };

    ComputedDataSource<OutRecordType, decltype(fileReader),
         decltype(composeCondition), decltype(composer)> source(fileReader, composeCondition, composer);

    Tree<DataSourceStrategy<OutRecordType>> tree(source);

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

void allLevelSumExampleWithChainedComputedSource() {

    typedef tuple<int, int, int, int, int> tupleSubType;
    typedef key<0, 1, 2, 3> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    typedef tuple<int, int, int, int> OutTupleSubType;
    typedef key<0, 1, 2> OutKeySubType;
    typedef Record<OutTupleSubType, OutKeySubType> OutRecordType;

    TextFileSource<RecordType> fileReader("input/input5.txt");

    auto composeCondition = [](auto r1, auto r2) {
        if (r1 == nullptr || r2 == nullptr) {
            return true;
        } else {
            auto r1Fields = r1->fields;
            auto r2Fields = r2->fields;
            return get<0>(r1Fields) == get<0>(r2Fields)
                    && get<1>(r1Fields) == get<1>(r2Fields)
                    && get<2>(r1Fields) == get<2>(r2Fields);
        }
    };
    
    auto composer = [] (vector<RecordType*>* records) {

        auto sum = 0;
        for (auto r : *records) {
            sum += get<4>(r->fields);
        }

        auto fields = records->at(0)->fields;
        auto resultTuple = make_tuple(
            get<0>(fields),
            get<1>(fields),
            get<2>(fields),
            sum
        );

        return new OutRecordType(resultTuple);
    };

    ComputedDataSource<OutRecordType, decltype(fileReader),
         decltype(composeCondition), decltype(composer)> source(fileReader, composeCondition, composer);

    auto composeCondition1 = [](auto r1, auto r2) {
        return false;
    };

    auto composer1 = [] (vector<OutRecordType*>* records) {
        auto fields = records->at(0)->fields;
        auto resultTuple = make_tuple(
            get<0>(fields),
            get<1>(fields),
            get<2>(fields),
            get<3>(fields) * 10
        );

        return new OutRecordType(resultTuple);
    };

    ComputedDataSource<OutRecordType, decltype(source),
         decltype(composeCondition1), decltype(composer1)> source1(source, composeCondition1, composer1);

    ComputedDataSource<OutRecordType, decltype(source1),
         decltype(composeCondition1), decltype(composer1)> source2(source1, composeCondition1, composer1);

    Tree<DataSourceStrategy<OutRecordType>> tree(source);

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

void allLevelSumExampleWithStreamedSource() {

    typedef tuple<int, int, int, int, int> tupleSubType;
    typedef key<0, 1, 2, 3> keySubType;
    typedef Record<tupleSubType, keySubType> RecordType;

    typedef tuple<int, int, int, int> OutTupleSubType;
    typedef key<0, 1, 2> OutKeySubType;
    typedef Record<OutTupleSubType, OutKeySubType> OutRecordType;

    TextFileSource<RecordType> fileReader("input/input5.txt");

    auto composeCondition = [] (auto r1, auto r2) {

        if (r1 == nullptr || r2 == nullptr) {
            return true;
        } else {
            auto r1Fields = r1->fields;
            auto r2Fields = r2->fields;
            return get<0>(r1Fields) == get<0>(r2Fields)
                    && get<1>(r1Fields) == get<1>(r2Fields)
                    && get<2>(r1Fields) == get<2>(r2Fields);
        }
    };
    
    auto composer = [] (vector<RecordType*>* records) {

        auto sum = 0;
        for (auto r : *records) {
            sum += get<4>(r->fields);
        }

        auto fields = records->at(0)->fields;
        auto resultTuple = make_tuple(
            get<0>(fields),
            get<1>(fields),
            get<2>(fields),
            sum
        );

        return new OutRecordType(resultTuple);
    };

    // ComputedDataSource<OutRecordType, decltype(fileReader),
    //      decltype(composeCondition), decltype(composer)> source(fileReader, composeCondition, composer);

    auto composeCondition1 = [](auto r1, auto r2) {
        return false;
    };

    auto composer1 = [] (vector<OutRecordType*>* records) {
        auto fields = records->at(0)->fields;
        auto resultTuple = make_tuple(
            get<0>(fields),
            get<1>(fields),
            get<2>(fields),
            get<3>(fields) * 10
        );

        return new OutRecordType(resultTuple);
    };

    // ComputedDataSource<OutRecordType, decltype(source),
    //      decltype(composeCondition1), decltype(composer1)> source1(source, composeCondition1, composer1);

    // ComputedDataSource<OutRecordType, decltype(source1),
    //      decltype(composeCondition1), decltype(composer1)> source2(source1, composeCondition1, composer1);

    auto cDs = map(fileReader, composeCondition, composer, OutRecordType());
    auto cDs1 = map(cDs, composeCondition1, composer1, OutRecordType());
    using DsOutRecordType = typename remove_reference<decltype(cDs1)>::type::RecordType;
    Tree<DataSourceStrategy<DsOutRecordType>> tree(cDs1);

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

int main() {

    // treeFilterTest();
    // dataSourceFilterTest();
    // conditionTest();
    // treeTraversing();
    //iteratorUsing();

    //allLevelSumExample();
    //allLevelSumExampleWithComputedSource();
    //allLevelSumExampleWithChainedComputedSource();
    allLevelSumExampleWithStreamedSource();
    // averageMark();

    return 0;
}