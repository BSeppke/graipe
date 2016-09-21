/*
 *  file_pointer_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef FILE_DESCRIPTOR_RECORD_HXX
#define FILE_DESCRIPTOR_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class FileDescriptorRecord : public ERS_Record{
	
public:
	FileDescriptorRecord(char* data){
		//get ESA-Header:		
		if (		(((unsigned char*)data)[4]==63 || ((unsigned char*)data)[4]==11)
				&&	((unsigned char*)data)[5]==192
				&&	((unsigned char*)data)[6]==18
				&&	((unsigned char*)data)[7]==18	){
				//cout << "FileDescriptorRecord::FileDescriptorRecord: Alles gut, Header scheint zu passe!\n";
		}
		else{
			cout << "FileDescriptorRecord::FileDescriptorRecord: Fehler, Header scheint nicht zu passen!\n";
		}
		
		unsigned int num_fields=96;
		fields.resize(num_fields);
		values.resize(num_fields);
		//Fixed segment
		fields[ 0] = ERS_Field(  1,   1,   4,   "B4",	"-",		"Record sequence number");
		fields[ 1] = ERS_Field(  2,   5,   5,   "B1",	"-",		"1-st record subtype code");
		fields[ 2] = ERS_Field(  3,   6,   6,   "B1",	"-",		"Record type code"  );
		fields[ 3] = ERS_Field(  4,   7,   7,   "B1",	"-",		"2-nd subtype code"  );
		fields[ 4] = ERS_Field(  5,   8,   8,   "B1",	"-",		"3-rd subtype code"  );
		fields[ 5] = ERS_Field(  6,   9,  12,   "B4",	"bytes",	"Length of this record" );
		fields[ 6] = ERS_Field( 7,  13,  14,	"A2",	"-",		"ASCII/EBCDIC flag" );
		fields[ 7] = ERS_Field( 8,  15,  16,	"A2", 	"-",		"Blanks");
		fields[ 8] = ERS_Field( 9,  17,  28,	"A12", 	"-",		"format control document ID for this data file format " );
		fields[ 9] = ERS_Field(10,  29,  30,	"A2", 	"-",		"Format control document revision level" );
		fields[10] = ERS_Field(11,	31,	 32,	"A2", 	"-",		"File design descriptor revision letter" );
		fields[11] = ERS_Field(12,  33,  34,	"A12", 	"-",		"Generating software release and revision level" );
		fields[12] = ERS_Field(13,  45,  48,	"I4", 	"-",		"File number" );
		fields[13] = ERS_Field(14,  49,  64,	"A16", 	"-",		"File name" );
		fields[14] = ERS_Field(15,  65,  68,	"A4", 	"-",		"Record sequence and location type flag" );
		fields[15] = ERS_Field(16,  69,  76,	"I8", 	"-",		"Sequence number location" );
		fields[16] = ERS_Field(17,  77,  80,	"I4", 	"-",		"Sequence number field length");
		fields[17] = ERS_Field(18,  81,  84,	"A4", 	"-",		"Record code and location type flag" );
		fields[18] = ERS_Field(19,  85,  92,	"I8", 	"-",		"Record code location" );
		fields[19] = ERS_Field(20,  93,  96,	"I4", 	"bytes",	"Record code field length" );
		fields[20] = ERS_Field(21,  97, 100,	"A4", 	"-",		"Record length and location type flag" );
		fields[21] = ERS_Field(22, 101, 108,	"I8", 	"-",		"Record length location");
		fields[22] = ERS_Field(23, 109, 112,	"I4", 	"bytes",	"Record length field length" );
		fields[23] = ERS_Field(24, 113, 113,	"A1", 	"-",		"Reserved");
		fields[24] = ERS_Field(25, 114, 114,	"A1", 	"-",		"Reserved");
		fields[25] = ERS_Field(26, 115, 115,	"A1",  	"-",		"Reserved");
		fields[26] = ERS_Field(27, 116, 116,	"A1",  	"-",		"Reserved" );
		fields[27] = ERS_Field(28, 117, 180,	"A64",  "-",		"Reserved segment" );
		//Variable Segment
		fields[28] = ERS_Field(29, 181, 186,	"I6",  	"-",		"Number of data set summary records" );
		fields[29] = ERS_Field(30, 187, 192,	"I6",  	"bytes",	"Data set summary record length" );
		fields[30] = ERS_Field(31, 193, 198,	"I6",  	"-",		"Number of map projection data records" );
		fields[31] = ERS_Field(32, 199, 204,	"I6",  	"bytes",	"Map projection record length" );
		fields[32] = ERS_Field(33, 205, 210,	"I6",  	"-",		"Number of platform pos. data records" );
		fields[33] = ERS_Field(34, 211, 216,	"I6",  	"bytes",	"Platform position record length");
		fields[34] = ERS_Field(35, 217, 222,	"I6",  	"-",		"Number of Attitude Data records" );
		fields[35] = ERS_Field(36, 223, 228,	"I6",  	"bytes",	"Attitude Data record length" );
		fields[36] = ERS_Field(37, 229, 234,	"I6",  	"-",		"Number of Radiometric Data records");
		fields[37] = ERS_Field(38, 235, 240,	"I6",  	"bytes",	"Radiometric record length" );
		fields[38] = ERS_Field(39, 241, 246,	"I6",  	"-",		"Number of Rad. compensation records" );
		fields[39] = ERS_Field(40, 247, 252,	"I6",  	"bytes",	"Radiometric compensation record length" );
		fields[40] = ERS_Field(41, 253, 258,	"I6",  	"-",		"Number of Data quality summary records" );
		fields[41] = ERS_Field(42, 259, 264,	"I6",  	"bytes",	"Data quality summary record length" );
		fields[42] = ERS_Field(43, 265, 270,	"I6",  	"-",		"Number of Data histograms records" );
		fields[43] = ERS_Field(44, 271, 276,	"I6",  	"bytes",	"Data histogram record length" );
		fields[44] = ERS_Field(45, 277, 282,	"I6",  	"-",		"Number of Range spectra records" );
		fields[45] = ERS_Field(46, 283, 288,	"I6",  	"bytes",	"Range spectra record length" );
		fields[46] = ERS_Field(47, 289, 294,	"I6",  	"-",		"Number of DEM descriptor records" );
		fields[47] = ERS_Field(48, 295, 300,	"I6",  	"bytes",	"DEM descriptor record length" );
		fields[48] = ERS_Field(49, 301, 306,	"I6",  	"-",		"Number of Radar par. update records" );
		fields[49] = ERS_Field(50, 307, 312,	"I6",  	"bytes",	"Radar par. update record length" );
		fields[50] = ERS_Field(51, 313, 318,	"I6",  	"-",		"Number of annotation Data records" );
		fields[51] = ERS_Field(52, 319, 324,	"I6",  	"bytes",	"Annotation Data record length" );
		fields[52] = ERS_Field(53, 325, 330,	"I6",  	"-",		"Number of det. processing records" );
		fields[53] = ERS_Field(54, 331, 336,	"I6",  	"bytes",	"Det. processing record length" );
		fields[54] = ERS_Field(55, 337, 342,	"I6",  	"-",		"Number of calibration records" );
		fields[55] = ERS_Field(56, 343, 348,	"I6",  	"bytes",	"Calibration record length" );
		fields[56] = ERS_Field(57, 349, 354,	"I6",  	"-",		"Number of GCP records" );
		fields[57] = ERS_Field(58, 355, 360,	"I6",  	"bytes",	"GCP record length" );
		fields[58] = ERS_Field(59, 361, 366,	"I6",  	"-",		"Spare" );
		fields[59] = ERS_Field(60, 367, 372,	"I6",  	"-",		"Spare" );
		fields[60] = ERS_Field(61, 373, 378,	"I6",  	"-",		"Spare" );
		fields[61] = ERS_Field(62, 379, 384,	"I6",  	"-",		"Spare" );
		fields[62] = ERS_Field(63, 385, 390,	"I6",  	"-",		"Spare" );
		fields[63] = ERS_Field(64, 391, 396,	"I6",  	"-",		"Spare" );
		fields[64] = ERS_Field(65, 397, 402,	"I6",  	"-",		"Spare" );
		fields[65] = ERS_Field(66, 403, 408,	"I6",  	"-",		"Spare" );
		fields[66] = ERS_Field(67, 409, 414,	"I6",  	"-",		"Spare" );
		fields[67] = ERS_Field(68, 415, 420,	"I6",  	"-",		"Spare" );
		fields[68] = ERS_Field(69, 421, 426,	"I6",  	"-",		"Number of facility Data1 records");
		fields[69] = ERS_Field(70, 427, 432,	"I8",  	"-",		"Facility Data1 record maximum length");
		//The following features may possible only been set by ALOS PalSar
		fields[70] = ERS_Field(71, 435, 440,	"I6",  	"-",		"Number of facility Data2 records");
		fields[71] = ERS_Field(72, 441, 448,	"I8",  	"-",		"Facility Data2 record maximum length");
		fields[72] = ERS_Field(73, 449, 454,	"I6",  	"-",		"Number of facility Data3 records");
		fields[73] = ERS_Field(74, 455, 462,	"I8",  	"-",		"Facility Data3 record maximum length");
		fields[74] = ERS_Field(75, 463, 468,	"I6",  	"-",		"Number of facility Data4 records");
		fields[75] = ERS_Field(76, 469, 476,	"I8",  	"-",		"Facility Data4 record maximum length");
		fields[76] = ERS_Field(77, 477, 482,	"I6",  	"-",		"Number of facility Data5 records");
		fields[77] = ERS_Field(78, 483, 490,	"I8",  	"-",		"Facility Data5 record maximum length");
		fields[78] = ERS_Field(79, 491, 496,	"I6",  	"-",		"Number of facility Data6 records");
		fields[79] = ERS_Field(80, 497, 504,	"I8",  	"-",		"Facility Data6 record maximum length");
		fields[80] = ERS_Field(81, 505, 510,	"I6",  	"-",		"Number of facility Data7 records");
		fields[81] = ERS_Field(82, 511, 518,	"I8",  	"-",		"Facility Data7 record maximum length");
		fields[82] = ERS_Field(83, 519, 524,	"I6",  	"-",		"Number of facility Data8 records");
		fields[83] = ERS_Field(84, 525, 532,	"I8",  	"-",		"Facility Data8 record maximum length");
		fields[84] = ERS_Field(85, 533, 538,	"I6",  	"-",		"Number of facility Data9 records");
		fields[85] = ERS_Field(86, 539, 546,	"I8",  	"-",		"Facility Data9 record maximum length");
		fields[86] = ERS_Field(87, 547, 552,	"I6",  	"-",		"Number of facility Data10 records");
		fields[87] = ERS_Field(88, 553, 560,	"I8",  	"-",		"Facility Data10 record maximum length");
		fields[88] = ERS_Field(89, 561, 566,	"I6",  	"-",		"Number of facility Data11 records");
		fields[89] = ERS_Field(90, 567, 574,	"I8",  	"-",		"Facility Data11 record maximum length");
		fields[90] = ERS_Field(91, 575, 580,	"I6",  	"-",		"Number of low resolution image data records");
		fields[91] = ERS_Field(92, 581, 586,	"I6",  	"-",		"low resolution image data record length");
		fields[92] = ERS_Field(93, 587, 592,	"I6",  	"px",		"Number of pixels of low resolution image data");
		fields[93] = ERS_Field(94, 593, 598,	"I6",  	"lines",	"Number of lines of low resolution image data");
		fields[94] = ERS_Field(95, 599, 604,	"I6",  	"samples",	"Number of bytes per one sample of low resolution image data");
		fields[95] = ERS_Field(96, 605, 720,	"A116",	"-",		"Blanks" );
		
		length = getRecordLength(data);
		int i=0;
		while(		i < num_fields
			  &&	 fields[i].end <= length){
			values[i] = getERSValue(data, fields[i]);
			++i;
		}
		if(values[91]!=""){
		char * temp_char = new char[6];
		memcpy(temp_char, data+581, 6); //get size of record length
		int low_image_size = atoi(temp_char);
		
		memcpy(temp_char, data+593, 6); //get count of lines
		low_image_size *= atoi(temp_char);
		
		low_image_size = max(0, low_image_size);
		
		// check if ther is an attached low-res image (only for PALSAR)
		if(low_image_size!=0){
			length += low_image_size;
		}
		}
	}
};

#endif