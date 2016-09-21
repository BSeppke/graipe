/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DOP_CENTROID_COEFFS_ADS_HXX
#define DOP_CENTROID_COEFFS_ADS_HXX

#include "envisat_field.hxx"
#include "envisat_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class DOP_CENTROID_COEFFS_ADS : public EnvisatRecord {
	
public:
	DOP_CENTROID_COEFFS_ADS(char* data){
		
		unsigned int num_fields=16;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ENVISAT_Field( 0,    1,  12, "MJD",			"MJD",		"zero_doppler_time",			"Zero doppler time at which Summary Quality information applies");
		fields[ 1] = ENVISAT_Field( 1,   13,  13, "BooleanFlag",	"flag",		"attach_flag",					"Set to 1 if all MDSRs corresponding to this ADSR are blank, set to zero otherwise. This flag will always be zero because this ADSR is updated once per slice or scene. Therefore, if there are no MDSRs, this ADSR is not produced at all.");
		fields[ 2] = ENVISAT_Field( 2,   14,  17, "fl",				"ns",		"slant_range_time",				"2-way slant range time origin (t0)");
		
		for(unsigned int s=0; s<5; ++s)
			fields[ 3+s] = ENVISAT_Field( 3, 	 18+s*4,  21+s*4,	"fl",		string("Hz/(s^") + lexical_cast<string>(s) + ")",		string("dop_coef_") + lexical_cast<string>(s),				"Doppler centroid coefficients as a function of slant range time: D0, D1, D2, D3, and D4. Where Doppler Centroid = D0 + D1(tSR-t0) + D2(tSR-t0)2 + D3(tSR-t0)3 + D4(tSR-t0)4"  );
		
		fields[ 8] = ENVISAT_Field( 4, 	 38,  41, "fl",				"-",		"dop_conf",						"Doppler Centroid Confidence Measure.  Value between 0 and 1, 0 = poorest confidence, 1= highest confidence");
		fields[ 9] = ENVISAT_Field( 5, 	 42,  42, "BooleanFlag",	"flag",		"dop_conf_below_thresh_flag",	"Doppler Centroid Confidence below threshold flag. 0=confidence above threshold, centroid calculated from data; 1=confidence below threshold, centroid calculated from orbit parameters");
		
		for(unsigned int s=0; s<5; ++s)
			fields[10+s] = ENVISAT_Field( 6, 	 43+s*2,  44+s*2,	"ss",		"-",													string("delta_dopp_coef_") + lexical_cast<string>(s),		string("Delta Doppler coefficents DelatD0(SS") + lexical_cast<string>(s+1) + ")"  );
		
		fields[15] = ENVISAT_Field(7, 	 53, 55, "SpareField",		"-",		"spare_1",						"" );
		
		int i=0;
		while(		i < num_fields ){
			values[i] = getENVISATValue(data, fields[i]);
			++i;
		}
	}
};

#endif