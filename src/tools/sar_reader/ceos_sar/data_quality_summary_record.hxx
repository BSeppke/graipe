/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DATA_QUALITY_SUMMARY_RECORD_HXX
#define DATA_QUALITY_SUMMARY_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class DataQualitySummaryRecord : public ERS_Record{
	
public:
	DataQualitySummaryRecord(char* data){
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==18 
				&&	((unsigned char*)data)[5]==60
				&&	((unsigned char*)data)[6]==18
				&&	((unsigned char*)data)[7]==20	){
			
			//cout << "DataQualitySummaryRecord::DataQualitySummaryRecord: Alles gut, Header scheint zu passe!\n";
		}
		else{
			cout << "DataQualitySummaryRecord::DataQualitySummaryRecord: Fehler, Header scheint nicht zu passen!\n";
		}
		
		unsigned int num_fields=93;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ERS_Field(  1,   1,   4,   "B4",	"-",		"Record sequence number");
		fields[ 1] = ERS_Field(  2,   5,   5,   "B1",	"-",		"1-st record subtype code");
		fields[ 2] = ERS_Field(  3,   6,   6,   "B1",	"-",		"Record type code"  );
		fields[ 3] = ERS_Field(  4,   7,   7,   "B1",	"-",		"2-nd subtype code"  );
		fields[ 4] = ERS_Field(  5,   8,   8,   "B1",	"-",		"3-rd subtype code"  );
		fields[ 5] = ERS_Field(  6,   9,  12,   "B4",	"bytes",	"Length of this record" );
		fields[ 6] = ERS_Field(  7,  13,  16,	"I4",	"-",		"Data summary quality record sequence number (starts at 1) = 'bbb1'");
		fields[ 7] = ERS_Field(  8,  17,  20,	"A4",	"-",		"SAR channel indicator");
		fields[ 8] = ERS_Field(  9,  21,  26,	"A4",	"-",		"Date of the last calibration update = 'YYMMDD'");
		fields[ 9] = ERS_Field( 10,  27,  30,	"I4",	"-",		"Number of channels (up to 16)");
		//ABSOLUTE RADIOMETRIC DATA QUALITY
		fields[10] = ERS_Field( 11,  31,  46, "F16.7",	"dB",		"Nominal Integrated Side Lobe Ratio (ISLR)");
		fields[11] = ERS_Field( 12,  47,  62, "F16.7",	"dB",		"Nominal Peak Side Lobe to main lobe Ratio (PSLR)");
		fields[12] = ERS_Field( 13,  63,  78, "F16.7",	"-",		"Nominal azimuth ambiguity (AAR)");
		fields[13] = ERS_Field( 14,  79,  94, "F16.7",	"-",		"Nominal range ambiguity (RAR)");
		fields[14] = ERS_Field( 15,  95, 110, "F16.7",	"dB",		"Estimate of SNR (from range spectra)");
		fields[15] = ERS_Field( 16, 111, 126, "F16.7",	"-",		"Actual Bit Error Rate (BER)");
		fields[16] = ERS_Field( 17, 127, 142, "F16.7",	"m",		"Nominal slant range resolution");
		fields[17] = ERS_Field( 18, 143, 158, "F16.7",	"m",		"Nominal azimuth resolution");
		fields[18] = ERS_Field( 19, 159, 174, "F16.7",	"dB",		"Nominal radiometric resolution");
		fields[19] = ERS_Field( 20, 175, 190, "F16.7",	"dB",		"Instantaneous dynamic range");
		fields[20] = ERS_Field( 21, 191, 206, "F16.7",	"dB",		"Nominal absolute radiometric calibration magnitude of uncertainty of SAR channel indicated in bytes 17-20");
		fields[21] = ERS_Field( 22, 207, 222, "F16.7",	"degrees",	"Nominal absolute radiometric calibration magnitude uncertainty of SAR channel indicated in bytes 17-20");
		//RELATIVE RADIOMETRIC DATA QUALITY
		for(int i=0; i<16; ++i){
			fields[22+2*i] = ERS_Field(  23+2*i, 223+32*i, 238+32*i, "F16.7", "dB",			string("Nominal relative radiometric calibration magnitude uncertainty of SAR channel ") + lexical_cast<string>(i+1) + " versus first of the other channels on a multi-channel volume");
			fields[23+2*i] = ERS_Field(  24+2*i, 239+32*i, 254+32*i, "F16.7", "degrees",	string("Nominal relative radiometric calibration phase uncertainty of SAR channel ") + lexical_cast<string>(i+1) + " versus first of the other channels on a multi-channel volume ");
		}
		//ABSOLUTE GEOMETRIC DATA QUALITY
		fields[54] = ERS_Field( 55, 735, 750, "F16.7", "m", "Nominal absolute location error along track");
		fields[55] = ERS_Field( 56, 751, 766, "F16.7", "m", "Nominal absolute location error cross track");
		fields[56] = ERS_Field( 57, 767, 782, "F16.7", "-", "Nominal geometric distortion scale in line direction");
		fields[57] = ERS_Field( 58, 783, 798, "F16.7", "-", "Nominal geometric distortion scale in pixel direction");
		fields[58] = ERS_Field( 59, 799, 814, "F16.7", "-", "Nominal geometric distortion skew");
		fields[59] = ERS_Field( 60, 815, 830, "F16.7", "-", "Scene orientation error");
		//RELATIVE GEOMETRIC DATA QUALITY
		for(int i=0; i<16; ++i){
			fields[60+2*i] = ERS_Field( 61+2*i, 831+32*i,846+32*i, "F16.7", "m", string("Along track relative misregistration error of SAR channel ") + lexical_cast<string>(i+1) + " versus first of the other channels");
			fields[61+2*i] = ERS_Field( 62+2*i, 847+32*i,862+32*i, "F16.7", "m", string("Cross track relative misregistration error of SAR channel ") + lexical_cast<string>(i+1) + " versus first of the other channels");
		}
		fields[92] = ERS_Field( 93, 1103, 1620, "A518", "-", "blanks");
		
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