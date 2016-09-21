/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CHIRP_PARAMS_ADS_HXX
#define CHIRP_PARAMS_ADS_HXX

#include "envisat_field.hxx"
#include "envisat_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class CHIRP_PARAMS_ADS : public EnvisatRecord{
	
public:
	CHIRP_PARAMS_ADS(char* data){
		
		unsigned int num_fields=367;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ENVISAT_Field( 0,    1,  12, "MJD",			"MJD",		"zero_doppler_time",		"Zero doppler time at which Summary Quality information applies");
		fields[ 1] = ENVISAT_Field( 1,   13,  13, "BooleanFlag",	"flag",		"attach_flag",				"Always set to zero for this ADSR");
		fields[ 2] = ENVISAT_Field( 2,   14,  16, "AsciiString",	"ascii",	"swath",					"Beam ID: SS1, SS2, SS3, SS4, or SS5 for WS and GM images. Set to NS for AP, IM, and WV images.");
		fields[ 3] = ENVISAT_Field( 3,   17,  19, "AsciiString",	"ascii",	"polar",					"Tx/Rx polarisation: H/H, H/V, V/V, or V/H");
		fields[ 4] = ENVISAT_Field( 4,   20,  23, "fl",				"samples",	"chirp_width",				"3-dB pulse width of chirp replica cross-correlation function between reconstructed chirp and nominal chirp");
		fields[ 5] = ENVISAT_Field( 5,   24,  27, "fl",				"dB",		"chirp_sidelobe",			"First side lobe level of chirp replica cross-correlation function between reconstructed chirp and nominal chirp");
		fields[ 6] = ENVISAT_Field( 6,   28,  31, "fl",				"dB",		"chirp_islr",				"ISLR of chirp replica cross-correlation function between reconstructed chirp and nominal chirp");
		fields[ 7] = ENVISAT_Field( 7,   32,  35, "fl",				"samples",	"chirp_peak_loc",			"Peak location of cross-correlation function between reconstructed chirp and nominal chirp");
		fields[ 8] = ENVISAT_Field( 8,   36,  39, "fl",				"dB",		"re_chirp_power",			"Reconstrcted Chirp powe");
		fields[ 9] = ENVISAT_Field( 9,   40,  43, "fl",				"dB",		"elev_chirp_power",			"Equivalent chirp Power");
		
		fields[10] = ENVISAT_Field(10,   44,  44, "BooleanFlag",	"flag",		"chirp_quality_flag",		"Reconstructed chirp exceeds quality thresholds. 0 = reconstructed chirp below quality thresholds, current chirp is the nominal chirp. 1 = reconstructed chirp above quality thresholds, current chirp is the reconstructed chirp.");
		fields[11] = ENVISAT_Field(11,   45,  48, "fl",				"dB",		"ref_chirp_power",			"Reference chirp Power");
		fields[12] = ENVISAT_Field(12,   49,  55, "AsciiString",	"ascii",	"normalization_source",		"Normalization source REPLICA, REF0000, EQV0000, NONE0000");
		
		fields[13] = ENVISAT_Field(13,   56,  59, "SpareField",		"-",		"spare_1",						"" );
		
		for(unsigned int s=0; s<32; ++s){
			fields[14+s*11] = ENVISAT_Field(14,   60+s*44,  63+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_max_cal_1",			"Max of Cal pulse 1 amplitude");
			fields[15+s*11] = ENVISAT_Field(14,   64+s*44,  67+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_max_cal_2",			"Max of Cal pulse 2 amplitude");
			fields[16+s*11] = ENVISAT_Field(14,   68+s*44,  71+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_max_cal_3",			"Max of Cal pulse 3 amplitude");
			fields[17+s*11] = ENVISAT_Field(14,   72+s*44,  75+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_avg_cal_1",			"Average of Cal pulse 1 amplitude over the 3 dB on either side of the max amplitude");
			fields[18+s*11] = ENVISAT_Field(14,   76+s*44,  79+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_avg_cal_2",			"Average of Cal pulse 2 amplitude over the 3 dB on either side of the max amplitude");
			fields[19+s*11] = ENVISAT_Field(14,   80+s*44,  83+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_avg_cal_3",			"Average of Cal pulse 3 amplitude over the 3 dB on either side of the max amplitude");
			fields[20+s*11] = ENVISAT_Field(14,   84+s*44,  87+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_avg_val_1a",			"Average of Cal pulse 1A over the sample window");
			fields[21+s*11] = ENVISAT_Field(14,   88+s*44,  91+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_phs_cal_1",			"Extracted phase for calibration pulse 1, 1A, 2, and 3");
			fields[22+s*11] = ENVISAT_Field(14,   92+s*44,  95+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_phs_cal_1A",			"Extracted phase for calibration pulse 1, 1A, 2, and 3");
			fields[23+s*11] = ENVISAT_Field(14,   96+s*44,  99+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_phs_cal_2",			"Extracted phase for calibration pulse 1, 1A, 2, and 3e");
			fields[24+s*11] = ENVISAT_Field(14,  100+s*44, 103+s*44, "fl",		"-",	string("cal_pulse_info_") + lexical_cast<string>(s+1) + "_phs_cal_3",			"Extracted phase for calibration pulse 1, 1A, 2, and 3");
		}
		
		fields[366] = ENVISAT_Field(15,   1468,  1483, "SpareField",		"-",		"spare_1",						"" );
		
		int i=0;
		while(		i < num_fields ){
			values[i] = getENVISATValue(data, fields[i]);
			++i;
		}
	}
};

#endif