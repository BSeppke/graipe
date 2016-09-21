/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PLATFORM_POSITION_DATA_RECORD_HXX
#define PLATFORM_POSITION_DATA_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

class PlatformPositionDataRecord : public ERS_Record{
	
public:
	PlatformPositionDataRecord(char* data){
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==18 
			&&	((unsigned char*)data)[5]==30
			&&	((unsigned char*)data)[6]==18
			&&	((unsigned char*)data)[7]==20	){
			
			//cout << "PlatformPositionDataRecord::PlatformPositionDataRecord: Alles gut, Header scheint zu passen (Revision A)!\n";
		}
		else if (		((unsigned char*)data)[4]==10 
					&&	((unsigned char*)data)[5]==30
					&&	((unsigned char*)data)[6]==31
					&&	((unsigned char*)data)[7]==20	){
			//cout << "PlatformPositionDataRecord::PlatformPositionDataRecord: Alles gut, Header scheint zu passen (Revision B)!\n";
		}
		else{
			cout << "PlatformPositionDataRecord::PlatformPositionDataRecord: Fehler, Header scheint nicht zu passen!\n";
		}
		
		char * temp_char = new char[4];
		memcpy(temp_char, data+140, 4); //get number of datapoints
		
		int num_datapoints = atoi(temp_char);
		num_datapoints = max(0, num_datapoints);
		
		unsigned int num_fields=28+num_datapoints*6;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[  0] = ERS_Field(  1,   1,   4,  "B4",		"-",		"Record sequence number");
		fields[  1] = ERS_Field(  2,   5,   5,  "B1",		"-",		"1-st record subtype code");
		fields[  2] = ERS_Field(  3,   6,   6,  "B1",		"-",		"Record type code"  );
		fields[  3] = ERS_Field(  4,   7,   7,  "B1",		"-",		"2-nd subtype code"  );
		fields[  4] = ERS_Field(  5,   8,   8,  "B1",		"-",		"3-rd subtype code"  );
		fields[  5] = ERS_Field(  6,   9,  12,	"B4",		"bytes",	"Length of this record" );
		fields[  6] = ERS_Field(  7,  13,  44,  "A32",		"-",		"reserved"    );
		fields[  7] = ERS_Field(  8,  45,  60,  "F16",		"-",		"reserved"    );
		fields[  8] = ERS_Field(  9,  61,  76,  "F16",		"-",		"reserved"    );
		fields[  9] = ERS_Field( 10,  77,  92,  "F16",		"-",		"reserved"    );
		fields[ 10] = ERS_Field( 11,  93, 108,  "F16",		"-",		"reserved"    );
		fields[ 11] = ERS_Field( 12, 109, 124,  "F16",		"-",		"reserved"    );
		fields[ 12] = ERS_Field( 13, 125, 140,  "F16",		"-",		"reserved"    );
		fields[ 13] = ERS_Field( 14, 141, 144,  "I4",		"-",		"Number of data points (up to 64)");
		fields[ 14] = ERS_Field( 15, 145, 148,  "I4",		"-",		"Year  of data point. (YYYY)");
		fields[ 15] = ERS_Field( 16, 149, 152,  "I4",		"-",		"Month of data point. ($$MM)" );
		fields[ 16] = ERS_Field( 17, 153, 156,  "I4",		"-",		"Day   of data point. ($$DD)");
		fields[ 17] = ERS_Field( 18, 157, 160,  "I4",		"-",		"Day in the year (GMT)"  );
		fields[ 18] = ERS_Field( 19, 161, 182,	"D22.15",	"-",		"Seconds of day (GMT) of data");
		fields[ 19] = ERS_Field( 20, 183, 204,	"D22.15",	"sec",		"Time interval between DATA points" );
		fields[ 20] = ERS_Field( 21, 205, 268,  "A64",		"-",		"Reference coordinate system" );
		fields[ 21] = ERS_Field( 22, 269, 290,	"D22.15",	"degrees",	"Greenwich mean hour angle");
		fields[ 22] = ERS_Field( 23, 291, 306,  "F16.7",	"meters",	"Along track position error" );
		fields[ 23] = ERS_Field( 24, 307, 322,  "F16.7",	"meters",	"Across track position error" );
		fields[ 24] = ERS_Field( 25, 323, 338,  "F16.7",	"meters",	"Radial position error");
		fields[ 25] = ERS_Field( 26, 339, 354,  "F16.7",	"-",		"reserved");
		fields[ 26] = ERS_Field( 27, 355, 370,  "F16.7",	"-",		"reserved");
		fields[ 27] = ERS_Field( 28, 371, 386,  "F16.7",	"-",		"reserved");
		//may be repeated for up to 64points! See Field 14 for point count...
		for (unsigned int i= 0; i < num_datapoints; ++i){
			fields[ 28+i*6 ] = ERS_Field( 29+i*2, 387+i*132, 408+i*132, "D22.15", "-",		lexical_cast<string>(i+1) + ". data  point  position  vectorX as latitude, longitude and altitude for airborne sensor platform"); //29-1
			fields[ 29+i*6 ] = ERS_Field( 29+i*2, 409+i*132, 430+i*132, "D22.15", "-",		lexical_cast<string>(i+1) + ". data  point  position  vectorY as latitude, longitude and altitude for airborne sensor platform"); //29-2
			fields[ 30+i*6 ] = ERS_Field( 29+i*2, 431+i*132, 452+i*132, "D22.15", "-",		lexical_cast<string>(i+1) + ". data  point  position  vectorZ as latitude, longitude and altitude for airborne sensor platform"); //29-3
			fields[ 31+i*6 ] = ERS_Field( 30+i*2, 453+i*132, 474+i*132, "D22.15", "-",		lexical_cast<string>(i+1) + ". data  point  position  vectorX in airborne coordinates (meters/second & degrees/second) for airborne sensor platform or in a reference system such as CTS for spaceborne sensor platforms"); //30-1
			fields[ 32+i*6 ] = ERS_Field( 30+i*2, 475+i*132, 496+i*132, "D22.15", "-",		lexical_cast<string>(i+1) + ". data  point  position  vectorY in airborne coordinates (meters/second & degrees/second) for airborne sensor platform or in a reference system such as CTS for spaceborne sensor platforms"); //30-2
			fields[ 33+i*6 ] = ERS_Field( 30+i*2, 497+i*132, 518+i*132, "D22.15", "-",		lexical_cast<string>(i+1) + ". data  point  position  vectorZ in airborne coordinates (meters/second & degrees/second) for airborne sensor platform or in a reference system such as CTS for spaceborne sensor platforms"); //30-3
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