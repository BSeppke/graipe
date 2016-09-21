/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MAIN_PRODUCT_HEADER_HXX
#define MAIN_PRODUCT_HEADER_HXX

#include "envisat_field.hxx"
#include "envisat_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class Main_Product_Header : public EnvisatRecord{
	
public:
	Main_Product_Header(char* data){
		
		unsigned int num_fields=34;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ENVISAT_Field(  2,   10,   71, "AsciiString",	"ascii",	"product",				"Product File name");
		fields[ 1] = ENVISAT_Field(  6,   85,   85, "AsciiString",	"ascii",	"proc_stage",			"Processing Stage FlagN = Near Real Time, T = test product, V= fully validated (fully consolidated) product, S = special product. Letters between N and V (with the exception of T and S) indicate steps in the consolidation process, with letters closer to V");
		fields[ 2] = ENVISAT_Field( 10,   96,  118, "AsciiString",	"ascii",	"ref_doc",				"Reference Document Describing Product AA-BB-CCC-DD-EEEE_V/I (23 characters, including blank space characters) where AA-BB-CCC-DD-EEEE is the ESA standard document no. and V/I is the Version / Issue. If the reference document is the Products Specifications PO-RS-MDA-GS-2009, the version and revision have to refer to the volume 1 of the document, where the status (version/revision) of all volumes can be found. If not used, set to ???????????????????????");
		fields[ 3] = ENVISAT_Field( 16,  183,  202, "AsciiString",	"ascii",	"acquisition_station",	"Acquisition Station ID (up to 3 codes) If not used, set to ????????????????????");
		fields[ 4] = ENVISAT_Field( 21,  218,  223, "AsciiString",	"ascii",	"proc_center",			"Processing Center ID which generated current product If not used, set to ??????");
		fields[ 5] = ENVISAT_Field( 26,  237,  263, "UtcExternal",	"UTC",		"proc_time",			"UTC Time of Processing (product generation time)UTC Time format. If not used, set to ???????????????????????????.");
		fields[ 6] = ENVISAT_Field( 31,  280,  293, "AsciiString",	"ascii",	"software_ver",			"Software Version number of processing softwareFormat: Name of processor (up to 10 characters)/ version number (4 characters) -- left justified (any blanks added at end). If not used, set to ??????????????.e.g. MIPAS/2.31????");
		
		fields[ 7] = ENVISAT_Field( 37,  352,  378, "UtcExternal",	"UTC",		"sensing_start",		"UTC start time of data sensing (first measurement in first data record) UTC Time format. If not used, set to ???????????????????????????.");
		fields[ 8] = ENVISAT_Field( 42,  395,  421, "UtcExternal",	"UTC",		"sensing_stop",			"UTC stop time of data sensing (last measurements last data record) UTC Time format. If not used, set to ???????????????????????????.");
		
		fields[ 9] = ENVISAT_Field( 47,  471,  471, "AsciiString",	"ascii",	"phase",				"Phasephase letter. If not used, set to X.");
		fields[10] = ENVISAT_Field( 50,  479,  482, "Ac",			"-",		"cycle",				"Cycle number. If not used, set to +000.");
		
		fields[11] = ENVISAT_Field( 53,  494,  499, "As",			"-",	"rel_orbit",			"Start relative orbit number If not used, set to +00000");
		fields[12] = ENVISAT_Field( 56,  511,  516, "As",			"-",	"abs_orbit",			"Start relative orbit number If not used, set to +00000");
		
		fields[13] = ENVISAT_Field( 60,  537,  563, "UtcExternal",	"UTC",		"state_vector_time",	"UTC of ENVISAT state vector. UTC time format. If not used, set to ???????????????????????????.");
		fields[14] = ENVISAT_Field( 64,  576,  583, "Ado06",	"s",	"delta_ut1",			"DUT1=UT1-UTC. If not used, set to +.000000.");
		fields[15] = ENVISAT_Field( 68,  599,  610, "Ado73",	"m",	"x_position",			"X Position in Earth-Fixed reference. If not used, set to +0000000.000.");
		fields[16] = ENVISAT_Field( 72,  626,  637, "Ado73",	"m",	"y_position",			"Y Position in Earth-Fixed reference. If not used, set to +0000000.000.");
		fields[17] = ENVISAT_Field( 76,  653,  664, "Ado73",	"m",	"z_position",			"Z Position in Earth-Fixed reference. If not used, set to +0000000.000.");
		fields[18] = ENVISAT_Field( 80,  680,  691, "Ado73",	"m/s",	"x_velocity",			"X Velocity in Earth-Fixed reference. If not used, set to +0000000.000.");
		fields[19] = ENVISAT_Field( 84,  709,  720, "Ado73",	"m/s",	"y_velocity",			"Y Velocity in Earth-Fixed reference. If not used, set to +0000000.000.");
		fields[20] = ENVISAT_Field( 88,  738,  749, "Ado73",	"m/s",	"z_velocity",			"Z Velocity in Earth-Fixed reference. If not used, set to +0000000.000.");
		fields[21] = ENVISAT_Field( 93,  771,  772, "AsciiString",	"ascii",	"vector_source",		"Source of Orbit Vectors");
		
		fields[22] = ENVISAT_Field( 99,	 830,  856, "UtcExternal",	"UTC",		"utc_sbt_time",			"UTC time corresponding to SBT below(currently defined to be given at the time of the ascending node state vector). If not used, set to ???????????????????????????.");
		fields[23] = ENVISAT_Field(103,	 875,  885, "Al",			"-",		"sat_binary_time",		"Satellite Binary Time (SBT) 32bit integer time of satellite clock. Its value is unsigned (=>0). If not used, set to +0000000000.");
		fields[24] = ENVISAT_Field(106,	 898,  908, "Al",			"ps",		"clock_step",			"Clock Step Sizeclock step in picoseconds. Its value is unsigned (=>0). If not used, set to +0000000000.");
		fields[25] = ENVISAT_Field(112,	 957,  983, "UtcExternal",	"UTC",		"leap_utc",				"UTC time of the occurrence of the Leap SecondSet to ??????????????????????????? if not used.");
		fields[26] = ENVISAT_Field(116,	 996,  999,  "Ac",			"s",		"leap_sign",			"Leap second sign(+001 if positive Leap Second, -001 if negative)Set to +000 if not used.");
		fields[27] = ENVISAT_Field(119,	1010, 1010, "AsciiString",	"ascii",	"leap_err",				"Leap second error if leap second occurs within processing segment = 1, otherwise = 0If not used, set to 0.");
		
		fields[28] = ENVISAT_Field(123,	1065, 1065, "AsciiString",	"ascii",	"product_err",			"1 or 0. If 1, errors have been reported in the product. User should then refer to the SPH or Summary Quality ADS of the product for details of the error condition. If not used, set to 0.");
		fields[29] = ENVISAT_Field(126,	1076, 1096, "Ad",			"bytes",	"tot_size",				"Total Size Of Product (# bytes DSR + SPH+ MPH)");
		fields[30] = ENVISAT_Field(130,	1114, 1124, "Al",			"bytes",	"sph_size",				"Length Of SPH(# bytes in SPH)");
		fields[31] = ENVISAT_Field(134, 1141, 1151, "Al",			"-",		"num_dsd",				"Number of DSDs(# DSDs)");
		fields[32] = ENVISAT_Field(137,	1162, 1172, "Al",			"-",		"dsd_size",				"Length of Each DSD(# bytes for each DSD, all DSDs shall have the same length)");
		fields[33] = ENVISAT_Field(141,	1195, 1205, "Al",			"-",		"num_data_sets",		"Number of DSs attached(not all DSDs have a DS attached)");
		
		
		int i=0;
		while(		i < num_fields ){
			values[i] = getENVISATValue(data, fields[i]);
			++i;
		}
	}
	vector<ENVISAT_Field> fields;
	vector<string> values;
};

#endif