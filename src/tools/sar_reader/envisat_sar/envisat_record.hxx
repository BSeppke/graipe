/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ENVISAT_RECORD_HXX
#define ENVISAT_RECORD_HXX

#include "envisat_field.hxx"
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

class EnvisatRecord{
	
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
	
protected:	
	vector<ENVISAT_Field> fields;
	vector<string> values;
};

#endif