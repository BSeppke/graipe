/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SR_GR_ADS_HXX
#define SR_GR_ADS_HXX

#include "envisat_field.hxx"
#include "envisat_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class SR_GR_ADS : public EnvisatRecord{
	
public:
	SR_GR_ADS(char* data){
		
		unsigned int num_fields=10;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ENVISAT_Field( 0,    1,  12, "MJD",			"MJD",		"zero_doppler_time",			"Zero doppler time at which Summary Quality information applies");
		fields[ 1] = ENVISAT_Field( 1,   13,  13, "BooleanFlag",	"flag",		"attach_flag",					"Set to 1 if all MDSRs corresponding to this ADSR are blank, set to zero otherwise. This flag will always be zero because this ADSR is updated once per slice or scene. Therefore, if there are no MDSRs, this ADSR is not produced at all.");
		fields[ 2] = ENVISAT_Field( 2,   14,  17, "fl",				"ns",		"slant_range_time",				"2-way slant range time to first range sample");
		fields[ 3] = ENVISAT_Field( 3,   18,  21, "fl",				"m",		"ground_range_origin",			"Ground range origin of the polynomial (GR0). Measured from the first pixel of the line");
		
		fields[ 4] = ENVISAT_Field( 4,   22,  25, "fl",				"m",		"srgr_coeff_0",			"The coefficient S0 of the ground range to slant range conversion polynomial. Slant range = S0 + S1(GR-GR0) + S2 (GR-GR0)2 + S3(GR-GR0)3 + S4(GR-GR0)4, where GR is the ground range distance from the first pixel of the range line.");
		fields[ 5] = ENVISAT_Field( 4,   26,  29, "fl",				"-",		"srgr_coeff_1",			"The coefficient S1 of the ground range to slant range conversion polynomial. Slant range = S0 + S1(GR-GR0) + S2 (GR-GR0)2 + S3(GR-GR0)3 + S4(GR-GR0)4, where GR is the ground range distance from the first pixel of the range line.");
		fields[ 6] = ENVISAT_Field( 4,   30,  33, "fl",				"m^-1",		"srgr_coeff_2",			"The coefficient S2 of the ground range to slant range conversion polynomial. Slant range = S0 + S1(GR-GR0) + S2 (GR-GR0)2 + S3(GR-GR0)3 + S4(GR-GR0)4, where GR is the ground range distance from the first pixel of the range line.");
		fields[ 7] = ENVISAT_Field( 4,   34,  37, "fl",				"m^-2",		"srgr_coeff_3",			"The coefficient S3 of the ground range to slant range conversion polynomial. Slant range = S0 + S1(GR-GR0) + S2 (GR-GR0)2 + S3(GR-GR0)3 + S4(GR-GR0)4, where GR is the ground range distance from the first pixel of the range line.");
		fields[ 8] = ENVISAT_Field( 4,   38,  41, "fl",				"m^-3",		"srgr_coeff_4",			"The coefficient S4 of the ground range to slant range conversion polynomial. Slant range = S0 + S1(GR-GR0) + S2 (GR-GR0)2 + S3(GR-GR0)3 + S4(GR-GR0)4, where GR is the ground range distance from the first pixel of the range line.");
		fields[ 9] = ENVISAT_Field( 5,   42,  55, "SpareField",		"-",		"spare_1",						"" );
		
		int i=0;
		while(		i < num_fields ){
			values[i] = getENVISATValue(data, fields[i]);
			++i;
		}
	}
};

#endif