/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TEXT_RECORD_HXX
#define TEXT_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class TextRecord : public ERS_Record{
	
public:
	TextRecord(char* data){
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==18 
				&&	(((unsigned char*)data)[5]==63 || ((unsigned char*)data)[5]==192)
				&&	((unsigned char*)data)[6]==18
				&&	((unsigned char*)data)[7]==18	){
			
			//cout << "TextRecord::TextRecord: Alles gut, Header scheint zu passe!\n";
		}
		else{
			cout << "TextRecord::TextRecord: Fehler, Header scheint nicht zu passen!\n";
		}
		
		unsigned int num_fields=15;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ERS_Field(  1,  1,   4,   "B4", "-",		"Record sequence number");
		fields[ 1] = ERS_Field(  2,  5,   5,   "B1", "-",		"1-st record subtype code");
		fields[ 2] = ERS_Field(  3,  6,   6,   "B1", "-",		"Record type code"  );
		fields[ 3] = ERS_Field(  4,  7,   7,   "B1", "-",		"2-nd subtype code"  );
		fields[ 4] = ERS_Field(  5,  8,   8,   "B1", "-",		"3-rd subtype code"  );
		fields[ 5] = ERS_Field(  6,  9,  12,   "B4", "bytes",	"Length of this record" );
		fields[ 6] = ERS_Field( 7,  13,  14,   "A2", "-",		"ASCII/EBCDIC flag" );
		fields[ 7] = ERS_Field( 8,  15,  16,   "A2", "-",		"Continuation flag (is set to C$ if information is continued on the next text record (if any))" );
		fields[ 8] = ERS_Field( 9,  17,  56,  "A40", "-",		"Product type specifier");
		fields[ 9] = ERS_Field(10,  57, 116,  "A60", "-",		"Location  and  date/time of product creation" );
		fields[10] = ERS_Field(11, 117, 156,  "A40", "-",		"Physical volumes identification" );
		fields[11] = ERS_Field(12, 157, 196,  "A40", "-",		"Scene identification" );
		fields[12] = ERS_Field(13, 197, 236,  "A40", "-",		"Scene location");
		fields[13] = ERS_Field(14, 237, 256,  "A20", "-",		"Spares" );
		fields[14] = ERS_Field(15, 257, 360, "A104", "-",		"Spares" );
		
		length = getRecordLength(data);
		int i=0;
		while(		i < num_fields
			  &&	 fields[i].end <= length){
			values[i] = getERSValue(data, fields[i]);
			++i;
		}
	}
};

#endif