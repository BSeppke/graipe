/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ATTITUDE_RECORD_HXX
#define ATTITUDE_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class AttitudeDataRecord : public ERS_Record{
	
public:
	AttitudeDataRecord(char* data){
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==18 
				&&	((unsigned char*)data)[5]==40
				&&	((unsigned char*)data)[6]==18
				&&	((unsigned char*)data)[7]==20	){
			
			//cout << "AttitudeDataRecord::AttitudeDataRecord: Alles gut, Header scheint zu passe!\n";
		}
		else{
			cout << "AttitudeDataRecord::AttitudeDataRecord: Fehler, Header scheint nicht zu passen!\n";
		}
		
		char * temp_char = new char[4];
		memcpy(temp_char, data+13, 4); //get Number of attitude data points
		int att_points = atoi(temp_char);
		
		unsigned int num_fields= 7 + att_points*14;
		fields.resize(num_fields);
		values.resize(num_fields);
		
		fields[ 0] = ERS_Field(  1,  1,   4,   "B4", "-",		"Record sequence number");
		fields[ 1] = ERS_Field(  2,  5,   5,   "B1", "-",		"1-st record subtype code");
		fields[ 2] = ERS_Field(  3,  6,   6,   "B1", "-",		"Record type code"  );
		fields[ 3] = ERS_Field(  4,  7,   7,   "B1", "-",		"2-nd subtype code"  );
		fields[ 4] = ERS_Field(  5,  8,   8,   "B1", "-",		"3-rd subtype code"  );
		fields[ 5] = ERS_Field(  6,  9,  12,   "B4", "bytes",	"Length of this record" );
		fields[ 6] = ERS_Field( 7,  13,  16,   "I4", "-",		"Number of attitude data points" );
		
		for (int i=0; i< att_points; ++i){
			fields[ 7+14*i] = ERS_Field( 8+14*i,  17+120*i,  20+120*i,   "I4", "-",		"Day of the year" );
			fields[ 8+14*i] = ERS_Field( 9+14*i,  21+120*i,  28+120*i,   "I8", "ms",		"Millisecond of day ('0' - '86399999')");
			fields[ 9+14*i] = ERS_Field(10+14*i,  29+120*i,  32+120*i,   "I4", "-",		"Pitch data quality flag  (Good : '0', 1 else)" );
			fields[10+14*i] = ERS_Field(11+14*i,  33+120*i,  36+120*i,   "I4", "-",		"Roll data quality flag  (Good : '0', 1 else)" );
			fields[11+14*i] = ERS_Field(12+14*i,  37+120*i,  40+120*i,   "I4", "-",		"Yaw data quality flag  (Good : '0', 1 else)"  );
			fields[12+14*i] = ERS_Field(13+14*i,  41+120*i,  54+120*i,"E14.6", "degrees",		"Pitch");
			fields[13+14*i] = ERS_Field(14+14*i,  45+120*i,  68+120*i,"E14.6", "degrees",		"Roll");
			fields[14+14*i] = ERS_Field(15+14*i,  69+120*i,  82+120*i,"E14.6", "degrees",		"Yaw");
			fields[15+14*i] = ERS_Field(16+14*i,  83+120*i,  86+120*i,   "I4", "-",		"Pitch rate data quality flag  (Good : '0', 1 else)" );
			fields[16+14*i] = ERS_Field(17+14*i,  87+120*i,  90+120*i,   "I4", "-",		"Roll rate data quality flag  (Good : '0', 1 else)" );
			fields[17+14*i] = ERS_Field(18+14*i,  91+120*i,  94+120*i,   "I4", "-",		"Yaw rate data quality flag  (Good : '0', 1 else)"  );
			fields[18+14*i] = ERS_Field(19+14*i,  95+120*i, 108+120*i,"E14.6", "degrees/sec",		"Pitch rate");
			fields[19+14*i] = ERS_Field(20+14*i, 109+120*i, 122+120*i,"E14.6", "degrees/sec",		"Roll rate");
			fields[20+14*i] = ERS_Field(21+14*i, 123+120*i, 136+120*i,"E14.6", "degrees/sec",		"Yaw rate");
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