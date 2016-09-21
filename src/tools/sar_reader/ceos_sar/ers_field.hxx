/*
 *  ers_field.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ERS_FIELD_HXX
#define ERS_FIELD_HXX

#include <string.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

class ERS_Field{
public:
	ERS_Field() 
	{}	
	
	ERS_Field(int n_id, int n_begin, int n_end, std::string n_format, std::string n_unit, std::string n_name)
	:	id(n_id),
	begin(n_begin), end(n_end),
	format(n_format),
	unit(n_unit),
	name(n_name)
	{}
	
	int id,
	begin, end;
	
	std::string format,unit,
	name;
};

bool isValid( char* data, unsigned int length){
	//A numeric value is invalid iff all character are ' ', '-', '9', 'E' or '.' 
	for (unsigned int i = 0; i< length; ++i){
		if ( data[i] != ' ' && data[i] != '-' && data[i] != '9'  && data[i] != 'E'  && data[i] != '.' )
			return true;
	}
	return false;
}

//get the string value of an ERS_Field
std::string getERSValue( char* data, ERS_Field & field){
	string result;
	unsigned int length = field.end - field.begin+1;
	unsigned int offset = field.begin-1;
	char * temp_char = new char[length];
	memcpy(temp_char, data+field.begin-1, length);
	
	//Char/String Type
	if (field.format.substr(0,1) == "A"){
		result = isValid(temp_char,length) ? string(temp_char,length) : "";
	}
	//Binary Short Type B1 ---ATTENTION TO LITTLE->BIG_ENDIAN CONVERSION
	else if(field.format == "B1"){
		result = lexical_cast<std::string>((int )((unsigned char*)data)[offset]);
	}
	//Binary Int Type B4 ---ATTENTION TO LITTLE->BIG_ENDIAN CONVERSION
	else if(field.format == "B4"){
		unsigned int temp_int;
		temp_int= ((unsigned char*)data)[offset+3] | ((unsigned char*)data)[offset+2] << 8 | ((unsigned char*)data)[offset+1] << 16 | ((unsigned char*)data)[offset] << 24;
		result = lexical_cast<std::string>(temp_int);
	}
	//int Type
	else if((field.format.substr(0,1) == "I" && field.end - field.begin+1 <= 4)){
		result = isValid(temp_char,length) ? string(temp_char,length) : "";
	}
	//long Type
	else if(field.format.substr(0,1) == "I" && field.end - field.begin+1 > 4 ){
		result = isValid(temp_char,length) ? string(temp_char,length) : "";
	}
	//float Type
	else if(field.format.substr(0,1) == "F" || field.format.substr(0,1) == "E"  || field.format.substr(0,1) == "D"  ){
		result = isValid(temp_char,length) ? string(temp_char,length) : "";
	}
	delete[] temp_char;
	return result;
}

#endif