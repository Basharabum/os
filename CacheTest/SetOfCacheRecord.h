#include "stdafx.h"
#include <thread>
#include <mutex>

//�����: ������ ������� ����
class SetOfCacheRecord {

	string cacheSetNumber;              //����� ������ � ��������
	int currentcacheRecordsCount;	   //������� ���������� ������� � ������
	int cacheRecordsCount;			   //������������ ���������� ������� � ������
	deque<CacheRecord*> dequeOfCacheRecords;//������� �� ������� ������
	thread* threadArray; //������ �������
	bool recordFound; //������ ������� � ������


public:

	SetOfCacheRecord(){ cacheSetNumber = ""; currentcacheRecordsCount = 0; cacheRecordsCount = 0; /*arrayOfCacheRecords = 0; */ recordFound = false; }
	SetOfCacheRecord(string thecacheSetNumber, int thecurrentcacheRecordsCount, int thecacheRecordsCount, bool theRecordFound){

		cacheSetNumber = thecacheSetNumber;
		currentcacheRecordsCount = thecurrentcacheRecordsCount;
		cacheRecordsCount = thecacheRecordsCount;
		recordFound = theRecordFound;

	}

	void setCacheSetNumber(string theNumber){ cacheSetNumber = theNumber; }
	void setCurrentCacheRecordsCount(int theCurrentCacheRecordsCount){ currentcacheRecordsCount = theCurrentCacheRecordsCount; }
	void setCacheRecordsCount(int theRecordsCount){ cacheRecordsCount = theRecordsCount; }
	void setRecordFound(bool theRecordFound){ recordFound = theRecordFound; }

	string getcacheSetNumber(){ return cacheSetNumber; }
	int getcurrentcacheRecordsCount(){ return currentcacheRecordsCount; }
	int getcacheRecordsCount(){ return cacheRecordsCount; }
	bool getRecordFound(){ return recordFound; }

	deque<CacheRecord*> getDeque(){ return dequeOfCacheRecords; }
	CacheRecord* getElemOfDeque(int index){ return dequeOfCacheRecords[index]; }

	//��������� ������ � ������
	bool addAddressToGroup(string tag) {

		if (currentcacheRecordsCount < cacheRecordsCount){ //���� ���� ������ ������ � ������
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

	std::mutex recordFound_mutex;

	//������� ������, ������������ ������� ��� ������ � ����� � ������
	void findInThreadsIndex(string tag, int index) {
		
		if (dequeOfCacheRecords[index]->getTag() == tag) {
			recordFound = true;
			dequeOfCacheRecords[index]->setReferBit(1);

		}
		
	}
	
	//�������, ����������� ������� ���� ������ � ����
	bool findAddressInCache(string tag) {

		bool result = false;
		
	/*	thread* threadArray = new thread[currentcacheRecordsCount]; //������ ������� ��� ������ ���� � ������
		
		for (int i = 0; i < currentcacheRecordsCount; i++) {
			threadArray[i] = thread(&SetOfCacheRecord::findInThreadsIndex, this, tag, i);
		}

		for (int i = 0; i < currentcacheRecordsCount; i++) {

			if (threadArray[i].joinable()) {
				threadArray[i].join();
			}
		}

		result = recordFound;*/
		for (int i = 0; i < dequeOfCacheRecords.size();i++)
		if (dequeOfCacheRecords[i]->getTag() == tag) {
			recordFound = true;
			dequeOfCacheRecords[i]->setReferBit(1);
			return true;
		}
		
		//recordFound = false;
		return false;
	    //return result;

	}
};

