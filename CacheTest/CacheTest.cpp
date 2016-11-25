#include "stdafx.h"
#include <ctime>
#include <deque>

#include "CacheRecord.h"
#include "SetOfCacheRecord.h"

#define DEBUG 1						//������ ����������� ������ � ���� result.txt
#define SAVE_CODE_TO_FILE 0	//��������� ��������������� ������������������ ����� ���� � ���� codegenerator.txt
#define LOAD_CODE_FROM_FILE 1		//��������� ������������������ ����� ���� �� ����� codegenerator.txt

#define MAIN_MEMORY_SIZE 1024		//������ �������� ������
#define CACHE_SIZE 128				//����� ������� � ����
#define CACHE_SETS_COUNT 16			//����� �����

#define SPATIAL_LOCALITY_LENGTH 0	//����� ���������������� �����������

#define LINEAR_CODE_PERCENT 24		//������� ��������� ����
#define LINEAR_CODE_DURATION 20     //������� ����������������� ��������� ���� (� ���������)

#define CYCLIC_CODE_PERCENT 56		//������� ������������ ����
#define CYCLIC_ITERATION_COUNT 10   //������� ���������� �������� � �����
#define CYCLIC_INSTRUCTION_COUNT 5  //������� ���������� ���������� � �����

#define FUNCTION_CODE_PERCENT 20    //������� ���� � ��������
#define FUNCTION_INSTRUCTION_COUNT 20 //������� ���������� ���������� � �������

//������� ���������� ������� ������ ����� 
int getPowerOfTwo(int num) {
	int i = 0;
	while (num % 2 == 0) {
		num /= 2;
		i++;
	}
	if (num == 1)
		return i;
	else
		return -1;
}

//������� ��������� ������� ������ � ���;
bool CheckCache(string address, SetOfCacheRecord* arrayofSets) {

	bool inCache = false;
	int numberGroupRankIndex = address.length() - getPowerOfTwo(CACHE_SETS_COUNT);
	string numberGroupRankStr = address.substr(numberGroupRankIndex, getPowerOfTwo(CACHE_SETS_COUNT)); //��������� ������, ���������� ����� ������
	string addressTag = address.substr(0, address.length() - getPowerOfTwo(CACHE_SETS_COUNT)); //��������� ������, ���������� ���

	for (int i = 0; i < CACHE_SETS_COUNT; i++) {
		if (arrayofSets[i].getcacheSetNumber() == numberGroupRankStr) {
			inCache = arrayofSets[i].findAddressInCache(addressTag);
		}
	}
	return inCache;
}

//������� ��������� ����� � ���
bool AddToCache(string address, SetOfCacheRecord* arrayofSets) {

	int numberGroupRankIndex = address.length() - getPowerOfTwo(CACHE_SETS_COUNT);

	string numberGroupRankStr = address.substr(numberGroupRankIndex, getPowerOfTwo(CACHE_SETS_COUNT)); //��������� ������, ���������� ����� ������
	string addressTag = address.substr(0, address.length() - getPowerOfTwo(CACHE_SETS_COUNT)); //��������� ������, ���������� ���

	for (int i = 0; i < CACHE_SETS_COUNT; i++) {
		if (numberGroupRankStr == arrayofSets[i].getcacheSetNumber()) {
			return arrayofSets[i].addAddressToGroup(addressTag);
		}
	}
	return false;
}

//������� ��������� ����� �� ����������� ���� � ��������
string intToBinaryString(int intNumber) {

	char tmp;
	string binNumber;

	while (intNumber) {
		tmp = '0' + intNumber % 2;
		binNumber = tmp + binNumber;
		intNumber /= 2;

	}

	while (binNumber.length() < 10)
		binNumber = '0' + binNumber;

	return binNumber;
}


//������� ���������� �������� ���
int linCode(int currentAddress, SetOfCacheRecord* arrayofSets, ostream &fout,ostream &codefileout, int &CacheHitCounter, int &CheckCounter) {
	//��������� ����� ����������
	int randomNumber = rand() % LINEAR_CODE_DURATION + 1;
	
	if (DEBUG){
		fout << "-> �������� ��� � ����������� ����������: " << randomNumber << endl << endl;
		
	}
	if (SAVE_CODE_TO_FILE){
		codefileout << "L " << randomNumber << endl;
	}
	for (int i = 0; i < randomNumber; i++){

		string BinaryAddress = intToBinaryString(currentAddress);
		bool inCache = CheckCache(BinaryAddress, arrayofSets);
		
		if (DEBUG){
			fout << "    ��������: " << currentAddress << " " << intToBinaryString(currentAddress) << " ";
		}
		if (inCache == true) {
			CacheHitCounter++;
			CheckCounter++;
			if (DEBUG){
				fout << "���������" << endl;
			}
		}
		else {
			if (DEBUG){
				fout << "������" << " ";
			}
			CheckCounter++;
			if (AddToCache(BinaryAddress, arrayofSets)) {
				if (DEBUG){
					fout << " �������� � ���" << endl;
				}
				//���� ����� �������� � ���, �� �������� ��������� �� ��� SPATIAL_LOCALITY_LENGTH �������
				for (int i = 1; i < SPATIAL_LOCALITY_LENGTH + 1; i++) {
					BinaryAddress = intToBinaryString(currentAddress + i);
					if (AddToCache(BinaryAddress, arrayofSets)) {
						if (DEBUG){
							fout << "      ����� " << currentAddress + i << " " << BinaryAddress << " �������� � ��� ��������� �������� ���������������� �����������" << endl;
						}
					}
				}
			}
			else {
				if (DEBUG){
					fout << " �� �������� � ���" << endl;
				}
			}
		}
		currentAddress++;
		inCache = false;
	}
	if (DEBUG){
		fout << "<- ����� ��������� ����" << endl;
	}
	return currentAddress;
}

//������� ���������� ����������� ���
int cycleCode(int currentAddress, SetOfCacheRecord* arrayofSets, ostream &fout, ostream &codefileout, int &CacheHitCounter, int &CheckCounter) {

	int iterationCount = rand() % CYCLIC_ITERATION_COUNT + 1;		//��������� ���������� �������� �����
	int instructionCount = rand() % CYCLIC_INSTRUCTION_COUNT + 1;	//��������� ���������� ��������� �����

	if (DEBUG){
		fout << "-> ����������� ��� � ����������� ��������: " << iterationCount << endl;
		fout << "   � ����������� ����������: " << instructionCount << endl << endl;
	}
	if (SAVE_CODE_TO_FILE){
		codefileout << "C " << iterationCount << " " << instructionCount << endl;
	}
	int firstAddressInCycle = currentAddress;
	int lastAddressInCycle = currentAddress;
	bool inCache = false;

		
	for (int i = 0; i < iterationCount; i++){
		for (int j = 0; j < instructionCount; j++){

			string BinaryAddress = intToBinaryString(currentAddress);
			inCache = CheckCache(BinaryAddress, arrayofSets);

			if (DEBUG) {
				fout << "    ��������: " << currentAddress << " " << intToBinaryString(currentAddress) << " ";
			}
			if (inCache == true) {

				CacheHitCounter++;
				CheckCounter++;
				if (DEBUG){
					fout << "���������" << endl;
				}
			}
			else {
				if (DEBUG){
					fout << "������" << " ";
				}
				CheckCounter++;
				if (AddToCache(BinaryAddress, arrayofSets)) {
					if (DEBUG){
						fout << " �������� � ���" << endl;
					}
					for (int i = 1; i < SPATIAL_LOCALITY_LENGTH + 1; i++)
					{
						BinaryAddress = intToBinaryString(currentAddress + i);
						if (AddToCache(BinaryAddress, arrayofSets)) {
							if (DEBUG){
								fout << "      ����� " << currentAddress + i << " " << BinaryAddress << " �������� � ��� ��������� �������� ���������������� �����������" << endl;
							}
						}
					}
				}
				else {
					if (DEBUG){
						fout << " �� �������� � ���" << endl;
					}
				}
			}
			currentAddress++;
			inCache = false;
		}
		lastAddressInCycle = currentAddress;
		currentAddress = firstAddressInCycle;
		//fout << endl;
	}
	//fout << "<- ����� ������������ ����" << endl;
	return lastAddressInCycle;

}

//������� ���������� �������������� ���
int funcCode(int currentAddress, SetOfCacheRecord* arrayofSets, ostream &fout,ostream &codefileout, int &CacheHitCounter, int &CheckCounter){

	int endOfFunctionAddress = 80;
	bool linearCode = false;
	bool cycicCode = false;
	bool functionCode = false;
	int randomNumber = rand() % FUNCTION_INSTRUCTION_COUNT + 1;//���������� ���������� � �������
	int lastAddress = currentAddress + randomNumber;
	
	if (DEBUG){
		fout << "[ ������� � ����������� ����������: " << randomNumber << " func]" << endl << endl;
	}
	if (SAVE_CODE_TO_FILE) {
		codefileout << "F " << randomNumber << endl;
	}
	
	
	
	for (int i = 0; i < randomNumber; i++) {
		while (currentAddress < lastAddress) {
			//����������� ���� ����
			randomNumber = rand() % 100;

			if (randomNumber < LINEAR_CODE_PERCENT)
				linearCode = true;
			else if (randomNumber < LINEAR_CODE_PERCENT + CYCLIC_CODE_PERCENT)
				cycicCode = true;
			else
				functionCode = true;

			if (linearCode) {

				currentAddress = linCode(currentAddress, arrayofSets, fout,codefileout, CacheHitCounter, CheckCounter);
				linearCode = false; // ����� ��������� ����

				if (DEBUG){
					fout << endl << endl;
				}
			}
			//���� ��� ��������� �����������
			if (cycicCode) {

				currentAddress = cycleCode(currentAddress, arrayofSets, fout,codefileout, CacheHitCounter, CheckCounter);
				cycicCode = false; // ����� ������������ ����
				if (DEBUG){
					fout << endl << endl;
				}

			}
			//���� ��� ��������� ��������������
			if (functionCode) {

				int entryAddress = currentAddress; //����� ����� � �������
				int endOfFunctionAddress = funcCode(currentAddress, arrayofSets, fout,codefileout, CacheHitCounter, CheckCounter);
				currentAddress = entryAddress + 1;
				functionCode = false;
			}
		}
	}
	if (DEBUG){
		fout << "<- ����� �������:________________ " << randomNumber << endl << endl;
	}
	functionCode = false;
	return endOfFunctionAddress;

}

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(NULL));
	setlocale(LC_CTYPE, "Russian");
	ofstream codefileout;
	ifstream codefilein;
	ofstream fout("result.txt");
	if (SAVE_CODE_TO_FILE){
		//ofstream codefileout("codegenerator.txt");
		codefileout.open("codegenerator.txt");
	}

	if (LOAD_CODE_FROM_FILE) {
	
		codefilein.open("codegenerator.txt");
		string str;
		int countOfLinesInFile = 0;

		while (!codefilein.eof()) {
			getline(codefilein,str);
			countOfLinesInFile++;
		}
	//	cout << countOfLinesInFile << endl;
		//countOfLinesInFile = 509;
		string* arrayOfCodeTypes = new string[countOfLinesInFile];
		/*for (int i = 0; i < countOfLinesInFile; i++){
			arrayOfCodeTypes[i] = " ";
		}*/

		int iter = 0;
		codefilein.seekg(0, codefilein.beg);
		while (!codefilein.eof())
		{
			getline(codefilein,arrayOfCodeTypes[iter]);
			iter++;
		}

		for (int i = 0; i < countOfLinesInFile; i++){
			cout << arrayOfCodeTypes[i] << endl;
		
		}
	}

	if (DEBUG){

		fout << "������ �������� ������: " << MAIN_MEMORY_SIZE << endl;
		fout << "����� ������� � ����: " << CACHE_SIZE << endl;
		fout << "����� ���������������� �����������: " << SPATIAL_LOCALITY_LENGTH << endl;

		fout << "������� ��������� ����: " << LINEAR_CODE_PERCENT << endl;
		fout << "������� ����������������� ��������� ���� (� ���������): " << LINEAR_CODE_DURATION << endl;
		fout << "������� ������������ ����: " << CYCLIC_CODE_PERCENT << endl;
		fout << "������� ���������� �������� � �����: " << CYCLIC_ITERATION_COUNT << endl;
		fout << "������� ���������� ���������� � �����: " << CYCLIC_INSTRUCTION_COUNT << endl;
		fout << "������� ���� � ��������: " << FUNCTION_CODE_PERCENT << endl;

		fout << endl << endl;
	}

	int countOfRecordsInSet = CACHE_SIZE / CACHE_SETS_COUNT;
	int numberGroupRankIndex = 0;
	string numberGroupRankStr = "";
	string setnumber = "";

	SetOfCacheRecord* arrayofSets = new SetOfCacheRecord[CACHE_SETS_COUNT]; // ������ ����� � ����

	for (int i = 0; i < CACHE_SETS_COUNT; i++) {

		arrayofSets[i].setCacheRecordsCount(countOfRecordsInSet);
		arrayofSets[i].setRecordFound(false);

		setnumber = intToBinaryString(i);
		numberGroupRankIndex = (setnumber.length()) - getPowerOfTwo(CACHE_SETS_COUNT);
		numberGroupRankStr = setnumber.substr(numberGroupRankIndex, getPowerOfTwo(CACHE_SETS_COUNT));

		arrayofSets[i].setCacheSetNumber(numberGroupRankStr); // ����� ������
	}

	//����� ��� ����� ����
	bool linear�ode = false;
	bool cyclic�ode = false;
	bool functionsCode = false;

	int lastAddress = 0; //��������� �����
	int currentAddress = 0; //������� �������� ������
	bool inCache = false; //��������� ������ � ���
	int CacheHitCounter = 0; //������� ��������� � ���
	int CheckCounter = 0; //������� ��������
	int randomNumber = 0;
	int randomNumber2 = 0;

	char typeOfCode[7];
	while (currentAddress < MAIN_MEMORY_SIZE) {
		//����������� ���� ����

		if (LOAD_CODE_FROM_FILE) {
			codefilein.getline(typeOfCode, 7);
			//cout << typeOfCode << endl;
			if (typeOfCode[0] == 'L'){
				linear�ode = true;
				//cout << "�������� ���" << endl;
			}
			else if (typeOfCode[0] == 'C'){
				cyclic�ode = true;
				//cout << "����������� ���" << endl;
			}
			else
			{
				functionsCode = true;
				//cout << "�������������� ���" << endl;
			}
		
		}
		else {
			randomNumber = rand() % 100;

			if (randomNumber < LINEAR_CODE_PERCENT)
				linear�ode = true;
			else if (randomNumber < LINEAR_CODE_PERCENT + CYCLIC_CODE_PERCENT)
				cyclic�ode = true;
			else
				functionsCode = true;
		}
		//���� ��� ��������� ��������
		if (linear�ode) {

			currentAddress = linCode(currentAddress, arrayofSets, fout, codefileout,CacheHitCounter, CheckCounter);
			linear�ode = false; // ����� ��������� ����

			if (DEBUG){
				fout << endl << endl;
			}
		}

		//���� ��� ��������� �����������
		if (cyclic�ode) {

			currentAddress = cycleCode(currentAddress, arrayofSets, fout,codefileout, CacheHitCounter, CheckCounter);
			cyclic�ode = false; // ����� ������������ ����
			if (DEBUG){
				fout << endl << endl;
			}

		}

		if (functionsCode) {
			
			int entryAddress = currentAddress; //����� ����� � �������

			int endOfFunctionAddress = funcCode(currentAddress, arrayofSets, fout,codefileout, CacheHitCounter, CheckCounter);

			currentAddress = entryAddress + 1;
			functionsCode = false;
			if (DEBUG){
				fout << endl << endl;
			}
		}
	}

	if (DEBUG){
		fout << "���������� ���������: " << CacheHitCounter << endl << endl;
		fout << "������� ���������: " << (CacheHitCounter * 100) / CheckCounter << endl;
	}
	cout << "���������� ���������: " << CacheHitCounter << endl << endl;
	cout << "������� ���������: " << (CacheHitCounter * 100) / CheckCounter << endl;

	fout << "������ � ����: " << endl;

	for (int i = 0; i < CACHE_SETS_COUNT; i++){
		for (int j = 0; j < arrayofSets[i].getcurrentcacheRecordsCount(); j++){
			fout << arrayofSets[i].getElemOfDeque(j)->getTag() << arrayofSets[i].getcacheSetNumber() << endl;
		}
		fout << endl;
	}

    codefilein.close();
	return 0;
}

