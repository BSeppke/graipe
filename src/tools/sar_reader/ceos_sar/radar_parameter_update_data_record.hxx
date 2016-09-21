/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RADAR_PARAMETER_UPDATE_DATA_RECORD_HXX
#define RADAR_PARAMETER_UPDATE_DATA_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

class RadarParameterUpdateDataRecord : public ERS_Record {
	
public:
	RadarParameterUpdateDataRecord(char* data){
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==18 
			&&	((unsigned char*)data)[5]==100
			&&	((unsigned char*)data)[6]==18
			&&	((unsigned char*)data)[7]==20	){
			
			//cout << "RadarParameterUpdateDataRecord::RadarParameterUpdateDataRecord: Alles gut, Header scheint zu passen (Revision A)!\n";
		}
		else if (		((unsigned char*)data)[4]==10 
					&&	((unsigned char*)data)[5]==100
					&&	((unsigned char*)data)[6]==31
					&&	((unsigned char*)data)[7]==20	){
			//cout << "RadarParameterUpdateDataRecord::RadarParameterUpdateDataRecord: Alles gut, Header scheint zu passen (Revision B)!\n";
		}
		else{
			cout << "RadarParameterUpdateDataRecord::RadarParameterUpdateDataRecord: Fehler, Header scheint nicht zu passen!\n";
		}
		
		char * temp_char = new char[8];
		memcpy(temp_char, data+20, 8); //get number of datapoints
		
		int num_datapoints = atoi(temp_char);
		num_datapoints = max(0, num_datapoints);
		unsigned int num_fields=10+num_datapoints*6;
		
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[  0] = ERS_Field(  1,   1,   4,   "B4", "-",		"Record sequence number");
		fields[  1] = ERS_Field(  2,   5,   5,   "B1", "-",		"1-st record subtype code");
		fields[  2] = ERS_Field(  3,   6,   6,   "B1", "-",		"Record type code"  );
		fields[  3] = ERS_Field(  4,   7,   7,   "B1", "-",		"2-nd subtype code"  );
		fields[  4] = ERS_Field(  5,   8,   8,   "B1", "-",		"3-rd subtype code"  );
		fields[  5] = ERS_Field(  6,   9,  12,   "B4", "bytes", "Length of this record" );
		fields[  6] = ERS_Field(  7,  13,  16,   "I4", "-",		"Radar parameter record sequence number" );
		fields[  7] = ERS_Field(  8,  17,  20,   "A4", "-",		"spare");
		fields[  8] = ERS_Field(  9,  21,  28,   "I8", "-",		"Number of radar parameter update data sets in the record" );
		fields[  9] = ERS_Field( 10,  29,  36,   "I8", "bytes",	"Radar parameter update data set size" );
		//1ST    RADAR   PARAMETER UPDATE DATA SET   
		//may be repeated! See Field 9 for samples count...
		for (unsigned int i= 0; i < num_datapoints; ++i){
			fields[ 10+i*6 ] = ERS_Field( 11+i*6,  37+i*58,  56+i*58,    "A20",  "GMT",		lexical_cast<string>(i+1) + ". GMT of Change (YYYYMMDD-hhmmssttt$$)" );
			fields[ 11+i*6 ] = ERS_Field( 12+i*6,  57+i*58,  60+i*58,     "A4",  "-",		lexical_cast<string>(i+1) + ". SAR channel indicator");
			fields[ 12+i*6 ] = ERS_Field( 13+i*6,  61+i*58,  68+i*58,     "I8",  "-",		lexical_cast<string>(i+1) + ". Radar data line number where this update takes effect" );
			fields[ 13+i*6 ] = ERS_Field( 14+i*6,  69+i*58,  76+i*58,     "I8",   "-",		lexical_cast<string>(i+1) + ". Radar data  sample number where this update takes effect");
			fields[ 14+i*6 ] = ERS_Field( 15+i*6,  77+i*58, 108+i*58,    "A32",   "-",		lexical_cast<string>(i+1) + ". Parameter descriptor field, one of: RECEIVER GAIN (dB), RECORD WINDOW POSITION (Vsec) , ELECTRONIC BORESIGHT (degrees) , PRF (Hz),PULSE BANDWIDTH (Hz) , PULSE DURATION (fsec), QUANTIZATION (bits)  etc." );
			fields[ 15+i*6 ] = ERS_Field(16+i*6, 109+i*58, 124+i*58,   "E16.7",   "-",		lexical_cast<string>(i+1) + ". Parameter value");
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