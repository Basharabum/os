#include "stdafx.h"

//Класс Запись кэша
class CacheRecord
{
	string tag;
	int referBit; //бит обращений
	

public:

	CacheRecord(){ tag = ""; referBit = 0; }
	CacheRecord(string &theTag, int theReferBit) : tag(theTag), referBit(theReferBit) {};
	CacheRecord(CacheRecord &record) {
		tag = record.getTag();
		referBit = record.getReferBit();
	}
	void setTag(string theTag){ tag = theTag; }
	void setReferBit(int theReferBit){ referBit = theReferBit; }
	
	string getTag(){ return tag; }
	int getReferBit(){ return referBit; }
	

};