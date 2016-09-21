/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ASAR_IMAGE_PRODUCTS_SPH_HXX
#define ASAR_IMAGE_PRODUCTS_SPH_HXX

#include "envisat_field.hxx"
#include "envisat_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class ASAR_Image_Products_SPH : public EnvisatRecord{
	
public:
	ASAR_Image_Products_SPH(char* data){
		
		unsigned int num_fields=32;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ENVISAT_Field(  2,   17,   44, "AsciiString",	"ascii",	"sph_descriptor" ,					"SPH Descriptor ASCII string describing the product");
		fields[ 1] = ENVISAT_Field(  6,   78,   81, "Ac",			"-",		"stripline_continuity_indicator",	"Strip line continuity indicator  Value = +000 for no stripline, otherwise is incremented for each stripline product in the orbit");
		fields[ 2] = ENVISAT_Field(  9,   98,  101, "Al",			"-",	"slice_position",					"Slice position in the stripline  Value: +001 to NUM_SLICES, Set to +001 if not stripline");
		fields[ 3] = ENVISAT_Field( 12,  114,  117, "Ac",			"-",		"num_slices",						"Number of slices in the stripline. Default = +001 for non-stripline product");
		
		fields[ 4] = ENVISAT_Field( 16,  136,  162, "UtcExternal",	"UTC",		"first_line_time",					"First Zero Doppler Azimuth time of product  UTC Time of first range line in the MDS of this product.");
		fields[ 5] = ENVISAT_Field( 21,  181,  207, "UtcExternal",	"UTC",		"last_line_time",					"Last Zero Doppler Azimuth time of product  UTC Time of last range line in the MDS of this product.");
		
		fields[ 6] = ENVISAT_Field( 25,  225,  235, "AsciiGeoCoordinate",	"10^-6 degrees",	"first_near_lat",					"Geodetic Latitude of the first sample of the first line A negative value denotes south latitude, a positive value denotes North latitude");
		fields[ 7] = ENVISAT_Field( 29,  263,  273, "AsciiGeoCoordinate",	"10^-6 degrees",	"first_near_long",					"East geodetic longitude of the first sample of the first line.Positive values East of Greenwich, negative values west of Greenwich.");
		fields[ 8] = ENVISAT_Field( 33,  299,  309, "AsciiGeoCoordinate",	"10^-6 degrees",	"first_mid_lat",					"Geodetic Latitude of the middle sample of the first line A negative value denotes south latitude, a positive value denotes North latitude");
		fields[ 9] = ENVISAT_Field( 37,  336,  346, "AsciiGeoCoordinate",	"10^-6 degrees",	"first_mid_long",					"East geodetic longitude of the middle sample of the first line.Positive values East of Greenwich, negative values west of Greenwich.");
		fields[10] = ENVISAT_Field( 41,  372,  382, "AsciiGeoCoordinate",	"10^-6 degrees",	"first_far_lat",					"Geodetic Latitude of the last sample of the first line A negative value denotes south latitude, a positive value denotes North latitude");
		fields[11] = ENVISAT_Field( 45,  409,  419, "AsciiGeoCoordinate",	"10^-6 degrees",	"first_far_long",					"East geodetic longitude of the last sample of the first line.Positive values East of Greenwich, negative values west of Greenwich.");
		
		fields[12] = ENVISAT_Field( 49,  445,  455, "AsciiGeoCoordinate",	"10^-6 degrees",	"last_near_lat",					"Geodetic Latitude of the first sample of the last line A negative value denotes south latitude, a positive value denotes North latitude");
		fields[13] = ENVISAT_Field( 53,  482,  492, "AsciiGeoCoordinate",	"10^-6 degrees",	"last_near_long",					"East geodetic longitude of the first sample of the last line.Positive values East of Greenwich, negative values west of Greenwich.");
		fields[14] = ENVISAT_Field( 57,  517,  527, "AsciiGeoCoordinate",	"10^-6 degrees",	"last_mid_lat",						"Geodetic Latitude of the middle sample of the last line A negative value denotes south latitude, a positive value denotes North latitude");
		fields[15] = ENVISAT_Field( 61,  553,  563, "AsciiGeoCoordinate",	"10^-6 degrees",	"last_mid_long",					"East geodetic longitude of the middle sample of the last line.Positive values East of Greenwich, negative values west of Greenwich.");
		fields[16] = ENVISAT_Field( 65,  588,  598, "AsciiGeoCoordinate",	"10^-6 degrees",	"last_far_lat",						"Geodetic Latitude of the last sample of the last line A negative value denotes south latitude, a positive value denotes North latitude");
		fields[17] = ENVISAT_Field( 69,  624,  634, "AsciiGeoCoordinate",	"10^-6 degrees",	"last_far_long",					"East geodetic longitude of the last sample of the last line.Positive values East of Greenwich, negative values west of Greenwich.");
		
		fields[18] = ENVISAT_Field( 75,  689,  691, "AsciiString",	"ascii",	"swath",							"Swath number: IS1, IS2, IS3, IS4, IS5, IS6, or IS7 for IM, and AP modes. Set to WS for WS and GM modes.");
		fields[19] = ENVISAT_Field( 80,  700,  709, "AsciiString",	"ascii",	"pass",								"Ascending or descending orbit designator (defined at start of time pass) ASCENDING, DESCENDING or FULLORBIT");
		
		fields[20] = ENVISAT_Field( 85,  725,  732, "AsciiString",	"ascii",	"sample_type",						"Detected or complex sample type designator:  DETECTED or COMPLEX");
		fields[21] = ENVISAT_Field( 90,  746,  752, "AsciiString",	"ascii",	"algorithm",						"Processing Algorithm Used: RAN/DOP or SPECAN");
		
		fields[22] = ENVISAT_Field( 95,  773,  775, "AsciiString",	"ascii",	"mds1_tx_rx_polar",					"Transmitter / Receiver Polarisation for MDS 1: H/V or H/H or V/H or V/V");
		fields[23] = ENVISAT_Field(100,  796,  798, "AsciiString",	"ascii",	"mds2_tx_rx_polar",					"Transmitter / Receiver Polarisation for MDS 2: H/V or H/H or V/H or V/V or blank for all modes with only one MDS.");
		
		fields[24] = ENVISAT_Field(105,  814,  818,  "AsciiString",	"ascii",	"compression",						"Compression algorithm used on echo data on-board the satellite:   FBAQ2, FBAQ3, FBAQ4 (FBAQ: 8 bits reduced to 2, 3, and 4 bits respectively). Others: S&M4 and NONE");
		fields[25] = ENVISAT_Field(109,  835,  838, "Ac",			"looks",	"azimuth_looks",					"Number of Looks in Azimuth");
		fields[26] = ENVISAT_Field(112,  852,  855, "Ac",			"looks",	"range_looks",						"Number of Looks in Range");
		fields[27] = ENVISAT_Field(115,  871,  885, "Afl",			"m",		"range_spacing",					"Range sample spacing in meters");
		fields[28] = ENVISAT_Field(119,  906,  920, "Afl",			"m",		"azimuth_spacing",					"Nominal azimuth sample spacing in meters");
		fields[29] = ENVISAT_Field(123,  944,  958, "Afl",			"s",		"line_time_interval",				"Azimuth sample spacing in time (Line Time Interval)");
		fields[30] = ENVISAT_Field(127,  975,  980, "As",			"samples",	"line_length",						"Number of samples per output line (includes zero filled samples): If a complex product, 1 sample = 1 I,Q pair, for a detected product, 1 sample = 1 pixel.");
		fields[31] = ENVISAT_Field(132, 1002, 1006, "AsciiString",	"ascii",	"data_type",						"Output data type: SWORD, UWORD, or UBYTE. The definition of a word here is a 16 bit integer");
		
		
		int i=0;
		while(		i < num_fields ){
			values[i] = getENVISATValue(data, fields[i]);
			++i;
		}
	}
};

#endif