/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef GEOLOCATION_GRID_ADS_HXX
#define GEOLOCATION_GRID_ADS_HXX

#include "envisat_field.hxx"
#include "envisat_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class GEOLOCATION_GRID_ADS: public EnvisatRecord{
	
public:
	GEOLOCATION_GRID_ADS(char* data){
		
		unsigned int num_fields=118;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ENVISAT_Field( 0,    1,  12, "MJD",			"MJD",		"first_zero_doppler_time",		"Zero doppler time in azimuth of first line of the granule. Gives azimuth location of grid line for first line of the granule.");
		fields[ 1] = ENVISAT_Field( 1,   13,  13, "BooleanFlag",	"flag",		"attach_flag",					"Set to 1 if all MDSRs corresponding to this ADSR are blank, set to zero otherwise. This flag will always be zero because this ADSR is updated once per slice or scene. Therefore, if there are no MDSRs, this ADSR is not produced at all.");
		fields[ 2] = ENVISAT_Field( 2, 	 14,  17, "ul",				"-",		"line_num",						"Range line number corresponding to the first line of the granule within the slice. Warning: (1) This is not always the record number of the corresponding image MDSR. Use the number of lines per granule field to determine the image MDS record corresponding to each record of tie points.(2) For a stripline product, which may consist of multiple slices in a single MDS, this number is reset to 1 at the beginning of each slice.(3) For child products, which are subsets of a full product, the range line number in the first record may not be 1.");
		fields[ 3] = ENVISAT_Field( 3, 	 18,  21, "ul",				"-",		"num_lines",					"Number of output lines in this granule"  );
		fields[ 4] = ENVISAT_Field( 4, 	 22,  25, "fl",				"-",		"sub_sat_track",				"Subsatellite track heading (relative to North) for first line of granule. This is the heading on the ground (includes Earth rotation)");
		for(unsigned int s =0; s< 11; ++s){
			fields[ 5+s] = ENVISAT_Field( 5, 	26+s*4,  29+s*4,	"ul", "-",	string("first_tie_point_") + lexical_cast<string>(s+1) +"_samp_number",	"Range sample number Gives the range location of the grid points. First range sample is 1, last is M");
		}
		for(unsigned int s =0; s< 11; ++s){
			fields[16+s] = ENVISAT_Field( 5, 	70+s*4,  73+s*4,	"fl", "ns",	string("first_tie_point_") + lexical_cast<string>(s+1) +"_slant_range_time",	"2 way slant range time to range sample");
		}
		for(unsigned int s =0; s< 11; ++s){
			fields[27+s] = ENVISAT_Field( 5, 	114+s*4,  117+s*4,	"fl", "deg.",	string("first_tie_point_") + lexical_cast<string>(s+1) +"_angle",	"Incidence Angle at range sample");
		}
		for(unsigned int s =0; s< 11; ++s){
			fields[38+s] = ENVISAT_Field( 5, 	158+s*4,  161+s*4,	"GeoCoordinate", "(1e-6) degrees",	string("first_tie_point_") + lexical_cast<string>(s+1) +"_lat",	"geodetic latitude (positive north)");
		}
		for(unsigned int s =0; s< 11; ++s){
			fields[49+s] = ENVISAT_Field( 5, 	202+s*4,  205+s*4,	"GeoCoordinate", "(1e-6) degrees",	string("first_tie_point_") + lexical_cast<string>(s+1) +"_long",	"geodetic longitude (positive east)");
		}
		fields[60] = ENVISAT_Field( 6, 	 246,  267, "SpareField",	"-",		"spare_1",		"");
		fields[61] = ENVISAT_Field( 7,   268,  279, "MJD",			"MJD",		"last_zero_doppler_time",			"Zero doppler time for the last line of the granule");
		for(unsigned int s =0; s< 11; ++s){
			fields[ 62+s] = ENVISAT_Field( 5, 	280+s*4,  283+s*4,	"ul", "-",	string("last_tie_point_") + lexical_cast<string>(s+1) +"_samp_number",	"Range sample number Gives the range location of the grid points. First range sample is 1, last is M");
		}
		for(unsigned int s =0; s< 11; ++s){
			fields[73+s] = ENVISAT_Field( 5, 	324+s*4,  327+s*4,	"fl", "ns",	string("last_tie_point_") + lexical_cast<string>(s+1) +"_slant_range_time",	"2 way slant range time to range sample");
		}
		for(unsigned int s =0; s< 11; ++s){
			fields[84+s] = ENVISAT_Field( 5, 	368+s*4,  371+s*4,	"fl", "deg.",	string("last_tie_point_") + lexical_cast<string>(s+1) +"_angle",	"Incidence Angle at range sample");
		}
		for(unsigned int s =0; s< 11; ++s){
			fields[95+s] = ENVISAT_Field( 5, 	412+s*4,  415+s*4,	"GeoCoordinate", "(1e-6) degrees",	string("last_tie_point_") + lexical_cast<string>(s+1) +"_lat",	"geodetic latitude (positive north)");
		}
		for(unsigned int s =0; s< 11; ++s){
			fields[106+s] = ENVISAT_Field( 5, 	456+s*4,  459+s*4,	"GeoCoordinate", "(1e-6) degrees",	string("last_tie_point_") + lexical_cast<string>(s+1) +"_long",	"geodetic longitude (positive east)");
		}
		fields[117] = ENVISAT_Field( 6, 	 500,  521, "SpareField",	"-",		"spare_2",		"");

		int i=0;
		while(		i < num_fields ){
			values[i] = getENVISATValue(data, fields[i]);
			++i;
		}
	}
};

#endif