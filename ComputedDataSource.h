
#include "Record.h"
#include <vector>

template<typename Record, typename InputDataSource, typename ComposeCondition, typename RecordComposer>
class ComputedDataSource;

template<typename InputDataSource, typename ComposeCondition, typename RecordComposer, typename... Types, int... Keys>
class ComputedDataSource<Record<tuple<Types...>, key<Keys...>>, InputDataSource, ComposeCondition, RecordComposer> :
    public DataSourceStrategy<Record<tuple<Types...>, key<Keys...>>> {

public:
    typedef Record<tuple<Types...>, key<Keys...>> RecordType;  

private:
    bool isHasNext = true;
    RecordType *currentRecord = nullptr;
    RecordType *nextRecord = nullptr;
    InputDataSource& inputDataSource;
    ComposeCondition& composeCondition;
    RecordComposer& composer;

public:

    explicit ComputedDataSource(InputDataSource& _inputDataSource, ComposeCondition& _composeCondition, RecordComposer& _composer)
        : inputDataSource(_inputDataSource), composeCondition(_composeCondition), composer(_composer) {
        
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
        if (!isHasNext) {
            nextRecord = nullptr;
            return currentRecord;
        }

        // Возьмём текущую запись из предоставленного источника данных.
        auto curRecord = inputDataSource.getCurrentRecord();
        // Добавим текущую запись в текущий класс эквивалентности.
        vector<decltype(curRecord)> recordsToCompose;
        recordsToCompose.push_back(curRecord);
        
        // Пока есть записи и они принадлежат текущему классу эквивалентности.
        while (inputDataSource.hasNext() && composeCondition(curRecord, inputDataSource.getNextRecord())) {
            recordsToCompose.push_back(inputDataSource.readRecord());
        }
        // Преобразуем записи класса эквивалентности в результирующую.
        nextRecord = composer(&recordsToCompose);
        
        if (inputDataSource.hasNext()) {
            inputDataSource.readRecord();
        } else {
            isHasNext = false;
        }

        return currentRecord;
    }

    bool hasNext() {

        return nextRecord != nullptr;
    }

    DataSourceStrategy<RecordType>* makeCopy() {

        return this;
    }
};