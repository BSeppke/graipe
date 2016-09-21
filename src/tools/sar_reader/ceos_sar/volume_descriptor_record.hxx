/*
 *  volume_descriptor_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef VOLUME_DESCRIPTOR_RECORD_HXX
#define VOLUME_DESCRIPTOR_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
using namespace std;

class VolumeDescriptorRecord : public ERS_Record{

public:
	VolumeDescriptorRecord(char* data){
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==192 
				&&	((unsigned char*)data)[5]==192
				&&	((unsigned char*)data)[6]==18
				&&	((unsigned char*)data)[7]==18	){
			//cout << "VolumeDescriptorRecord::VolumeDescriptorRecord: Alles gut, Header scheint zu passen!\n";
		}
		else{
			cout << "VolumeDescriptorRecord::VolumeDescriptorRecord: Fehler, Header scheint nicht zu passen!\n";
		}
			
		unsigned int num_fields=32;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ERS_Field( 1,   1,   4,   "B4",  "-",		"Record sequence number");
		fields[ 1] = ERS_Field( 2,   5,   5,   "B1",  "-",		"1-st record subtype code");
		fields[ 2] = ERS_Field( 3,   6,   6,   "B1",  "-",		"Record type code"  );
		fields[ 3] = ERS_Field( 4,   7,   7,   "B1",  "-",		"2-nd subtype code"  );
		fields[ 4] = ERS_Field( 5,   8,   8,   "B1",  "-",		"3-rd subtype code"  );
		fields[ 5] = ERS_Field( 6,   9,  12,   "B4",  "bytes",	"Length of this record" );
		fields[ 6] = ERS_Field( 7,  13,  14,   "A2",  "-",		"ASCII/EBCDIC flag" );
		fields[ 7] = ERS_Field( 8,  15,  16,   "A2",  "-",		"Blanks");
		fields[ 8] = ERS_Field( 9,  17,  28,  "A12",  "-",		"Format control document" );
		fields[ 9] = ERS_Field(10,  29,  30,   "A2",  "-",		"Superstructure format control document");
		fields[10] = ERS_Field(11,  31,  32,   "A2",  "-",		"Superstructure record format revision");
		fields[11] = ERS_Field(12,  33,  44,  "A12",  "-",		"Logical volume  generating  facility software release and revision level" );
		fields[12] = ERS_Field(13,  45,  60,  "A16",  "-",		"ID of physical volume containing this volume descriptor" );
		fields[13] = ERS_Field(14,  61,  76,  "A16",  "-",		"Logical volume identifier" );
		fields[14] = ERS_Field(15,  77,  92,  "A16",  "-",		"Volume set identifier"    );
		fields[15] = ERS_Field(16,  93,  94,   "I2",  "-",		"Total number of physical volumes in the logical volume" );
		fields[16] = ERS_Field(17,  95,  96,   "I2",  "-",		"Physical volume sequence number of the first tape within the logical volume"  );
		fields[17] = ERS_Field(18,  97,  98,   "I2",  "-",		"Physical volume sequence number of the last tape within the logical volume" );
		fields[18] = ERS_Field(19,  99, 100,   "I2",  "-",		"Physical volume sequence number of the current tape within the logical volume" );
		fields[19] = ERS_Field(20, 101, 104,   "I4",  "-",		"First referenced file number  in this physical volume whithin the logical volume" );
		fields[20] = ERS_Field(21, 105, 108,   "I4",  "-",		"Logical volume within a volume set" );
		fields[21] = ERS_Field(22, 109, 112,   "I4",  "-",		"Logical volume number within physical volume"  );
		fields[22] = ERS_Field(23, 113, 120,   "A8",  "-",		"Logical volume creation date (YYYYMMDD)"    );
		fields[23] = ERS_Field(24, 121, 128,   "A8",  "-",		"Logical volume creation time (hhmmssdd, dd=deci-seconds)" );
		fields[24] = ERS_Field(25, 129, 140,  "A12",  "-",		"Logical volume generation country" );
		fields[25] = ERS_Field(26, 141, 148,   "A8",  "-",		"Logical volume generating agency");
		fields[26] = ERS_Field(27, 149, 160,  "A12",  "-",		"Logical volume generating facility" );
		fields[27] = ERS_Field(28, 161, 164,   "I4",  "-",		"Number of file pointer records in volume directory" );
		fields[28] = ERS_Field(29, 165, 168,   "I4",  "-",		"Number of records in volume directory" );
		fields[29] = ERS_Field(30, 169, 172,   "I4",  "-",		"Total number of logical volumes in volume set" );
		fields[30] = ERS_Field(31, 173, 260,  "A88",  "-",		"Volume descriptor spare segment (always blank filled)" );
		fields[31] = ERS_Field(32, 261, 360, "AlOO",  "-",		"Local use segment");
		
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