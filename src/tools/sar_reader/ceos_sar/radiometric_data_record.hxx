/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RADIOMETRIC_DATA_RECORD_HXX
#define RADIOMETRIC_DATA_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

class RadiometricDataRecord : public ERS_Record{
	
public:
	RadiometricDataRecord(char* data){
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==18 
			&&	((unsigned char*)data)[5]==50
			&&	((unsigned char*)data)[6]==18
			&&	((unsigned char*)data)[7]==20	){
			
			//cout << "RadiometricDataRecord::RadiometricDataRecord: Alles gut, Header scheint zu passen (Revision A)!\n";
		}
		else{
			cout << "RadiometricDataRecord::RadiometriDataRecord: Fehler, Header scheint nicht zu passen!\n";
		}
		
		unsigned int num_fields=26;
		
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[  0] = ERS_Field(  1,   1,    4,  "B4",	  "-",		"Record sequence number");
		fields[  1] = ERS_Field(  2,   5,    5,  "B1",	  "-",		"1-st record subtype code");
		fields[  2] = ERS_Field(  3,   6,    6,  "B1",	  "-",		"Record type code"  );
		fields[  3] = ERS_Field(  4,   7,    7,  "B1",	  "-",		"2-nd subtype code"  );
		fields[  4] = ERS_Field(  5,   8,    8,  "B1",	  "-",		"3-rd subtype code"  );
		fields[  5] = ERS_Field(  6,   9,   12,  "B4",	  "bytes",	"Length of this record" );
		fields[  6] = ERS_Field( 7,   13,   16,  "I4",    "-",		"Radiometric Data Record sequence number ( starts at 1 ) = 'bbb1'");
		fields[  7] = ERS_Field( 8,   17,   20,  "I4",    "-",		"Number of Radiometric Data Record fields");
		fields[  8] = ERS_Field( 9,   21,   36,  "F16.7", "-",		"Calibration factor (CF) Level 1.1 : s0 = 10 * log10<I2+Q2> + CF - 32.0  Level 1.5 : s0 = 10*log10<DN2> + CF This means that the sigma-naught of the pixel can be obtained by the ensemble averaging (<>) of the pixel values, in reality, the spatial averaging of the pixel values around the target.Here, I, Q, and DN in <> of the above formulas are the pixel values in levels 1.1, and 1.5, respectively.");
		//Transmission and reception distortion matrices applied for polarimetric data (level 1.1, *)
		fields[  9] = ERS_Field(10,   37,  52, 	 "F16.7", "-",		"Real part of transmission distortion matrix (DT)(1,1)");
		fields[ 10] = ERS_Field(11,   53,  68, 	 "F16.7", "-",		"Imaginary part of DT (1,1)");
		fields[ 11] = ERS_Field(12,   69,  84, 	 "F16.7", "-",		"Real part of DT (1,2)");
		fields[ 12] = ERS_Field(13,   85, 100, 	 "F16.7", "-",		"Imaginary part of DT (1,2)");
		fields[ 13] = ERS_Field(14,  101, 116,	 "F16.7", "-",		"Real part of DT (2, 1)");
		fields[ 14] = ERS_Field(15,  117, 132, 	 "F16.7", "-",		"Imaginary part of DT(2, 1)" );
		fields[ 15] = ERS_Field(16,  133, 148, 	 "F16.7", "-",		"Real part of DT (2, 2)");
		fields[ 16] = ERS_Field(17,  149, 164, 	 "F16.7", "-",		"Imaginary part of DT (2, 2)");
		fields[ 17] = ERS_Field(18,  165, 180, 	 "F16.7", "-",		"Real part of reception distortion matrix (DR)(1,1)");
		fields[ 18] = ERS_Field(19,  181, 196, 	 "F16.7", "-",		"Imaginary part of DR (1,1)");
		fields[ 19] = ERS_Field(20,  197, 212, 	 "F16.7", "-",		"Real part of DR (1,2)");
		fields[ 20] = ERS_Field(21,  213, 228, 	 "F16.7", "-",		"Imaginary part of DR (1,2)");
		fields[ 21] = ERS_Field(22,  229, 244, 	 "F16.7", "-",		"Real part of DR (2, 1)");
		fields[ 22] = ERS_Field(23,  245, 260, 	 "F16.7", "-",		"Imaginary part of DR (2, 1)");
		fields[ 23] = ERS_Field(24,  261, 276,	 "F16.7", "-",		"Real part of DR(2, 2)");
		fields[ 24] = ERS_Field(25,  277, 292,	 "F16.7", "-",		"Imaginary part of DR (2, 2)");
		fields[ 25] = ERS_Field(26,  293,9860,  "A9568", "-",		"Reserve (blanks)");
		
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