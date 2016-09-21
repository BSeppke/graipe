/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MAIN_PROCESSING_PARAMS_ADS_HXX
#define MAIN_PROCESSING_PARAMS_ADS_HXX

#include "envisat_field.hxx"
#include "envisat_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class MAIN_PROCESSING_PARAMS_ADS: public EnvisatRecord{
	
public:
	MAIN_PROCESSING_PARAMS_ADS(char* data){
		
		unsigned int num_fields=389;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[ 0] = ENVISAT_Field( 0,	 1,  12,	"MJD",				"MJD",		"first_zero_doppler_time",				"First Zero Doppler Azimuth time of MDS which this data set describesTime of first range line in the MDS described by this data set");
		fields[ 1] = ENVISAT_Field( 1,	13,  13,	"BooleanFlag",		"flag",		"attach_flag",							"Always set to zero for this ADSR.");
		fields[ 2] = ENVISAT_Field( 2,  14,  25,	"MJD",				"MJD",		"last_zero_doppler_time",				"Last Zero Doppler Azimuth time of MDS which this data set describesTime of last range line in the MDS described by this data set");
		fields[ 3] = ENVISAT_Field( 3, 	26,  37,	"AsciiString",		"ascii",	"work_order_id",						"Work Order ID (left-justified)"  );
		fields[ 4] = ENVISAT_Field( 4, 	38,  41,	"fl",				"s",		"time_diff",							"Time difference between sensing time of first input line and zero Doppler time of first output image line (tdelta). May be used during child product extraction from a stripline product. Left blank (set to zero) for non-stripline products");
		fields[ 5] = ENVISAT_Field( 5, 	42,  44,	"AsciiString",		"ascii",	"swath_num",							"Swath number IS1, IS2, IS3, IS4, IS5, IS6, or IS7 for IM, WV and AP modes.SS1 ,SS2, SS3, SS4, SS5 for WSS Products Ws0 for WS and GM modes where only one ADSR for the whole scene is provided");
		fields[ 6] = ENVISAT_Field( 6,  45,  48,	"fl",				"m",		"range_spacing",						"Range sample spacing");
		fields[ 7] = ENVISAT_Field(7,	49,  52,	"fl",				"m",		"azimuth_spacing",						"Azimuth sample spacing at image center");
		fields[ 8] = ENVISAT_Field(8,	53,	 56,	"fl",				"s",		"line_time_interval",					"Azimuth sample spacing in time (Line Time Interval)" );
		fields[ 9] = ENVISAT_Field(9,   57,  60, 	"ul",				"lines",	"num_output_lines",						"Number of output range lines in the image.For WSS products this number will vary for each sub swath" );
		fields[10] = ENVISAT_Field(10,  61,  64,    "ul",				"lines",	"num_samples_per_line" ,				"Number of samples per output range line includes zero filled samples");
		fields[11] = ENVISAT_Field(11,  65,  69,    "AsciiString",		"ascii",	"data_type",							"Output data typeSWORD, UWORD, or UBYTE");
		fields[12] = ENVISAT_Field(12,  70,  73,    "ul",				"lines",	"num_range_lines_per_burst",			"Number of output range lines per burst. Not used for single-beam products");
		fields[13] = ENVISAT_Field(13,  74,  77,    "fl",				"s",		"time_diff_zero_doppler" ,				"Time difference between zero Doppler time and acquisition time of output image lines" );
		fields[14] = ENVISAT_Field(14,  78, 120,  	"SpareField",		"-",		"spare_1",								""   );
		fields[15] = ENVISAT_Field(15, 121, 121,	"BooleanFlag",		"flag",		"data_analysis_flag",					"Raw Data Analysis used for Raw Data Correction : 0 = correction done using default parameters 1 = correction done using raw data analysis results");
		fields[16] = ENVISAT_Field(16, 122, 122,    "BooleanFlag",		"flag",		"ant_elev_corr_flag",					"Antenna Elevation Pattern Correction Applied: 0 = no correction applied 1 = correction applied" );
		fields[17] = ENVISAT_Field(17, 123, 123,   	"BooleanFlag",		"flag",		"chirp_extract_flag",					"Reconstructed Chirp to be used (if reconstruction successful): 0 = nominal chirp replica to be used 1 = reconstructed chirp to be used");
		fields[18] = ENVISAT_Field(18, 124, 124,  	"BooleanFlag",		"flag",		"srgr_flag",							"Slant Range to Ground Range Conversion Applied:  0 = no conversion applied 1 = conversion applied" );
		fields[19] = ENVISAT_Field(19, 125, 125,  	"BooleanFlag",		"flag",		"dop_cen_flag",							"Doppler Centroid Estimation Performed:  0 = no estimation done 1 = estimation done");
		fields[20] = ENVISAT_Field(20, 126, 126,    "BooleanFlag",		"flag",		 "dop_amb_flag",						"Doppler Ambiguity Estimation Performed:	0 = no estimate done 1 = estimate done");
		fields[21] = ENVISAT_Field(21, 127, 127,    "BooleanFlag",		"flag",		 "range_spread_comp_flag",				"Range-spreading loss compensation Applied:  0 = no compensation applied 1 = compensation applied");
		fields[22] = ENVISAT_Field(22, 128, 128,    "BooleanFlag",		"flag",		 "detected_flag",						"Detection Applied:  0 = output product is complex 1 = output product was detected" );
		fields[23] = ENVISAT_Field(23, 129, 129,    "BooleanFlag",		"flag",		 "look_sum_flag",						"Look Summation Performed: 0 = product is single look 1 = product is multi-looked");
		fields[24] = ENVISAT_Field(24, 130, 130,    "BooleanFlag",		"flag",		"rms_equal_flag",						"RMS Equalisation performed: 0= rms equalization not performed during FBAQ decoding, 1 = rms equalization performed during FBAQ decoding");
		fields[25] = ENVISAT_Field(25, 131, 131,    "BooleanFlag",		"flag",		"ant_scal_flag",						"Antenna Elevation Gain Scaling Factor  applied 0= no scaling factor applied, 1 = scaling factor applied" );
		fields[26] = ENVISAT_Field(26, 132, 132,    "BooleanFlag",		"flag",		"vga_com_echo_flag",					"Receive Gain Droop Compensation applied to Echo. 0=no compression, 1=compensation applied." );
		fields[27] = ENVISAT_Field(27, 133, 133,    "BooleanFlag",		"flag",		"vga_com_cal_flag" ,					"Receive Gain Droop Compensation applied to Calibration Pulse P2. 0=no compression, 1=compensation applied."	);
		fields[28] = ENVISAT_Field(28, 134, 134,    "BooleanFlag",		"flag",		"ga_com_nom_time_flag",					"Nominal time delay applied for Receive Gain Droop Compensation of Calibration Pulse P2 order zero. 0=do not use nominal time delay (compensation depends on P2 time delay), 1= use nominal time delay (constant)"	);
		fields[29] = ENVISAT_Field(29, 135, 135,    "BooleanFlag",		"flag",		"gm_range_comp_inverse_filter_flag",	"Inverse filter used for range compression.  (GM Mode only). 0 = matched filter used for range compression 1 = inverse filter used for range compression"	);
		
		fields[30] = ENVISAT_Field(30, 136, 141,	"SpareField",		"-",		"spare_2",								"");
		
		for(unsigned int s=0; s<2; ++s){
			fields[31+s*26] = ENVISAT_Field(31, 142+s*92,	145+s*92,		"ul",				"gaps",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_num_gaps",				"Number of input data gaps (a gap is defined as a predetermined number of range lines)");
			fields[32+s*26] = ENVISAT_Field(31, 146+s*92,	149+s*92,		"ul",				"lines",	string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_num_missing_lines",	"Number of missing lines, excluding data gaps" );
			fields[33+s*26] = ENVISAT_Field(31, 150+s*92,	153+s*92,		"ul",				"samples",	string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_range_samp_skip",		"Range sample skipping factor for raw data analysis");
			fields[34+s*26] = ENVISAT_Field(31, 154+s*92,	157+s*92,		"ul",				"lines",	string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_range_lines_skip",		"Range lines skipping factor for raw data analysis");
			fields[35+s*26] = ENVISAT_Field(31, 158+s*92,	161+s*92,		"fl",				"-"	,		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_calc_i_bias",			"Calculated I channel bias");
			fields[36+s*26] = ENVISAT_Field(31, 162+s*92,	165+s*92,		"fl",				"-"	,		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_calc_q_bias",			"Calculated Q channel bias" );
			fields[37+s*26] = ENVISAT_Field(31, 166+s*92,	169+s*92,		"fl",				"-"	,		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_calc_i_std_dev",		"Calculated I standard deviation" );
			fields[38+s*26] = ENVISAT_Field(31, 170+s*92,	173+s*92,		"fl",				"-"	,		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_calc_q_std_dev",		"Calculated Q standard deviation"  );
			fields[39+s*26] = ENVISAT_Field(31, 174+s*92,	177+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_calc_gain",			"Calculated I/Q gain imbalance");
			fields[40+s*26] = ENVISAT_Field(31, 178+s*92,	181+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_calc_quad",			"Calculated I/Q quadrature departure");
			fields[41+s*26] = ENVISAT_Field(31, 182+s*92,	185+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_i_bias_max",			"I bias upper bound");
			fields[42+s*26] = ENVISAT_Field(31, 186+s*92,	189+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_i_bias_min",			"I bias lower bound" );
			fields[43+s*26] = ENVISAT_Field(31, 190+s*92,	193+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_q_bias_max",			"Q bias upper bound");
			fields[44+s*26] = ENVISAT_Field(31, 194+s*92,	197+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_q_bias_min",			"Q bias lower bound" );
			fields[45+s*26] = ENVISAT_Field(31, 198+s*92,	201+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_gain_min  ",			"I/Q lower bound" );
			fields[46+s*26] = ENVISAT_Field(31, 202+s*92,	205+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_gain_max",				"I/Q upper bound");
			fields[47+s*26] = ENVISAT_Field(31, 206+s*92,	209+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_quad_min",				"I/Q quadrature departure lower bound" );
			fields[48+s*26] = ENVISAT_Field(31, 210+s*92,	213+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_quad_max",				"I/Q quadrature departure upper bound");
			fields[49+s*26] = ENVISAT_Field(31, 214+s*92,	214+s*92,		"BooleanFlag",		"flag",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_i_bias_flag" ,			"I bias significance: 0 = I bias falls within acceptable range, 1 = I bias falls outside acceptable range");
			fields[50+s*26] = ENVISAT_Field(31, 215+s*92,	215+s*92,		"BooleanFlag",		"flag",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_q_bias_flag" ,			"Q bias significance: 0 = Q bias falls within acceptable range, 1 = Q bias falls outside acceptable range");
			fields[51+s*26] = ENVISAT_Field(31, 216+s*92,	216+s*92,		"BooleanFlag",		"flag",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_gain_flag",			"I/Q Gain Significance: 0 = Gain falls within acceptable range, 1 = Gain falls outside acceptable range");
			fields[52+s*26] = ENVISAT_Field(31, 217+s*92,	217+s*92,		"BooleanFlag",		"flag",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_quad_flag",			"I/Q Quadrature Departure Significance: 0 = Quadrature departure falls within acceptable range, 1 =Quadrature departure falls outside acceptable range");
			fields[53+s*26] = ENVISAT_Field(31, 218+s*92,	221+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_used_i_bias",			"I channel bias used for correction (may be different from measured value)");
			fields[54+s*26] = ENVISAT_Field(31, 222+s*92,	225+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_used_q_bias",			"Q channel bias used for correction (may be different from measured value)" );
			fields[55+s*26] = ENVISAT_Field(31, 226+s*92,	229+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_used_gain",			"I/Q gain imbalance  used for correction (may be different from measured value)"  );
			fields[56+s*26] = ENVISAT_Field(31, 230+s*92,	233+s*92,		"fl",				"-",		string("raw_data_analysis_") + lexical_cast<string>(s+1) + "_used_quad",			"I/Q quadrature departure used for correction (may be different from measured value)" );
		}
		
		fields[83] = ENVISAT_Field(32, 326, 357,   "SpareField",		"-",	"spare_3", "");
		
		for(unsigned int s=0; s<2; ++s){
			fields[84+s*3] = ENVISAT_Field(33,  358+s*20, 361+s*20,		"ul",	"-",	string("start_time_MDS_") + lexical_cast<string>(s+1) + "_first_obt_hsb",	"On-board binary time of first input line processed HSB.(First of two long integers)");
			fields[85+s*3] = ENVISAT_Field(33,  362+s*20, 365+s*20,		"ul",	"-",	string("start_time_MDS_") + lexical_cast<string>(s+1) + "_first_obt_lsb",	"On-board binary time of first input line processed LSB accurate to 15.26 ms. (Second of two long integers)" );
			fields[86+s*3] = ENVISAT_Field(33,  366+s*20, 377+s*20,		"MJD",	"MJD",	string("start_time_MDS_") + lexical_cast<string>(s+1) + "_first_mjd",   	"Sensing time (MJD format) of first input line processedconverted from satellite binary time"   );
		}
		
		for(unsigned int s=0; s<5; ++s)
			fields[ 90+s] = ENVISAT_Field(34, 398+s*2, 399+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_swst_code",				"Sampling Window Start time code of first processed line");
		for(unsigned int s=0; s<5; ++s)
			fields[ 95+s] = ENVISAT_Field(34, 408+s*2, 409+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_last_swst_code",		"Sampling Window Start time code of last processed line");
		for(unsigned int s=0; s<5; ++s)
			fields[100+s] = ENVISAT_Field(34, 418+s*2, 419+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_pri_code",				"Pulse Repetition Interval code");
		for(unsigned int s=0; s<5; ++s)	
			fields[105+s] = ENVISAT_Field(34, 428+s*2, 429+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_tx_pulse_len_code",		"Tx pulse length");
		for(unsigned int s=0; s<5; ++s)
			fields[110+s] = ENVISAT_Field(34, 438+s*2, 439+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_tx_bw_code",			"Tx pulse bandwidth");
		for(unsigned int s=0; s<5; ++s)
			fields[115+s] = ENVISAT_Field(34, 448+s*2, 449+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_echo_win_len_code",		"Echo Window Length");
		for(unsigned int s=0; s<5; ++s)
			fields[120+s] = ENVISAT_Field(34, 458+s*2, 459+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_up_code",				"Upconverter Level - Upconverter gain set on the instrument");
		for(unsigned int s=0; s<5; ++s)
			fields[125+s] = ENVISAT_Field(34, 468+s*2, 469+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_down_code",				"Downconverter Level - Downconverter gain set on the instrument");
		for(unsigned int s=0; s<5; ++s)
			fields[130+s] = ENVISAT_Field(34, 478+s*2, 479+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_resamp_code" ,			"Resampling factor for echo data");
		for(unsigned int s=0; s<5; ++s)
			fields[135+s] = ENVISAT_Field(34, 488+s*2, 489+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_beam_adj_code",			"Beam adjustment delta");
		for(unsigned int s=0; s<5; ++s)
			fields[140+s] = ENVISAT_Field(34, 498+s*2, 499+s*2, "us", "code",  string("parameter_code_") + lexical_cast<string>(s+1) + "_beam_set_num_code",	"Antenna Beam Set Number");
		for(unsigned int s=0; s<5; ++s)
			fields[145+s] = ENVISAT_Field(34, 508+s*2, 509+s*2, "us", "code", string("parameter_code_") + lexical_cast<string>(s+1) + "_tx_monitor_code",		"Auxiliary Tx Monitor Level");
		
		fields[150] = ENVISAT_Field(35, 518, 577,	"SpareField",	"-",	"spare_4", "");
		
		fields[151] = ENVISAT_Field(36, 578, 581,	"ul",		"-",		 "num_err_swst","Number of errors detected in Sampling Window start time field.");
		fields[152] = ENVISAT_Field(36, 582, 585,	"ul",		"-",		 "num_err_pri","Number of errors detected in PRI code field");
		fields[153] = ENVISAT_Field(36, 586, 589,	"ul",		"-",		 "num_err_tx_pulse_len","Number of errors detected in Tx pulse length field");
		fields[154] = ENVISAT_Field(36, 590, 593,	"ul",		"-",		 "num_err_tx_pulse_bw","Number of errors detected in Tx pulse bandwidth field.");
		fields[155] = ENVISAT_Field(36, 594, 597,	"ul",		"-",		 "num_err_echo_win_len","Number of errors detected in Echo Window Length field.");
		fields[156] = ENVISAT_Field(36, 598, 601,	"ul",		"-",		 "num_err_up","Number of errors detected in Upconverter Level field.");
		fields[157] = ENVISAT_Field(36, 602, 605,	"ul",		"-",		 "num_err_down","Number of errors detected in Downconverter Level field.");
		fields[158] = ENVISAT_Field(36, 606, 609,	"ul",		"-",		 "num_err_resamp","Number of errors detected in Resampling factor for echo data field.");
		fields[159] = ENVISAT_Field(36, 610, 613,	"ul",		"-",		 "num_err_beam_adj","Number of errors detected in Beam adjustment delta field.");
		fields[160] = ENVISAT_Field(36, 614, 617,	"ul",		"-",		 "num_err_beam_set_num","Number of errors detected in Antenna Beam Set Number field.");
		
		fields[161] = ENVISAT_Field(37, 618, 643,	"SpareField",	"-",	"spare_5", "");
		
		for(unsigned int s=0; s<5; ++s)
			fields[162+s] = ENVISAT_Field(38, 644+s*4, 647+s*4, "fl", "s", string("image_parameter_") + lexical_cast<string>(s+1) + "_swst_value",				"Sampling Window Start time of first processed line");
		for(unsigned int s=0; s<5; ++s)
			fields[167+s] = ENVISAT_Field(38, 664+s*4, 667+s*4, "fl", "s", string("image_parameter_") + lexical_cast<string>(s+1) + "_last_swst_value",		"Sampling Window Start time of last processed line");
		for(unsigned int s=0; s<5; ++s)
			fields[172+s] = ENVISAT_Field(38, 684+s*4, 687+s*4, "ul", "-", string("image_parameter_") + lexical_cast<string>(s+1) + "_swst_changes","Number of Sample Window Start Time changes within a beam");
		for(unsigned int s=0; s<5; ++s)	
			fields[177+s] = ENVISAT_Field(38, 704+s*4, 707+s*4, "fl", "Hz", string("image_parameter_") + lexical_cast<string>(s+1) + "_prf_value","Pulse Repetition Frequency");
		for(unsigned int s=0; s<5; ++s)
			fields[182+s] = ENVISAT_Field(38, 724+s*4, 727+s*4, "fl", "s", string("image_parameter_") + lexical_cast<string>(s+1) + "_x_pulse_len_value","Tx pulse lengt");
		for(unsigned int s=0; s<5; ++s)
			fields[187+s] = ENVISAT_Field(38, 744+s*4, 747+s*4, "fl", "Hz", string("image_parameter_") + lexical_cast<string>(s+1) + "_tx_pulse_bw_value","Tx pulse bandwidth");
		for(unsigned int s=0; s<5; ++s)
			fields[192+s] = ENVISAT_Field(38, 764+s*4, 767+s*4, "fl", "s", string("image_parameter_") + lexical_cast<string>(s+1) + "_echo_win_len_value","Echo Window Length");
		for(unsigned int s=0; s<5; ++s)
			fields[197+s] = ENVISAT_Field(38, 784+s*4, 787+s*4, "fl", "dB", string("image_parameter_") + lexical_cast<string>(s+1) + "_up_value","Upconverter Level - Upconverter gain set on the instrument");
		for(unsigned int s=0; s<5; ++s)
			fields[202+s] = ENVISAT_Field(38, 804+s*4, 807+s*4, "fl", "dB", string("image_parameter_") + lexical_cast<string>(s+1) + "_down_value","Downconverter Level - Downconverter gain set on the instrument");
		for(unsigned int s=0; s<5; ++s)
			fields[207+s] = ENVISAT_Field(38, 824+s*4, 827+s*4, "fl", ".", string("image_parameter_") + lexical_cast<string>(s+1) + "_resamp_value","Resampling factor");
		for(unsigned int s=0; s<5; ++s)
			fields[212+s] = ENVISAT_Field(38, 844+s*4, 847+s*4, "fl", "deg.",  string("image_parameter_") + lexical_cast<string>(s+1) +  "_beam_adj_value",			"Beam adjustment delta");
		for(unsigned int s=0; s<5; ++s)
			fields[217+s] = ENVISAT_Field(38, 864+s*2, 865+s*2, "us", "-", string("image_parameter_") + lexical_cast<string>(s+1) + "_beam_set_value",	"Antenna Beam Set Number");
		for(unsigned int s=0; s<5; ++s)
			fields[222+s] = ENVISAT_Field(38, 874+s*4, 877+s*4, "fl", "-", string("image_parameter_") + lexical_cast<string>(s+1) + "_tx_monitor_value",		"Auxiliary Tx Monitor Level");
		for(unsigned int s=0; s<5; ++s)
			fields[227+s] = ENVISAT_Field(38, 894+s*4, 897+s*4, "ul", "-", string("image_parameter_") + lexical_cast<string>(s+1) + "_rank","Number of PRI between transmitted pulse and return echo");
		
		fields[232] = ENVISAT_Field(39, 914, 975,	"SpareField",	"-",	"spare_6", "");
		
		fields[233] = ENVISAT_Field(40,  976,  979,		"ul",			"samples",	"first_proc_range_samp",	"First processed input range sample, first sample is 1");
		fields[234] = ENVISAT_Field(41,  980,  983,		"fl",			"m",		"range_ref",				"Range spreading loss reference range");
		fields[235] = ENVISAT_Field(42,  984,  987,		"fl",			"Hz",		"range_samp_rate",			"Range sampling rate");
		fields[236] = ENVISAT_Field(43,  988,  991,		"fl",			"Hz",		"radar_freq",				"Radar Frequency");
		fields[237] = ENVISAT_Field(44,  993,  993,		"us",			"looks",	"num_looks_range",			"Number of range looks");
		fields[238] = ENVISAT_Field(45,  994, 1000,		"AsciiString",	"ascii",	"filter_range",				"Matched filter window type: HAMMING or KAISER or NONE");
		fields[239] = ENVISAT_Field(46, 1001, 1004,		"fl",			"-",		"filter_coef_range",		"Window coefficient for range-matched filter");
		
		for(unsigned int s=0; s<5; ++s)
			fields[240+s] = ENVISAT_Field(47, 1005+s*4, 1008+s*4, "fl", "Hz", string("bandwidth_") + lexical_cast<string>(s+1) + "_look_bw_range","Range Look Bandwidth (null to null)");
		for(unsigned int s=0; s<5; ++s)
			fields[245+s] = ENVISAT_Field(47, 1025+s*4, 1028+s*4, "fl", "Hz", string("bandwidth_") + lexical_cast<string>(s+1) + "_tot_bw_range","Total processed range bandwidth (null to null)");
		
		for(unsigned int s=0; s<5; ++s){
			fields[250+s*8] = ENVISAT_Field(48, 1045+s*32, 1048+s*32, "fl", "-",		string("nominal_chirp_") + lexical_cast<string>(s+1) + "_amp_1",	"First nominal chirp amplitude coefficient");
			fields[251+s*8] = ENVISAT_Field(48, 1049+s*32, 1052+s*32, "fl", "s^-1",		string("nominal_chirp_") + lexical_cast<string>(s+1) + "_amp_2",	"Second nominal chirp amplitude coefficient");
			fields[252+s*8] = ENVISAT_Field(48, 1053+s*32, 1056+s*32, "fl", "s^-2",		string("nominal_chirp_") + lexical_cast<string>(s+1) + "_amp_3",	"Third nominal chirp amplitude coefficient");
			fields[253+s*8] = ENVISAT_Field(48, 1057+s*32, 1060+s*32, "fl", "s^-3",		string("nominal_chirp_") + lexical_cast<string>(s+1) + "_amp_4",	"Fourth nominal chirp amplitude coefficient");

			fields[254+s*8] = ENVISAT_Field(48, 1061+s*32, 1064+s*32, "fl", "cycles",	string("nominal_chirp_") + lexical_cast<string>(s+1) + "_phs_1",	"First nominal chirp phase coefficient");
			fields[255+s*8] = ENVISAT_Field(48, 1065+s*32, 1068+s*32, "fl", "Hz",		string("nominal_chirp_") + lexical_cast<string>(s+1) + "_phs_2",	"Second nominal chirp phase coefficient");
			fields[256+s*8] = ENVISAT_Field(48, 1069+s*32, 1072+s*32, "fl", "Hz/s",		string("nominal_chirp_") + lexical_cast<string>(s+1) + "_phs_3",	"Third nominal chirp phase coefficient");
			fields[257+s*8] = ENVISAT_Field(48, 1073+s*32, 1076+s*32, "fl", "Hz/(s^2)",	string("nominal_chirp_") + lexical_cast<string>(s+1) + "_phs_4",	"Fourth nominal chirp phase coefficient");
		}
		
		fields[290] = ENVISAT_Field(49, 1205, 1264,	"SpareField",	"-",		"spare_7", "");
		
		fields[291] = ENVISAT_Field(50, 1265, 1268,	"ul",			"lines",	"num_lines_proc",	"Number of input lines processed");
		fields[292] = ENVISAT_Field(51, 1269, 1270,	"us",			"looks",	"num_look_az",		"Number of Azimuth Looks");
		fields[293] = ENVISAT_Field(52, 1271, 1274,	"fl",			"Hz",		"look_bw_az",		"Azimuth Look Bandwidth (null to null) ");
		fields[294] = ENVISAT_Field(53, 1275, 1278,	"fl",			"Hz",		"to_bw_az",			"Processed Azimuth bandwidth (null to null)");
		fields[295] = ENVISAT_Field(54, 1279, 1285,	"AsciiString",	"ascii",	"filter_az",		"Matched filter window type: HAMMING or KAISER or NONE");
		fields[296] = ENVISAT_Field(55, 1286, 1289,	"fl",			"-",		"filter_coef_az",	"Window coefficient for azimuth-matched filter");
		
		fields[297] = ENVISAT_Field(56, 1290, 1293,	"fl",			"Hz/s",		"az_fm_rate_C0",	"First co-efficient for Azimuth FM rate: Azimuth FM rate = C0 + C1(tSR-t0) + C2(tSR - t0)2 tSR = 2 way slant range time");
		fields[298] = ENVISAT_Field(56, 1294, 1297,	"fl",			"Hz/(s^2)",	"az_fm_rate_C1",	"First co-efficient for Azimuth FM rate: Azimuth FM rate = C0 + C1(tSR-t0) + C2(tSR - t0)2 tSR = 2 way slant range time");
		fields[299] = ENVISAT_Field(56, 1298, 1301,	"fl",			"Hz/(s^3)",	"az_fm_rate_C2",	"First co-efficient for Azimuth FM rate: Azimuth FM rate = C0 + C1(tSR-t0) + C2(tSR - t0)2 tSR = 2 way slant range time");
		
		fields[300] = ENVISAT_Field(57, 1302, 1305,	"fl",			"ns",		"ax_fm_origin",		"2 way slant range time origin (t0) for Azimuth FM rate calculation");
		fields[301] = ENVISAT_Field(58, 1306, 1309,	"fl",			"-",		"dop_amb_conf",		"Doppler Centroid Ambiguity Confidence MeasureValue between 0 and 1, 0 = poorest confidence, 1= highest confidence");
		
		fields[302] = ENVISAT_Field(59, 1310, 1377,	"SpareField",	"-",		"spare_8", "");
		
		for(unsigned int s=0; s<2; ++s){
			fields[303+s*2] = ENVISAT_Field(60, 1378+s*8, 1381+s*8, "fl", "-",		string("proc_scaling_fact_") + lexical_cast<string>(s+1),	"Processor scaling factor");
			fields[304+s*2] = ENVISAT_Field(60, 1382+s*8, 1385+s*8, "fl", "-",		string("ext_cal_fact_") + lexical_cast<string>(s+1),		"External Calibration Scaling Factor (mode/swath/polarization dependent)");
		}	
		
		
		for(unsigned int s=0; s<5; ++s)
			fields[307+s] = ENVISAT_Field(61, 1394+s*4, 1397+s*4, "fl", "-", string("noise_power_corr_") + lexical_cast<string>(s+1) ,"Noise power correction factors");
		for(unsigned int s=0; s<5; ++s)
			fields[312+s] = ENVISAT_Field(61, 1414+s*4, 1417+s*4, "ul", "-", string("num_noise_lines_") + lexical_cast<string>(s+1) ,"Number of noise lines used to calculate factors");
		
		fields[317] = ENVISAT_Field(62, 1434, 1497,	"SpareField",	"-",		"spare_9", "");
		fields[318] = ENVISAT_Field(63, 1498, 1509,	"SpareField",	"-",		"spare_10", "");
		
		
		for(unsigned int s=0; s<2; ++s){
			fields[319+s*4] = ENVISAT_Field(64, 1510+s*16, 1513+s*16, "fl", "-",		string("out_mean_") + lexical_cast<string>(s+1),			"Output data mean Magnitude for detected products, real sample mean for SLC products");
			fields[320+s*4] = ENVISAT_Field(64, 1514+s*16, 1517+s*16, "fl", "-",		string("out_imag_mean_") + lexical_cast<string>(s+1),		"Output imaginary data mean Used for SLC products only (set to zero otherwise)");
			fields[321+s*4] = ENVISAT_Field(64, 1518+s*16, 1521+s*16, "fl", "-",		string("out_std_dev_") + lexical_cast<string>(s+1),			"Output data standard deviation Magnitude std. dev. for detected products, real sample std. dev. for SLC products");
			fields[322+s*4] = ENVISAT_Field(64, 1522+s*16, 1525+s*16, "fl", "-",		string("out_imag_std_dev_") + lexical_cast<string>(s+1),	"Output imaginary data standard deviation Used for SLC products only (set to zero otherwise)");
		}	
		
		fields[327] = ENVISAT_Field(65, 1542, 1545,	"fl",			"m",		"avg_scene_height_ellpsoid",	"avg_scene_height_ellpsoid");
		
		fields[328] = ENVISAT_Field(66, 1546, 1593,	"SpareField",	"-",		"spare_11",						"");
		
		fields[329] = ENVISAT_Field(67, 1594, 1597,	"AsciiString",	"ascii",	"echo_comp",				"Compression Method used for echo samples: FBAQ, S&M, NONE");
		fields[330] = ENVISAT_Field(68, 1598, 1600,	"AsciiString",	"ascii",	"echo_comp_ratio",			"Compression Ratio for echo samples8/4, 8/3, 8/2, or 8/8");
		fields[331] = ENVISAT_Field(69, 1601, 1604,	"AsciiString",	"ascii",	"init_cal_comp  ",			"Compression Method used for initial calibration samples: FBAQ, S&M, NONE");
		fields[332] = ENVISAT_Field(70, 1605, 1607,	"AsciiString",	"ascii",	"init_cal_ratio",			"Compression Ratio for initial calibration samples8/4, 8/3, 8/2, or 8/8");
		fields[333] = ENVISAT_Field(71, 1608, 1611,	"AsciiString",	"ascii",	"per_cal_comp",				"Compression Method used for periodic calibration samples: FBAQ, S&M, NONE");
		fields[334] = ENVISAT_Field(72, 1612, 1614,	"AsciiString",	"ascii",	"per_cal_ratio",			"Compression Ratio for periodic calibration samples8/4, 8/3, 8/2, or 8/8");
		fields[335] = ENVISAT_Field(73, 1615, 1618,	"AsciiString",	"ascii",	"noise_comp",				"Compression Method used for noise samples: FBAQ, S&M, NONE");
		fields[336] = ENVISAT_Field(74, 1619, 1621,	"AsciiString",	"ascii",	"noise_comp_ratio",			"Compression Ratio for noise samples8/4, 8/3, 8/2, or 8/8");
		
		fields[337] = ENVISAT_Field(75, 1622, 1685,	"SpareField",	"-",	"spare_12", "");
		
		for(unsigned int s=0; s<4; ++s)		
			fields[338+s] = ENVISAT_Field(76, 1686+s*4, 1689+s*4,	"ul",	"-",	string("beam_overlap_") + lexical_cast<string>(s+1),	"Number of slant range samples in beam merging: One value per merge region (1-2, 2-3, 3-4, 4-5). This parameter is equivalent to N in the following beam merging formula: xmerged(n) = (1 - (n/N)P * xnear(n) + ((n/N)P * xfar(n)");
		
		for(unsigned int s=0; s<4; ++s)		
			fields[342+s] = ENVISAT_Field(77, 1702+s*4, 1705+s*4,	"fl",	"-",	string("beam_param_") + lexical_cast<string>(s+1),	"Beam merge algorithm parameter used for beam merging: One value per merge region (1-2, 2-3, 3-4, 4-5). This parameter is equivalent to P in the above beam merging forumla, and different values have the following affect: P = 1, linear weighting of the two beams (near and far). P = -1, (which represents infinity in the beam merging formula) only near beam contributes to the merged one P = 0, only far beam contributes to the merged one P > 1, near beam is favoured 0 < P < 1, far beam is favoured.");
		
		for(unsigned int s=0; s<5; ++s)		
			fields[346+s] = ENVISAT_Field(78, 1718+s*4, 1721+s*4,	"ul",	"-",	string("lines_per_burst_") + lexical_cast<string>(s+1),	"Number of lines per burst for this image. 5 values for beams SS1 to SS5 in WS and GM modes. Two values for AP mode, all others set to zero.");
		
		
		fields[351] = ENVISAT_Field(79, 1738, 1749,	"MJD",	"MJD",	"time_first_SS1_echo", "Time of first SS1 Echo Source Packet");
		
		fields[352] = ENVISAT_Field(80, 1750, 1765,	"SpareField",	"-",	"spare_13", "");
		
		for(unsigned int s=0; s<5; ++s){
			fields[353+s*7] = ENVISAT_Field(81, 1766+s*36, 1777+s*36,	"MJD",	"MJD",			string("state_vect_time_") + lexical_cast<string>(s+1),		"Time of state vector");
			fields[354+s*7] = ENVISAT_Field(81, 1778+s*36, 1781+s*36,	"sl",	"10e-2 m",		string("x_pos_") + lexical_cast<string>(s+1),				"X position in Earth fixed reference frame");
			fields[355+s*7] = ENVISAT_Field(81, 1782+s*36, 1785+s*36,	"sl",	"10e-2 m",		string("y_pos_") + lexical_cast<string>(s+1),				"Y position in Earth fixed reference frame");
			fields[356+s*7] = ENVISAT_Field(81, 1786+s*36, 1789+s*36,	"sl",	"10e-2 m",		string("z_pos_") + lexical_cast<string>(s+1),				"Z position in Earth fixed reference frame");
			fields[357+s*7] = ENVISAT_Field(81, 1790+s*36, 1793+s*36,	"sl",	"10e-5 m",		string("x_vel_") + lexical_cast<string>(s+1),				"X velocity relative to Earth fixed reference frame");
			fields[358+s*7] = ENVISAT_Field(81, 1794+s*36, 1797+s*36,	"sl",	"10e-5 m",		string("y_vel_") + lexical_cast<string>(s+1),				"Y velocity relative to Earth fixed reference frame");
			fields[359+s*7] = ENVISAT_Field(81, 1798+s*36, 1801+s*36,	"sl",	"10e-5 m",		string("z_vel_") + lexical_cast<string>(s+1),				"Z velocity relative to Earth fixed reference frame");
		}
		
		fields[388] = ENVISAT_Field(80, 1946, 2009,	"SpareField",	"-",	"spare_13", "");
		
		int i=0;
		while(		i < num_fields ){
			values[i] = getENVISATValue(data, fields[i]);
			++i;
		}
	}
};

#endif