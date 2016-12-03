#include "stdafx.h"
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>


// ласс: √руппа записей кэша
class SetOfCacheRecord {

	string cacheSetNumber;              //номер группы с запис€ми
	int currentcacheRecordsCount;	   //текущее количество записей в группе
	int cacheRecordsCount;			   //максимальное количество записей в группе
	deque<CacheRecord*> dequeOfCacheRecords;//ќчередь из записей группы
	std::vector<std::thread> workers;
	bool recordFound; //запись найдена в группе
	

public:

	SetOfCacheRecord() {
		cacheSetNumber = ""; currentcacheRecordsCount = 0; cacheRecordsCount = 0; /*arrayOfCacheRecords = 0; */ recordFound = false;

	}
	SetOfCacheRecord(string thecacheSetNumber, int thecurrentcacheRecordsCount, int thecacheRecordsCount, bool theRecordFound ) {

		cacheSetNumber = thecacheSetNumber;
		currentcacheRecordsCount = thecurrentcacheRecordsCount;
		cacheRecordsCount = thecacheRecordsCount;
		recordFound = theRecordFound;
	}

	void setCacheSetNumber(string theNumber) { cacheSetNumber = theNumber; }
	void setCurrentCacheRecordsCount(int theCurrentCacheRecordsCount) { currentcacheRecordsCount = theCurrentCacheRecordsCount; }
	void setCacheRecordsCount(int theRecordsCount) { cacheRecordsCount = theRecordsCount; }
	void setRecordFound(bool theRecordFound) { recordFound = theRecordFound; }
	
	string getcacheSetNumber() { return cacheSetNumber; }
	int getcurrentcacheRecordsCount() { return currentcacheRecordsCount; }
	int getcacheRecordsCount() { return cacheRecordsCount; }
	bool getRecordFound() { return recordFound; }


	deque<CacheRecord*> getDeque() { return dequeOfCacheRecords; }
	CacheRecord* getElemOfDeque(int index) { return dequeOfCacheRecords[index]; }

	//добавл€ет запись в группу
	bool addAddressToGroup(string tag) {

		if (currentcacheRecordsCount < cacheRecordsCount) { //если есть пустые записи в группе
			CacheRecord* cacheRecord = new CacheRecord(tag, 0);
			dequeOfCacheRecords.push_back(cacheRecord);
			currentcacheRecordsCount++;
			return true;
		}
		else {

			if (dequeOfCacheRecords.front()->getReferBit() == 0)
			{
				dequeOfCacheRecords.pop_front();

				CacheRecord* cacheRecord = new CacheRecord(tag, 0);
				dequeOfCacheRecords.push_back(cacheRecord);
				return true;
			}
			else {
				CacheRecord* record = dequeOfCacheRecords.front();
				dequeOfCacheRecords.pop_front();
				dequeOfCacheRecords.push_back(record);
				dequeOfCacheRecords.back()->setReferBit(0);
				addAddressToGroup(tag);
				return true;
			}

		}
		return false;
	}
	
	//‘ункци€ потока, сравнивающа€ текущий тег адреса с тегом в группе
	void findInThreadsIndex(string tag, int index) {

		if (dequeOfCacheRecords[index]->getTag() == tag) {
			recordFound = true;
			dequeOfCacheRecords[index]->setReferBit(1);

			cout << "поиск тега" << tag << " " << endl;
		}

	}

	//‘ункци€, провер€юща€ наличие тега адреса в кэше
	bool findAddressInCache(string tag) {

		for (int i = 0; i < dequeOfCacheRecords.size(); i++)
			if (dequeOfCacheRecords[i]->getTag() == tag) {
				recordFound = true;
				dequeOfCacheRecords[i]->setReferBit(1);
				return true;
			}

	 return false;// normal method
	
	}
	//‘ункци€, провер€юща€ наличие тега адреса в кэше с помощью потоков
	bool findAddressInCacheWithThreads(string tag) {

		bool result = false;

		if (dequeOfCacheRecords.size() == 0)
			return false;

		currentcacheRecordsCount = dequeOfCacheRecords.size();

		for (int i = 0; i < currentcacheRecordsCount; i++) {
			workers.push_back(std::thread(&SetOfCacheRecord::findInThreadsIndex, this, tag, i));
		}

		result = recordFound;
		recordFound = false;
		return result;

	}
};

