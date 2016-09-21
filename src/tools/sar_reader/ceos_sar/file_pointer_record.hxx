/*
 *  file_pointer_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef FILE_POINTER_RECORD_HXX
#define FILE_POINTER_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class FilePointerRecord : public ERS_Record{
	
public:
	FilePointerRecord(char* data){
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==219 
				&&	((unsigned char*)data)[5]==192
				&&	((unsigned char*)data)[6]==18
				&&	((unsigned char*)data)[7]==18	){
				//cout << "FilePointerRecord::FilePointerRecord: Alles gut, Header scheint zu passe!\n";
		}
		else{
			cout << "FilePointerRecord::FilePointerRecord: Fehler, Header scheint nicht zu passen!\n";
		}
		
		unsigned int num_fields=25;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ERS_Field( 1,   1,   4,	"B4",	"-",		"Record sequence number");
		fields[ 1] = ERS_Field( 2,   5,   5,	"B1",	"-",		"1-st record subtype code");
		fields[ 2] = ERS_Field( 3,   6,   6,	"B1",	"-",		"Record type code"  );
		fields[ 3] = ERS_Field( 4,   7,   7,	"B1",	"-",		"2-nd subtype code"  );
		fields[ 4] = ERS_Field( 5,   8,   8,	"B1",	"-",		"3-rd subtype code"  );
		fields[ 5] = ERS_Field( 6,   9,  12,	"B4",	"bytes",	"Length of this record" );
		fields[ 6] = ERS_Field( 7,  13,  14,	"A2", 	"-",		"ASCII/EBCDIC flag" );
		fields[ 7] = ERS_Field( 8,  15,  16,	"A2", 	"-",		"Blanks");
		fields[ 8] = ERS_Field( 9,  17,  20,	"I4", 	"-",		"Referenced file number" );
		fields[ 9] = ERS_Field(10,  21,  36,	"A16",	"-",		"Referenced file name" );
		fields[10] = ERS_Field(11,  37,  64,	"A28",	"-",		"Referenced file class" );
		fields[11] = ERS_Field(12,  65,  68,	"A4", 	"-",		"Referenced file class code" );
		fields[12] = ERS_Field(13,  69,  96,	"A28",	"-",		"Referenced file data type" );
		fields[13] = ERS_Field(14,  97, 100,	"A4", 	"-",		"Referenced file data type code" );
		fields[14] = ERS_Field(15, 101, 108,	"I8", 	"-",		"Number of records in referenced file" );
		fields[15] = ERS_Field(16, 109, 116,	"I8",	"bytes",	"Referenced file  1-st  record  length" );
		fields[16] = ERS_Field(17, 117, 124,	"I8", 	"bytes",	"Referenced file maximum record length" );
		fields[17] = ERS_Field(18, 125, 136,	"A12",	"-",		"Referenced file record length type" );
		fields[18] = ERS_Field(19, 137, 140,	"A4", 	"-",		"Referenced file record length type code" );
		fields[19] = ERS_Field(20, 141, 142,	"I2",	"-",		"Referenced file physical volume start number" );
		fields[20] = ERS_Field(21, 143, 144,	"I2",	"-",		"Referenced file physical volume end number" );
		fields[21] = ERS_Field(22, 145, 152,	"I8", 	"-",		"Referenced file portion start, 1-st record number for this physical volume" );
		fields[22] = ERS_Field(23, 153, 160,	"I8", 	"-",		"Referenced file portion end, last record number for this physical volume" );
		fields[23] = ERS_Field(24, 161, 260,	"AlOO",	"-",		"File pointer spare segment" );
		fields[24] = ERS_Field(25, 261, 360,	"AlOO", "-",		"Local use segment" );
		
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