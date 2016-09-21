/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ERS_RECORD_HXX
#define ERS_RECORD_HXX

#include "ers_field.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

unsigned int getRecordSequenceNumber( char* data){
	return ((unsigned char*)data)[3] | ((unsigned char*)data)[2] << 8 | ((unsigned char*)data)[1] << 16 | ((unsigned char*)data)[0] << 24;
}
unsigned int getRecordLength( char* data){
	return ((unsigned char*)data)[11] | ((unsigned char*)data)[10] << 8 | ((unsigned char*)data)[9] << 16 | ((unsigned char*)data)[8] << 24;
}

class ERS_Record{
	
public:
	
	string toCSV(char separator =';', bool trim_values=true, bool show_units = true, bool suppress_empty_fields=true){
		stringstream s;
		for(unsigned int i=0; i<fields.size(); ++i){
			string temp  = values[i];  
			if (trim_values) 
				trim(temp);
			if ( !(suppress_empty_fields && temp =="")){
				s << fields[i].name << separator << temp;
				if (show_units && fields[i].unit !="-")
					s << separator << fields[i].unit ;
				s << "\n";
			}
		}
		return s.str();
	}
	
	unsigned int getLength(){
		return length;
	}
	
	vector<ERS_Field> fields;
	vector<string> values;
	unsigned int length;
};

#endif