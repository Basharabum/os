#include "stdafx.h"
#include <ctime>
#include <deque>

#include "CacheRecord.h"
#include "SetOfCacheRecord.h"

#define DEBUG 1						//запись результатов работы в файл result.txt
#define SAVE_CODE_TO_FILE 0	//сохранить сгенерированную последовательность видов кода в файл codegenerator.txt
#define LOAD_CODE_FROM_FILE 1		//загрузить последовательность видов кода из файла codegenerator.txt

#define MAIN_MEMORY_SIZE 1024		//размер основной памяти
#define CACHE_SIZE 128				//число записей в кэше
#define CACHE_SETS_COUNT 16			//число групп

#define SPATIAL_LOCALITY_LENGTH 0	//длина пространственной локальности

#define LINEAR_CODE_PERCENT 24		//процент линейного кода
#define LINEAR_CODE_DURATION 20     //средняя продолжительность линейного кода (в итерациях)

#define CYCLIC_CODE_PERCENT 56		//процент циклического кода
#define CYCLIC_ITERATION_COUNT 10   //среднее количество итераций в цикле
#define CYCLIC_INSTRUCTION_COUNT 5  //среднее количество инструкций в цикле

#define FUNCTION_CODE_PERCENT 20    //процент кода в функциях
#define FUNCTION_INSTRUCTION_COUNT 20 //среднее количество инструкций в функции

//Функция возвращает степень двойки числа 
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

//Функция проверяет наличие адреса в кэш;
bool CheckCache(string address, SetOfCacheRecord* arrayofSets) {

	bool inCache = false;
	int numberGroupRankIndex = address.length() - getPowerOfTwo(CACHE_SETS_COUNT);
	string numberGroupRankStr = address.substr(numberGroupRankIndex, getPowerOfTwo(CACHE_SETS_COUNT)); //подстрока адреса, содержащая номер группы
	string addressTag = address.substr(0, address.length() - getPowerOfTwo(CACHE_SETS_COUNT)); //подстрока адреса, содержащая тег

	for (int i = 0; i < CACHE_SETS_COUNT; i++) {
		if (arrayofSets[i].getcacheSetNumber() == numberGroupRankStr) {
			inCache = arrayofSets[i].findAddressInCache(addressTag);
		}
	}
	return inCache;
}

//Функция добавляет адрес в кэш
bool AddToCache(string address, SetOfCacheRecord* arrayofSets) {

	int numberGroupRankIndex = address.length() - getPowerOfTwo(CACHE_SETS_COUNT);

	string numberGroupRankStr = address.substr(numberGroupRankIndex, getPowerOfTwo(CACHE_SETS_COUNT)); //подстрока адреса, содержащая номер группы
	string addressTag = address.substr(0, address.length() - getPowerOfTwo(CACHE_SETS_COUNT)); //подстрока адреса, содержащая тег

	for (int i = 0; i < CACHE_SETS_COUNT; i++) {
		if (numberGroupRankStr == arrayofSets[i].getcacheSetNumber()) {
			return arrayofSets[i].addAddressToGroup(addressTag);
		}
	}
	return false;
}

//Функция переводит число из десятичного вида в двоичный
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


//Функция генерирует линейный код
int linCode(int currentAddress, SetOfCacheRecord* arrayofSets, ostream &fout,ostream &codefileout, int &CacheHitCounter, int &CheckCounter) {
	//генерация числа инструкций
	int randomNumber = rand() % LINEAR_CODE_DURATION + 1;
	
	if (DEBUG){
		fout << "-> Линейный код с количеством инструкций: " << randomNumber << endl << endl;
		
	}
	if (SAVE_CODE_TO_FILE){
		codefileout << "L " << randomNumber << endl;
	}
	for (int i = 0; i < randomNumber; i++){

		string BinaryAddress = intToBinaryString(currentAddress);
		bool inCache = CheckCache(BinaryAddress, arrayofSets);
		
		if (DEBUG){
			fout << "    проверка: " << currentAddress << " " << intToBinaryString(currentAddress) << " ";
		}
		if (inCache == true) {
			CacheHitCounter++;
			CheckCounter++;
			if (DEBUG){
				fout << "ПОПАДАНИЕ" << endl;
			}
		}
		else {
			if (DEBUG){
				fout << "ПРОМАХ" << " ";
			}
			CheckCounter++;
			if (AddToCache(BinaryAddress, arrayofSets)) {
				if (DEBUG){
					fout << " Добавлен в кэш" << endl;
				}
				//Если адрес добавлен в кэш, то добавить следующие за ним SPATIAL_LOCALITY_LENGTH адресов
				for (int i = 1; i < SPATIAL_LOCALITY_LENGTH + 1; i++) {
					BinaryAddress = intToBinaryString(currentAddress + i);
					if (AddToCache(BinaryAddress, arrayofSets)) {
						if (DEBUG){
							fout << "      Адрес " << currentAddress + i << " " << BinaryAddress << " Добавлен в кэш благодаря свойству пространственной локальности" << endl;
						}
					}
				}
			}
			else {
				if (DEBUG){
					fout << " НЕ добавлен в кэш" << endl;
				}
			}
		}
		currentAddress++;
		inCache = false;
	}
	if (DEBUG){
		fout << "<- Конец линейного кода" << endl;
	}
	return currentAddress;
}

//Функция генерирует циклический код
int cycleCode(int currentAddress, SetOfCacheRecord* arrayofSets, ostream &fout, ostream &codefileout, int &CacheHitCounter, int &CheckCounter) {

	int iterationCount = rand() % CYCLIC_ITERATION_COUNT + 1;		//генерация количества итераций цикла
	int instructionCount = rand() % CYCLIC_INSTRUCTION_COUNT + 1;	//генерация количества инструкий цикла

	if (DEBUG){
		fout << "-> Циклический код с количеством итераций: " << iterationCount << endl;
		fout << "   и количеством инструкций: " << instructionCount << endl << endl;
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
				fout << "    проверка: " << currentAddress << " " << intToBinaryString(currentAddress) << " ";
			}
			if (inCache == true) {

				CacheHitCounter++;
				CheckCounter++;
				if (DEBUG){
					fout << "ПОПАДАНИЕ" << endl;
				}
			}
			else {
				if (DEBUG){
					fout << "ПРОМАХ" << " ";
				}
				CheckCounter++;
				if (AddToCache(BinaryAddress, arrayofSets)) {
					if (DEBUG){
						fout << " Добавлен в кэш" << endl;
					}
					for (int i = 1; i < SPATIAL_LOCALITY_LENGTH + 1; i++)
					{
						BinaryAddress = intToBinaryString(currentAddress + i);
						if (AddToCache(BinaryAddress, arrayofSets)) {
							if (DEBUG){
								fout << "      Адрес " << currentAddress + i << " " << BinaryAddress << " Добавлен в кэш благодаря свойству пространственной локальности" << endl;
							}
						}
					}
				}
				else {
					if (DEBUG){
						fout << " НЕ добавлен в кэш" << endl;
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
	//fout << "<- Конец циклического кода" << endl;
	return lastAddressInCycle;

}

//Функция генерирует функциональный код
int funcCode(int currentAddress, SetOfCacheRecord* arrayofSets, ostream &fout,ostream &codefileout, int &CacheHitCounter, int &CheckCounter){

	int endOfFunctionAddress = 80;
	bool linearCode = false;
	bool cycicCode = false;
	bool functionCode = false;
	int randomNumber = rand() % FUNCTION_INSTRUCTION_COUNT + 1;//количество инструкций в функции
	int lastAddress = currentAddress + randomNumber;
	
	if (DEBUG){
		fout << "[ Функция с количеством инструкций: " << randomNumber << " func]" << endl << endl;
	}
	if (SAVE_CODE_TO_FILE) {
		codefileout << "F " << randomNumber << endl;
	}
	
	
	
	for (int i = 0; i < randomNumber; i++) {
		while (currentAddress < lastAddress) {
			//определение вида кода
			randomNumber = rand() % 100;

			if (randomNumber < LINEAR_CODE_PERCENT)
				linearCode = true;
			else if (randomNumber < LINEAR_CODE_PERCENT + CYCLIC_CODE_PERCENT)
				cycicCode = true;
			else
				functionCode = true;

			if (linearCode) {

				currentAddress = linCode(currentAddress, arrayofSets, fout,codefileout, CacheHitCounter, CheckCounter);
				linearCode = false; // конец линейного кода

				if (DEBUG){
					fout << endl << endl;
				}
			}
			//если код получился циклическим
			if (cycicCode) {

				currentAddress = cycleCode(currentAddress, arrayofSets, fout,codefileout, CacheHitCounter, CheckCounter);
				cycicCode = false; // конец циклического кода
				if (DEBUG){
					fout << endl << endl;
				}

			}
			//если код получился функциональным
			if (functionCode) {

				int entryAddress = currentAddress; //адрес входа в функцию
				int endOfFunctionAddress = funcCode(currentAddress, arrayofSets, fout,codefileout, CacheHitCounter, CheckCounter);
				currentAddress = entryAddress + 1;
				functionCode = false;
			}
		}
	}
	if (DEBUG){
		fout << "<- Конец функции:________________ " << randomNumber << endl << endl;
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

		fout << "размер основной памяти: " << MAIN_MEMORY_SIZE << endl;
		fout << "число записей в кэше: " << CACHE_SIZE << endl;
		fout << "длина пространственной локальности: " << SPATIAL_LOCALITY_LENGTH << endl;

		fout << "процент линейного кода: " << LINEAR_CODE_PERCENT << endl;
		fout << "средняя продолжительность линейного кода (в итерациях): " << LINEAR_CODE_DURATION << endl;
		fout << "процент циклического кода: " << CYCLIC_CODE_PERCENT << endl;
		fout << "среднее количество итераций в цикле: " << CYCLIC_ITERATION_COUNT << endl;
		fout << "среднее количество инструкций в цикле: " << CYCLIC_INSTRUCTION_COUNT << endl;
		fout << "процент кода в функциях: " << FUNCTION_CODE_PERCENT << endl;

		fout << endl << endl;
	}

	int countOfRecordsInSet = CACHE_SIZE / CACHE_SETS_COUNT;
	int numberGroupRankIndex = 0;
	string numberGroupRankStr = "";
	string setnumber = "";

	SetOfCacheRecord* arrayofSets = new SetOfCacheRecord[CACHE_SETS_COUNT]; // Массив групп в кэше

	for (int i = 0; i < CACHE_SETS_COUNT; i++) {

		arrayofSets[i].setCacheRecordsCount(countOfRecordsInSet);
		arrayofSets[i].setRecordFound(false);

		setnumber = intToBinaryString(i);
		numberGroupRankIndex = (setnumber.length()) - getPowerOfTwo(CACHE_SETS_COUNT);
		numberGroupRankStr = setnumber.substr(numberGroupRankIndex, getPowerOfTwo(CACHE_SETS_COUNT));

		arrayofSets[i].setCacheSetNumber(numberGroupRankStr); // номер группы
	}

	//флаги для видов кода
	bool linearСode = false;
	bool cyclicСode = false;
	bool functionsCode = false;

	int lastAddress = 0; //последний адрес
	int currentAddress = 0; //текущее значение адреса
	bool inCache = false; //попадание записи в кэш
	int CacheHitCounter = 0; //счетчик попаданий в кэш
	int CheckCounter = 0; //счетчик проверок
	int randomNumber = 0;
	int randomNumber2 = 0;

	char typeOfCode[7];
	while (currentAddress < MAIN_MEMORY_SIZE) {
		//определение вида кода

		if (LOAD_CODE_FROM_FILE) {
			codefilein.getline(typeOfCode, 7);
			//cout << typeOfCode << endl;
			if (typeOfCode[0] == 'L'){
				linearСode = true;
				//cout << "Линейный код" << endl;
			}
			else if (typeOfCode[0] == 'C'){
				cyclicСode = true;
				//cout << "Циклический код" << endl;
			}
			else
			{
				functionsCode = true;
				//cout << "Функциональный код" << endl;
			}
		
		}
		else {
			randomNumber = rand() % 100;

			if (randomNumber < LINEAR_CODE_PERCENT)
				linearСode = true;
			else if (randomNumber < LINEAR_CODE_PERCENT + CYCLIC_CODE_PERCENT)
				cyclicСode = true;
			else
				functionsCode = true;
		}
		//если код получился линейным
		if (linearСode) {

			currentAddress = linCode(currentAddress, arrayofSets, fout, codefileout,CacheHitCounter, CheckCounter);
			linearСode = false; // конец линейного кода

			if (DEBUG){
				fout << endl << endl;
			}
		}

		//если код получился циклическим
		if (cyclicСode) {

			currentAddress = cycleCode(currentAddress, arrayofSets, fout,codefileout, CacheHitCounter, CheckCounter);
			cyclicСode = false; // конец циклического кода
			if (DEBUG){
				fout << endl << endl;
			}

		}

		if (functionsCode) {
			
			int entryAddress = currentAddress; //адрес входа в функцию

			int endOfFunctionAddress = funcCode(currentAddress, arrayofSets, fout,codefileout, CacheHitCounter, CheckCounter);

			currentAddress = entryAddress + 1;
			functionsCode = false;
			if (DEBUG){
				fout << endl << endl;
			}
		}
	}

	if (DEBUG){
		fout << "Количество попаданий: " << CacheHitCounter << endl << endl;
		fout << "Процент попаданий: " << (CacheHitCounter * 100) / CheckCounter << endl;
	}
	cout << "Количество попаданий: " << CacheHitCounter << endl << endl;
	cout << "Процент попаданий: " << (CacheHitCounter * 100) / CheckCounter << endl;

	fout << "Адреса в Кэше: " << endl;

	for (int i = 0; i < CACHE_SETS_COUNT; i++){
		for (int j = 0; j < arrayofSets[i].getcurrentcacheRecordsCount(); j++){
			fout << arrayofSets[i].getElemOfDeque(j)->getTag() << arrayofSets[i].getcacheSetNumber() << endl;
		}
		fout << endl;
	}

    codefilein.close();
	return 0;
}

