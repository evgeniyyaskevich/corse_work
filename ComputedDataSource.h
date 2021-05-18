
#include "Record.h"

template<typename Record, typename InputDataSource, typename SameRecordComposer, typename DifRecordComposer>
class ComputedDataSource;

template<typename InputDataSource, typename SameRecordComposer, typename DifRecordComposer, typename... Types, int... Keys>
class ComputedDataSource<Record<tuple<Types...>, key<Keys...>>, InputDataSource, SameRecordComposer, DifRecordComposer> :
    public DataSourceStrategy<Record<tuple<Types...>, key<Keys...>>> {

public:
    typedef Record<tuple<Types...>, key<Keys...>> RecordType;  

private:
    bool isHasNext = true;
    RecordType *currentRecord = nullptr;
    RecordType *nextRecord = nullptr;
    InputDataSource& inputDataSource;
    SameRecordComposer& sameRecordComposer;
    DifRecordComposer& difRecordComposer;

public:

    explicit ComputedDataSource(InputDataSource& _inputDataSource, SameRecordComposer& _composer, DifRecordComposer& _composer1)
        : inputDataSource(_inputDataSource), sameRecordComposer(_composer), difRecordComposer(_composer1) {
        
        readRecord();
        readRecord();
    };

    RecordType *getCurrentRecord() {
        return currentRecord;
    }

    RecordType *getNextRecord() {

        return nextRecord;
    }

    RecordType *readRecord() {

        currentRecord = nextRecord;
        // Возьмём текущую и следующую записи из предыдущего источника данных.
        auto sourceCurRec = inputDataSource.getCurrentRecord();
        auto sourceNextRec = inputDataSource.hasNext() ? inputDataSource.readRecord() : nullptr;
        if (sourceNextRec == nullptr) {
            isHasNext = false;
        }
        // Вызовем функцию-преобразователь для двух записей.
        // Результат выполнения пара: (записи объединены или нет, результирующая запись).
        pair<bool, RecordType*> result = sameRecordComposer(sourceCurRec, sourceNextRec);
        // Пока в источниках есть ещё записи и предыдущие записи были объединены
        while (result.first && inputDataSource.hasNext()) {
            // Пробуем преобразовать полученную преобразованную запись с новой
            result = difRecordComposer(result.second, inputDataSource.readRecord());
        }
        nextRecord = result.second;
        return currentRecord;
    }

    bool hasNext() {
        return isHasNext;
    }

    DataSourceStrategy<RecordType>* makeCopy() {
        return this;
    }
};