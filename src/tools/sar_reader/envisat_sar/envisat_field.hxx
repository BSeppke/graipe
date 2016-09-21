/*
 *  ers_field.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef ENVISAT_FIELD_HXX
#define ENVISAT_FIELD_HXX

#include <string.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

class ENVISAT_Field{
public:
	ENVISAT_Field() 
	{}	
	
	ENVISAT_Field(int n_id, int n_begin, int n_end, std::string n_format, std::string n_unit, std::string n_name, std::string n_description)
	:	id(n_id),
	begin(n_begin), end(n_end),
	format(n_format),
	unit(n_unit),
	name(n_name),
	description(n_description){}
	
	int id,
	begin, end;
	
	std::string name, description,format, unit;
};

//get the string value of an ERS_Field
std::string getENVISATValue( char* data, ENVISAT_Field & field){
	string result;
	unsigned int length = field.end - field.begin+1;
	unsigned int offset = field.begin-1;
	char * temp_char = new char[length];
	memcpy(temp_char, data+field.begin-1, length);
	
	//Char/String Type
	if (field.format == "SpareField" || field.format == "AsciiString" || field.format =="UtcExternal"){
		result = string(temp_char,length);
	}
	//Boolean Type
	else if (field.format == "BooleanFlag"){
		result = lexical_cast<std::string>((int)*temp_char);
	}
	//Short type (signed)
	else if (field.format == "ss"){
		result = lexical_cast<std::string>((short)*temp_char);
	}
	//Short types (unsigned)
	else if (field.format == "us"){
		result = lexical_cast<std::string>((unsigned short)*temp_char);
	}
	//Integer-like types (ASCII) - we unfortunately have to use atof to cope with long integer types where atol does not work...
	else if(field.format == "Ac" || field.format == "As" || field.format == "Al" || field.format == "Ad" || field.format == "AsciiGeoCoordinate"  ){
		char * temp_cstr = new char[length+1];
		memcpy(temp_cstr, data+field.begin-1, length);
		temp_cstr[length]=0;
		result = lexical_cast<std::string>(atof(temp_cstr));
		cerr << "@" << field.format << ": " << temp_cstr <<" -> " << result << "\n";
		delete temp_cstr;
	}
	
	//MJD Date type
	else if(field.format == "MJD"){
		char temp[4];
		temp[0]=data[offset+3]; 		temp[1]=data[offset+2]; 		temp[2]=data[offset+1]; 		temp[3]=data[offset  ];
		string result_d = lexical_cast<std::string>(*((long*)temp));
		temp[0]=data[offset+7]; 		temp[1]=data[offset+6]; 		temp[2]=data[offset+5]; 		temp[3]=data[offset+4];
		string result_s = lexical_cast<std::string>(*((unsigned long*)temp));
		temp[0]=data[offset+11]; 		temp[1]=data[offset+10]; 		temp[2]=data[offset+9]; 		temp[3]=data[offset+8];
		string result_millisec = lexical_cast<std::string>(*((unsigned long*)temp));
		result = "D: " + result_d + ", sec: " + result_s +", millisec: "+result_millisec;
	}
	//Unsigned long
	else if(field.format == "ul"){
		char temp[4];
		temp[0]=data[offset+3]; 		temp[1]=data[offset+2]; 		temp[2]=data[offset+1]; 		temp[3]=data[offset  ];
		result = lexical_cast<std::string>(*((unsigned long*)temp));
	}
	//signed long type
	else if(field.format == "sl" || field.format == "GeoCoordinate"  ){
		char temp[4];
		temp[0]=data[offset+3]; 		temp[1]=data[offset+2]; 		temp[2]=data[offset+1]; 		temp[3]=data[offset  ];
		result = lexical_cast<std::string>(*((long*)temp));
	}
	//Float Type
	else if(field.format == "fl"){
		char temp[4];
		temp[0]=data[offset+3]; 		temp[1]=data[offset+2]; 		temp[2]=data[offset+1]; 		temp[3]=data[offset  ];
		result = lexical_cast<std::string>(*((float*)temp));
	}
	//Float-like Types (ASCII)
	else if(field.format == "Afl" || field.format.substr(0,3) == "Ado"){
		char * temp_cstr = new char[length+1];
		memcpy(temp_cstr, data+field.begin-1, length);
		temp_cstr[length]=0;
		result = lexical_cast<std::string>(atof(temp_cstr));
		cerr << "@" << field.format << ": " << temp_cstr <<" -> " << result << "\n";
		
		delete temp_cstr;
	}
	delete[] temp_char;
	return result;
}

#endif