/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MDS1_SQ_ADS_HXX
#define MDS1_SQ_ADS_HXX

#include "envisat_field.hxx"
#include "envisat_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class MDS1_SQ_ADS: public EnvisatRecord{
	
public:
	MDS1_SQ_ADS(char* data){
		
		unsigned int num_fields=43;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ENVISAT_Field( 0,    1,  12, "MJD",			"MJD",		"zero_doppler_time",			"Zero doppler time at which Summary Quality information applies");
		fields[ 1] = ENVISAT_Field( 1,   13,  13, "BooleanFlag",	"flag",		"attach_flag",					"Set to 1 if all MDSRs corresponding to this ADSR are blank, set to zero otherwise. This flag will always be zero because this ADSR is updated once per slice or scene. Therefore, if there are no MDSRs, this ADSR is not produced at all.");
		fields[ 2] = ENVISAT_Field( 2, 	 14,  14, "BooleanFlag",	"flag",		"input_mean_flag",				"Input data mean outside nominal range flag  0 = mean of I and Q input values are both within specified range from expected mean. For expected mean of x, the measured mean must fall between x-threshold to x+threshold. 1 = otherwise");
		fields[ 3] = ENVISAT_Field( 3, 	 15,  15, "BooleanFlag",	"flag",		"input_std_dev_flag",			"Input data standard deviation outside nominal range flag 0 = standard deviation values of I and Q input values are both within specified range of expected standard deviation. For expected std. dev. x, the measured std. dev. must fall between x-threshold to x+threshold. 1 = otherwise"  );
		fields[ 4] = ENVISAT_Field( 4, 	 16,  16, "BooleanFlag",	"flag",		"input_gaps_flag",				"Significant gaps in the input data flag  0 = number of input gaps <= threshold value 1 = number of input data gaps > threshold value.");
		fields[ 5] = ENVISAT_Field( 5, 	 17,  17, "BooleanFlag",	"flag",		"input_missing_lines_flag",		"Missing lines significant flag 0 = percentage of missing lines <= threshold value 1 = percentage of missing lines > threshold value. The number of missing lines is the number of lines missing from the input data, excluding data gaps.");
		fields[ 6] = ENVISAT_Field( 6, 	 18,  18, "BooleanFlag",	"flag",		"dop_cen_flag",					"Doppler Centroid Uncertain flag  0 = confidence measure >= specified value 1 = confidence measure < specified value. If more than one Doppler centroid estimation is performed in a slice, the flag is set if any confidence measure is less than the threshold.");
		fields[ 7] = ENVISAT_Field( 7, 	 19,  19, "BooleanFlag",	"flag",		"dop_amb_flag",					"Doppler ambiguity estimate uncertain flag 0 = confidence measure >= specified value 1 = confidence measure < specified value");
		fields[ 8] = ENVISAT_Field( 8, 	 20,  20, "BooleanFlag",	"flag",		"output_mean_flag",				"Output data mean outside nominal range flag 0 = mean of I and Q output values for SLC image or mean of detected pixels for a detected product, are both within specified range from expected mean. For expected mean of x, the measured mean must fall between x-threshold to x+threshold. 1= otherwise.");
		fields[ 9] = ENVISAT_Field( 9, 	 21,  21, "BooleanFlag",	"flag",		"output_std_dev_flag",			"Output data standard deviation outside nominal range flag  0 = mean of I and Q output values for SLC image or mean of detected pixels for a detected product, are both within specified range from expected mean. For expected mean of x, the measured mean must fall between x-threshold to x+threshold. 1 = otherwise");
		fields[10] = ENVISAT_Field(10, 	 22,  22, "BooleanFlag",	"flag",		"chirp_flag",					"Chirp extraction failed or is of low quality flag 0 = able to reconstruct all chirps or chirp reconstruction not requested (nominal chirp used) AND all quality measures were acceptable. 1 = unable to reconstruct a chirp during processing and chirp reconstruction was requested or the quality is below the acceptable levels. When value = 1, (i.e. unable to reconstruct a chirp during processing and chirp reconstruction was requested or the quality is below the acceptable levels), PF-ASAR uses the nominal range pulse for processing and a nominal elevation beam scaling factor.");
		fields[11] = ENVISAT_Field(11, 	 23,  23, "BooleanFlag",	"flag",		"missing_data_sets_flag",		"Data sets missing flag  0 = all data sets which are supposed to be in the product are present 1 = any data sets (including ADSs) are missing from the product which are supposed to be included under normal circumstances. When value = 1 (i.e any data sets, including ADSs, are missing from the product which are supposed to be included under normal circumstances), which data sets are missing can be determined by an examination of the DSDs in the SPH.");
		fields[12] = ENVISAT_Field(12, 	 24,  24, "BooleanFlag",	"flag",		"invalid_downlink_flag",		"Invalid downlink parameters flag  0 = all parameters read from the downlinked data were valid 1 = displayed if any downlink parameter is out of range and therefore a default value has been used during processing.");
		fields[13] = ENVISAT_Field(13,   25,  31, "SpareField",		"-",		"spare_1",						"");
		fields[14] = ENVISAT_Field(14, 	 32,  35, "fl",				"%",		"thresh_chirp_broadening",		"Threshold for setting the chirp quality flag. Maximum percentage broadening permitted in cross-correlation pulse width compared to theoretical width." );
		fields[15] = ENVISAT_Field(15, 	 36,  39, "fl",				"dB",		"thresh_chirp_sidelobe",		"Threshold for setting the chirp quality flag. First sidelobe of the chirp cross correlation function" );
		fields[16] = ENVISAT_Field(16, 	 40,  43, "fl",				"dB",		"thresh_chirp_islr",			"Threshold for setting the chirp quality flag  ISLR of the chirp cross correlation function" );
		fields[17] = ENVISAT_Field(17, 	 44,  47, "fl",				"-",		"thresh_input_mean",			"Threshold for setting the mean of input data quality flag For an expected mean value of x, this is the value T, such that the measured mean must fall between the x-T and x+T." );
		fields[18] = ENVISAT_Field(18, 	 48,  51, "fl",				"-",		"exp_input_mean",				"Expected mean input value for this product for both I and Q." );
		fields[19] = ENVISAT_Field(19, 	 52,  55, "fl",				"-",		"thresh_input_std_dev",			"Threshold for setting the standard deviation of input data quality flag. For an expected standard deviation value of y, this is the value D, such that the measured standard deviation must fall between the y-D and y+D." );
		fields[20] = ENVISAT_Field(20, 	 56,  59, "fl",				"-",		"exp_input_std_dev",			"Expected input std. dev. for this product for both I and Q." );
		fields[21] = ENVISAT_Field(21, 	 60,  63, "fl",				"-",		"thresh_dop_cen",				"Threshold for setting the Doppler Centroid quality flag.  Threshold for Doppler Centroid confidence." );
		fields[22] = ENVISAT_Field(22, 	 64,  67, "fl",				"-",		"thresh_dop_amb",				"Threshold for setting the Doppler Centroid ambiguity quality flag. Threshold for setting the Doppler Centroid ambiguity confidence flag" );
		fields[23] = ENVISAT_Field(23, 	 68,  71, "fl",				"-",		"thresh_output_mean",			"Threshold for setting the mean of output data quality flag. For an expected mean value of x, this is the value T, such that the measured mean must fall between the x-T and x+T. " );
		fields[24] = ENVISAT_Field(24, 	 72,  75, "fl",				"-",		"exp_output_mean",				"Expected mean output value for this product. For an SLC product this is the expected mean of both the I and Q values. 	- 	1 	fl 	4 byte(s)" );
		fields[25] = ENVISAT_Field(25, 	 76,  79, "fl",				"-",		"thresh_output_std_dev",		"Threshold for setting the standard deviation of output data quality flag. For an expected standard deviation value of y, this is the value D, such that the measured standard deviation must fall between the y-D and y+D." );
		fields[26] = ENVISAT_Field(26, 	 80,  83, "fl",				"-",		"exp_output_std_dev",			"Expected output std. dev. for this product. For an SLC product this is the expected output std. dev. for both I and Q values." );
		fields[27] = ENVISAT_Field(27, 	 84,  87, "fl",				"-",		"thresh_input_missing_lines",	"Threshold for setting the missing lines quality flag: Maximum percentage of missing lines to total lines." );
		fields[28] = ENVISAT_Field(28, 	 88,  91, "fl",				"-",		"thresh_input_gaps",			"Threshold for setting the missing gaps quality flag: Maximum number of missing gaps allowed." );
		fields[29] = ENVISAT_Field(29, 	 92,  95, "fl",				"-",		"lines_per_gaps",				"Number of missing lines which constitute a gap lines" );
		fields[30] = ENVISAT_Field(30, 	 96, 110, "SpareField",		"-",		"spare_2",						"" );
		fields[31] = ENVISAT_Field(31, 	111, 114, "fl",				"-",		"input_mean_I",					"Input data mean I channel" );
		fields[32] = ENVISAT_Field(31, 	115, 118, "fl",				"-",		"input_mean_Q",					"Input data mean Q channel" );
		fields[33] = ENVISAT_Field(32, 	119, 122, "fl",				"-",		"input_std_dev_I",				"Input data standard deviation I channel" );
		fields[34] = ENVISAT_Field(32, 	123, 126, "fl",				"-",		"input_std_dev_Q",				"Input data standard deviation Q channel" );
		fields[35] = ENVISAT_Field(33, 	127, 130, "fl",				"-",		"num_gaps",						"Number of gaps Composed of a predetermined number of consecutive missing lines" );
		fields[36] = ENVISAT_Field(34, 	131, 134, "fl",				"-",		"num_missing_lines",			"Number of missing lines Excluding gaps" );
		fields[37] = ENVISAT_Field(35, 	135, 138, "fl",				"-",		"output_mean_I",				"Output data mean: For SLC products, for the I channel" );
		fields[38] = ENVISAT_Field(35, 	139, 142, "fl",				"-",		"output_mean_Q",				"Output data mean: For SLC products, for the Q channel. For detected products, second value is set to zero"  );
		fields[39] = ENVISAT_Field(36, 	143, 146, "fl",				"-",		"output_std_dev_I",				"Output data std deviation: For SLC products, for the I channel" );
		fields[40] = ENVISAT_Field(36, 	147, 150, "fl",				"-",		"output_std_dev_Q",				"Output data std deviation: For SLC products, for the Q channel. For detected products, second value is set to zero"  );
		fields[41] = ENVISAT_Field(37, 	151, 154, "fl",				"-",		"tot_errors",					"Total number of errors detected in isp headers" );
		fields[42] = ENVISAT_Field(38,  155, 170, "SpareField",		"-",		"spare_3",						"" );
		
		int i=0;
		while(		i < num_fields ){
			values[i] = getENVISATValue(data, fields[i]);
			++i;
		}
	}
};

#endif