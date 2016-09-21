/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MDS1_ANTENNA_ELEV_PATT_ADS_HXX
#define MDS1_ANTENNA_ELEV_PATT_ADS_HXX

#include "envisat_field.hxx"
#include "envisat_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class MDS1_ANTENNA_ELEV_PATT_ADS : public EnvisatRecord{
	
public:
	MDS1_ANTENNA_ELEV_PATT_ADS(char* data){
		
		unsigned int num_fields=37;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ENVISAT_Field( 0,    1,  12, "MJD",			"MJD",		"first_zero_doppler_time",		"Zero doppler time in azimuth of first line of the granule. Gives azimuth location of grid line for first line of the granule.");
		fields[ 1] = ENVISAT_Field( 1,   13,  13, "BooleanFlag",	"flag",		"attach_flag",					"Set to 1 if all MDSRs corresponding to this ADSR are blank, set to zero otherwise. This flag will always be zero because this ADSR is updated once per slice or scene. Therefore, if there are no MDSRs, this ADSR is not produced at all.");
		fields[ 2] = ENVISAT_Field( 2, 	 14,  17, "AsciiString",	"ascii",	"swath",						"Beam ID to which pattern applies SS1 to SS5 or NS");
		for(unsigned int s =0; s< 11; ++s){
			fields[3+s] = ENVISAT_Field( 3, 	18+s*4,  21+s*4,	"fl", "ns",		string("slant_range_time_") + lexical_cast<string>(s+1),	"2 way slant range time to range sample");
		}
		for(unsigned int s =0; s< 11; ++s){
			fields[14+s] = ENVISAT_Field( 3, 	62+s*4,  65+s*4,	"fl", "deg.",	string("elevation_angle_") + lexical_cast<string>(s+1),	"Corresponding elevation angle");
		}
		for(unsigned int s =0; s< 11; ++s){
			fields[25+s] = ENVISAT_Field( 3, 	106+s*4,  109+s*4,	"fl", "dB",		string("antenna_pattern_") + lexical_cast<string>(s+1),	"Corresponding two-way antenna elevation pattern values");
		}
		fields[36] = ENVISAT_Field( 4, 	 150,  162, "SpareField",	"-",		"spare_1",		"");

		int i=0;
		while(		i < num_fields ){
			values[i] = getENVISATValue(data, fields[i]);
			++i;
		}
	}
};

#endif