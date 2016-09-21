/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RADIOMETRIC_COMPENSATION_DATA_RECORD_HXX
#define RADIOMETRIC_COMPENSATION_DATA_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

class RadiometricCompensationDataRecord : public ERS_Record{
	
public:
	RadiometricCompensationDataRecord(char* data){
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==18 
			&&	((unsigned char*)data)[5]==51
			&&	((unsigned char*)data)[6]==18
			&&	((unsigned char*)data)[7]==20	){
			
			//cout << "RadiometricCompensationDataRecord::RadiometricCompensationDataRecord: Alles gut, Header scheint zu passen (Revision A)!\n";
		}
		else if (		((unsigned char*)data)[4]==10 
					&&	((unsigned char*)data)[5]==40
					&&	((unsigned char*)data)[6]==31
					&&	((unsigned char*)data)[7]==20	){
			//cout << "RadiometricCompensationDataRecord::RadiometricCompensationDataRecord: Alles gut, Header scheint zu passen (Revision B)!\n";
		}
		else{
			cout << "RadiometricCompensationDataRecord::RadiometricCompensationDataRecord: Fehler, Header scheint nicht zu passen!\n";
		}
		
		char * temp_char = new char[8];
		memcpy(temp_char, data+20, 8); //get number of datapoints
		
		int num_datapoints = atoi(temp_char);
		num_datapoints = max(0, num_datapoints);
		unsigned int num_fields=24+num_datapoints*2;
		
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[  0] = ERS_Field(  1,   1,   4,  "B4",		"-",		"Record sequence number");
		fields[  1] = ERS_Field(  2,   5,   5,  "B1",		"-",		"1-st record subtype code");
		fields[  2] = ERS_Field(  3,   6,   6,  "B1",		"-",		"Record type code"  );
		fields[  3] = ERS_Field(  4,   7,   7,  "B1",		"-",		"2-nd subtype code"  );
		fields[  4] = ERS_Field(  5,   8,   8,  "B1",		"-",		"3-rd subtype code"  );
		fields[  5] = ERS_Field(  6,   9,  12,  "B4",		"bytes",	"Length of this record" );
		fields[  6] = ERS_Field(  7,  13,  16,  "I4",		"-",		"Rad. compensation record Sequence number");
		fields[  7] = ERS_Field(  8,  17,  20,  "I4",		"-",		"SAR channel indicator" );
		fields[  8] = ERS_Field(  9,  21,  28,  "I8",		"-",		"Number of radiometric compensation data sets in the record" );
		fields[  9] = ERS_Field( 10,  29,  36,  "I8",		"bytes",	"Compensation data set size" );
		fields[ 10] = ERS_Field( 11,  37,  44,  "A8",		"-",		"Compensation data type designator   (eg: RANGE  AZIMUTH  PIXEL , LINE)" );
		fields[ 11] = ERS_Field( 12,  45,  76,	"A32",		"-",		"Compensation  data   descriptor   (eg: elevation antenna pattern, range attenuation, resolution cell size, azimuth attenuation , etc.)" );
		fields[ 12] = ERS_Field( 13,  77,  80,  "I4",		"-",		"Number of compensation records");
		fields[ 13] = ERS_Field( 14,  81,  84,  "I4",		"-",		"Sequence number in the full compensation table of the table contained in this record");
		fields[ 14] = ERS_Field( 15,  85,  92,  "I8",		"-",		"Total number of compensation pairs in the full compensation table" );
		fields[ 15] = ERS_Field( 16,  93, 100,  "I8",		"-",		"Data pixel number corresponding to first correction value in compensation table");
		fields[ 16] = ERS_Field( 17, 101, 108,  "I8",		"-",		"Data pixel number corresponding to last correction value in compensation table" );
		fields[ 17] = ERS_Field( 18, 109, 116,	"I8",		"pixels",	"Compensation pixel group size");
		fields[ 18] = ERS_Field( 19, 117, 132,	"F16.7",	"dB",		"Min. table Offset value" );
		fields[ 19] = ERS_Field( 20, 133, 148,	"F16.7",	"dB",		"Min. table Gain value" );
		fields[ 20] = ERS_Field( 21, 149, 164,	"F16.7",	"dB",		"Max. table Offset value");
		fields[ 21] = ERS_Field( 22, 165, 180,	"F16.7",	"dB",		"Max. table Gain value"  );
		fields[ 22] = ERS_Field( 23, 181, 196,  "A16",		"-",		"spare"  );
		fields[ 23] = ERS_Field( 24, 197, 204,  "I8",		"-",		"Number of compensation table entries (up to 256 samples/record in example)" );
		
		// RADIOMETRIC COMPENSATION TABLE VALUES
		//may be repeated for up to 256 samples/record in example! See Field 9 for samples count...
		for (unsigned int i= 0; i < num_datapoints; ++i){
			fields[ 24+i*2] = ERS_Field( 25+i*3, 205+i*32, 220+i*32,  "F16.7",   "dB",	lexical_cast<string>(i+1) + ". compensation sample Offset" );
			fields[ 25+i*2] = ERS_Field( 26+i*3, 221+i*32, 236+i*32,  "F16.7",   "dB",	lexical_cast<string>(i+1) + ". compensation sample Gain");
		}
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