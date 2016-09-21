/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef FACILITY_RELATED_DATA_RECORD_HXX
#define FACILITY_RELATED_DATA_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class FacilityRelatedDataRecordGeneralType : public ERS_Record{
	
public:
	FacilityRelatedDataRecordGeneralType(char* data){
		char revision ='U';
		//get ESA-Header:			
		if (		((unsigned char*)data)[4]==18 
				&&	((unsigned char*)data)[5]==200
				&&	((unsigned char*)data)[6]==18
				&&	((unsigned char*)data)[7]==50	){
			revision ='A';
			//cout << "FacilityRelatedDataRecordGeneralType::FacilityRelatedDataRecordGeneralType: Alles gut, Header scheint zu passen (Revision A)!\n";
		}
		else if (		((unsigned char*)data)[4]==10 
					&&	((unsigned char*)data)[5]==200
					&&	((unsigned char*)data)[6]==31
					&&	((unsigned char*)data)[7]==50	){
			revision ='B';
			//cout << "FacilityRelatedDataRecordGeneralType::FacilityRelatedDataRecordGeneralType: Alles gut, Header scheint zu passen (Revision B)!\n";
		}
		else{
			cout << "FacilityRelatedDataRecordGeneralType::FacilityRelatedDataRecordGeneralType: Fehler, Header scheint nicht zu passen!\n";
		}
		unsigned int num_fields=(revision == 'A') ? 69 : 163;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[  0] = ERS_Field(  1,   1,     4,	"B4",		"-",		"Record sequence number");
		fields[  1] = ERS_Field(  2,   5,     5,	"B1",		"-",		"1-st record subtype code");
		fields[  2] = ERS_Field(  3,   6,     6,	"B1",		"-",		"Record type code"  );
		fields[  3] = ERS_Field(  4,   7,     7,	"B1",		"-",		"2-nd subtype code"  );
		fields[  4] = ERS_Field(  5,   8,     8,	"B1",		"-",		"3-rd subtype code"  );
		fields[  5] = ERS_Field(  6,   9,    12,	"B4",		"bytes",	"Length of this record" );
		fields[  6] = ERS_Field(  7,   13,   76,	"A64", 		"-",		"Name of this facility related data record   $RELATED$DATA$RECORD$GENERAL$TYPE$..$" );
		fields[  7] = ERS_Field(  8,   77,   82,    "A6", 		"-",		"Date of the last release of QC software as YYMMDD" );
		fields[  8] = ERS_Field(  9,   83,   84,    "A2", 		"-",		"spare"   );
		fields[  9] = ERS_Field( 10,   85,   90,    "A6", 		"-",		"Date of the last calibration update as YYMMDD" );
		//SAR QA Summary Flags: 
		fields[ 10] = ERS_Field( 11,   91,   94,    "I4", 		"-",		"Overall QA Summary Flag (0/1)" );
		fields[ 11] = ERS_Field( 12,   95,   98,    "I4", 		"-",		"PRF code change Flag (0/1)" );
		fields[ 12] = ERS_Field( 13,   99,  102,    "I4", 		"-",		"Sampling Window change Flag (0/1)" );
		fields[ 13] = ERS_Field( 14,  103,  106,    "I4", 		"-",		"Cal system & Receiver gain change Flag (0/1)");
		fields[ 14] = ERS_Field( 15,  107,  110,    "I4", 		"-",		"Chirp replica quality Flag (0/1)");
		fields[ 15] = ERS_Field( 16,  111,  114,    "I4", 		"-",		"Input data statistic Flag (0/1)");
		fields[ 16] = ERS_Field( 17,  115,  118,    "I4", 		"-",		"Doppler centroid confidence measure Flag (0/1)");
		fields[ 17] = ERS_Field( 18,  119,  122,    "I4", 		"-",		"Doppler centroid value Flag ( 0/1)");
		fields[ 18] = ERS_Field( 19,  123,  126,    "I4", 		"-",		"Doppler ambiguity confidence measure Flag (0/1)" );
		fields[ 19] = ERS_Field( 20,  127,  130,    "I4", 		"-",		"Output data Mean Flag");
		//SAR QA Parameters:
		fields[ 20] = ERS_Field( 21,  131,  134,    "I4", 		"-",		"OGRC/OBRC flag (0/1)"  );
		fields[ 21] = ERS_Field( 22,  135,  138,    "I4", 		"-",		"Number of PRF code changes"  );
		fields[ 22] = ERS_Field( 23,  139,  142,    "I4", 		"-",		"Number of sampling window time changes"  );
		fields[ 23] = ERS_Field( 24,  143,  146,    "I4", 		"-",		"Number of calibration subsystems gain change" );
		fields[ 24] = ERS_Field( 25,  147,  150,    "I4", 		"lines",	"Number of missing lines"  );
		fields[ 25] = ERS_Field( 26,  151,  154,    "I4", 		"-",		"Number of receiver gain changes");
		fields[ 26] = ERS_Field( 27,  155,  170,	"F16.7",	"-",		"3-dB pulse width of ( first ) Chirp" );
		//Replica Autocorrelation Function (ACF)
		fields[ 27] = ERS_Field( 28,  171,  186,	"F16.7", 	"-",		"first side, lobe level of Chirp ACF" );
		fields[ 28] = ERS_Field( 29,  187,  202,	"F16.7",	"-",		"ISLR of Chirp ACF function" );
		fields[ 29] = ERS_Field( 30,  203,  218,	"F16.7",	"-",		"Doppler Centroid confidence measure (processor  specific) (value normalised such that it takes a  value between zero and one as worst and best case)");
		fields[ 30] = ERS_Field( 31,  219,  234,	"F16.7", 	"-",		"Doppler ambiguity confidence measure  (processor specific)"  );
		fields[ 31] = ERS_Field( 32,  235,  250,	"F16.7", 	"-",		"Estimated Mean of I input data" );
		fields[ 32] = ERS_Field( 33,  251,  266,	"F16.7", 	"-",		"Estimated Mean of Q input data");
		fields[ 33] = ERS_Field( 34,  267,  282,	"F16.7", 	"-",		"Estimated Standard deviation of I input data" );
		fields[ 34] = ERS_Field( 35,  283,  298,	"F16.7", 	"-",		"Estimated Standard deviation of Q input data" );
		fields[ 35] = ERS_Field( 36,  299,  314,	"F16.7", 	"-",		"Calibration system gain" );
		fields[ 36] = ERS_Field( 37,  215,  330,	"F16.7", 	"-",		"First Receiver Gain Read");
		fields[ 37] = ERS_Field( 38,  231,  346,	"F16.7", 	"-",		"Doppler Ambiguity Number");
		fields[ 38] = ERS_Field( 39,  347,  362,    "A16",		"-",		"Spare");
		
		//Internal Calibration
		int revision_shift = (revision=='A')? 2 : 0;
		fields[ 39] = ERS_Field( 40,  363+revision_shift,  378+revision_shift,	"F16.7", 	"-",		"bias correction applied to I channel" );
		fields[ 40] = ERS_Field( 41,  379+revision_shift,  394+revision_shift,	"F16.7", 	"-",		"bias correction applied to Q channel");
		fields[ 41] = ERS_Field( 42,  395+revision_shift,  410+revision_shift,	"F16.7", 	"-",		"Gain imbalance correction applied to I channel" );
		fields[ 42] = ERS_Field( 43,  411+revision_shift,  426+revision_shift,	"F16.7", 	"-",		"Gain imbalance correction applied to Q channel" );
		fields[ 43] = ERS_Field( 44,  427+revision_shift,  442+revision_shift,	"F16.7", 	"-",		"I/Q Non, orthogonality correction applied to Q channel" );
		fields[ 44] = ERS_Field( 45,  443+revision_shift,  458+revision_shift,	"A16",		"-",		"Spare" );
		fields[ 45] = ERS_Field( 46,  459+revision_shift,  474+revision_shift,	"F16.7", 	"-",		"Estimated noise power" );
		fields[ 46] = ERS_Field( 47,  475+revision_shift,  490+revision_shift,	"I16",		"nanosec",		"calibration pulse time delay" );
		fields[ 47] = ERS_Field( 48,  491+revision_shift,  494+revision_shift,    "I4",		"pulses",	"Number of valid calibration pulses" );
		fields[ 48] = ERS_Field( 49,  495+revision_shift,  498+revision_shift,    "I4",		"pulses",		"Number of valid noise pulses" );
		fields[ 49] = ERS_Field( 50,  499+revision_shift,  502+revision_shift,    "I4",		"pulses",		"Number of valid replica pulses" );
		fields[ 50] = ERS_Field( 51,  503+revision_shift,  518+revision_shift, "F16.7", 		"samples",		"first sample in replica (CEI)" );
		fields[ 51] = ERS_Field( 52,  519+revision_shift,  534+revision_shift, "F16.7", 		"-",		"Mean Cal pulse power" );
		fields[ 52] = ERS_Field( 53,  535+revision_shift,  550+revision_shift, "F16.7", 		"-",		"Mean noise power" );
		fields[ 53] = ERS_Field( 54,  551+revision_shift,  566+revision_shift, "F16.7", 		"-",		"range compression normalisation factor");
		fields[ 54] = ERS_Field( 55,  567+revision_shift,  582+revision_shift, "F16.7", 		"-",		"replica power" );
		fields[ 55] = ERS_Field( 56,  583+revision_shift,  598+revision_shift, "F16.7", 		"degrees",	"incidence angle at first range pixel  (at mid-azimuth)" );
		fields[ 56] = ERS_Field( 57,  599+revision_shift,  614+revision_shift, "F16.7", 		"degrees",	"incidence angle at centre range pixel (at mid-azimuth)" );
		fields[ 57] = ERS_Field( 58,  615+revision_shift,  630+revision_shift, "F16.7", 		"degrees",	"incidence angle at last range pixel   (at mid-azimuth)" );
		fields[ 58] = ERS_Field( 59,  631+revision_shift,  646+revision_shift, "F16.7", 		"km",		"normalisation reference range Ro ( set to 0 for ERS.SAR.SLC )");
		
		if(revision == 'A'){
			fields[ 59] = ERS_Field( 60,  647,  648,   "A2", 		"-",		"Spare" );
			fields[ 60] = ERS_Field( 61,  649,  652,    "I4", 		"-",		"antenna elevation gain pattern flag (0/1)" );
			fields[ 62] = ERS_Field( 63,  653,  668, "F16.7", 		"-",		"Upper bound K (+3 std dev )" );
			fields[ 63] = ERS_Field( 64,  669,  684, "F16.7", 		"-",		"Lower bound K (-3 std dev )" );
			fields[ 64] = ERS_Field( 65,  685,  700, "F16.7", 		"dB",		"estimated, noise equivalent sigma deg." );
			fields[ 65] = ERS_Field( 66,  701,  706,    "A6", 		"-",		"Date on which K was generated as YYMMDD" );
			fields[ 66] = ERS_Field( 67,  707,  710,    "A4", 		"-",		"K version number as XXYY where:  XX refers to a K update implemented across the  ground segment, YY refers to an upgrade only at the source PAF (as may arise in case of local software updates)" );
			fields[ 67] = ERS_Field( 68,  711,  714,     "I4", 		"lines",	"number of duplicated input lines" );
			fields[ 68] = ERS_Field( 69,  715,  730,  "F16.7", 		"-",		"estimated bit error rate" );
		}
		else{
			fields[ 59] = ERS_Field( 60,  647,  658,   "A12", 		"-",		"Spare" );
			fields[ 60] = ERS_Field( 61,  659,  662,    "I4", 		"-",		"antenna elevation gain pattern flag (0/1)" );
			fields[ 61] = ERS_Field( 62,  663,  678, "F16.7", 		"-",		"absolute calibration constant K (scalar)" );
			fields[ 62] = ERS_Field( 63,  679,  694, "F16.7", 		"-",		"Upper bound K (+3 std dev )" );
			fields[ 63] = ERS_Field( 64,  695,  710, "F16.7", 		"-",		"Lower bound K (-3 std dev )" );
			fields[ 64] = ERS_Field( 65,  711,  726, "F16.7", 		"dB",		"estimated, noise equivalent sigma deg." );
			fields[ 65] = ERS_Field( 66,  727,  732,    "A6", 		"-",		"Date on which K was generated as YYMMDD" );
			fields[ 66] = ERS_Field( 67,  733,  736,    "A4", 		"-",		"K version number as XXYY where:  XX refers to a K update implemented across the  ground segment, YY refers to an upgrade only at the source PAF (as may arise in case of local software updates)" );
			fields[ 67] = ERS_Field( 68,  737,  740,     "I4", 		"lines",	"number of duplicated input lines" );
			fields[ 68] = ERS_Field( 69,  741,  750,  "F16.7", 		"-",		"estimated bit error rate" );
			fields[ 69] = ERS_Field( 70,  637,  768,   "A132", 		"-",		"Spare" );
			fields[ 70] = ERS_Field( 71,  769,  784,  "F16.7", 		"-",		"output image mean" );
			fields[ 71] = ERS_Field( 72,  785,  800,  "F16.7", 		"-",		"output image standard deviation");
			fields[ 72] = ERS_Field( 73,  801,  816,  "F16.7", 		"-",		"output image maximum value" );
			fields[ 73] = ERS_Field( 74,  817,  840,    "A24", 		"UTC",		"time of raw data first input range line <dd, MMM, yyy hh:mm:ss.ttt>");
			fields[ 74] = ERS_Field( 75,  841,  864,    "A24", 		"UTC",		"time of aseending node state vectors <dd, MMM, yyy hh:mm:ss.ttt>");
			fields[ 75] = ERS_Field( 76,  865,  886, "D22.15",		"m",		"ascending node position vector X component");
			fields[ 76] = ERS_Field( 77,  887,  908, "D22.15", 		"m",		"ascending node position vector Y component");
			fields[ 77] = ERS_Field( 78,  909,  930, "D22.15", 		"m",		"ascending node position vector Z component" );
			fields[ 78] = ERS_Field( 79,  931,  952, "D22.15", 		"m/s",		"ascending node velocity vector Vx component");
			fields[ 79] = ERS_Field( 80,  953,  974, "D22.15", 		"m/s",		"ascending node velocity vector Vy component" );
			fields[ 80] = ERS_Field( 81,  975,  996, "D22.15",  	"m/s",		"ascending node velocity vector Vz component" );
			fields[ 81] = ERS_Field( 82,  997, 1000,     "I4", 		"bits",		"output pixel bit length" );
			fields[ 82] = ERS_Field( 83, 1001, 1016,  "F16.7", 		"-",		"processor gain parameter 1" );
			fields[ 83] = ERS_Field( 84, 1017, 1032,  "F16.7", 		"-",		"processor gain parameter 2" );
			fields[ 84] = ERS_Field( 85, 1033, 1048,  "F16.7", 		"-",		"processor gain parameter 3" );
			fields[ 85] = ERS_Field( 86, 1049, 1052,     "I4", 		"samples",	"peak location of cross correlation function between first extracted chirp and nominal chirp");
			fields[ 86] = ERS_Field( 87, 1053, 1068,  "F16.7", 		"samples",	"3-dB width of CCF between last extracted chirp and  nominal chirp" );
			fields[ 87] = ERS_Field( 88, 1069, 1084,  "F16.7", 		"dB",		"first side lobe level of chirp (CCF) between last  extracted chirp and nominal chirp" );
			fields[ 88] = ERS_Field( 89, 1085, 1100,  "F16.7", 		"dB",		"ISLR of CCF between last exeracted chirp and nominal chirp" );
			fields[ 89] = ERS_Field( 90, 1101, 1104,     "I4", 		"samples",	"peak location of CCF betwee last extracted chirp and nominal chirp");
			fields[ 90] = ERS_Field( 91, 1105, 1108,     "I4", 		"-",		"Roll Tilt Mode flag (0 = not in roll tilt mode)");
			fields[ 91] = ERS_Field( 92, 1109, 1112,     "I4", 		"-",		"raw data correction flag (0 = correction with default  parameters)" );
			fields[ 92] = ERS_Field( 93, 1113, 1116,     "I4", 		"-",		"look detection flag tl = power detected and summed)" );
			fields[ 93] = ERS_Field( 94, 1117, 1120,     "I4", 		"-",		"doppler ambiguity estimation flag (0 = no estimation done)");
			fields[ 94] = ERS_Field( 95, 1121, 1124,     "I4", 		"-",		"azimuth baseband conversion flag (0 = no conversion done)" );
			fields[ 95] = ERS_Field( 96, 1125, 1128,    "I4", 		"samples",	"samples per line used for the raw data analysis" );
			fields[ 96] = ERS_Field( 97, 1129, 1132,     "I4", 		"lines",	"range lines skip factor for raw data analysis");
			fields[ 97] = ERS_Field( 98, 1133, 1156,    "A24", 		"UTC",		"time of input state vectors used to process the image <dd, MMM, yyy hh:mm:ss.ttt>" );
			fields[ 98] = ERS_Field( 99, 1157, 1178, "D22.15", 		"m",		"input state vector position X component" );
			fields[ 99] = ERS_Field(100, 1179, 1200, "D22.15", 		"m",		"input state vector position Y component" );
			fields[100] = ERS_Field(101, 1201, 1222, "D22.15", 		"m",		"input state vector position Z component" );
			fields[101] = ERS_Field(102, 1223, 1244, "D22.15", 		"m/s",		"input state vector velocity Vx component" );
			fields[102] = ERS_Field(103, 1245, 1266, "D22.15", 		"m/s",		"input state vector velocity Vy component");
			fields[103] = ERS_Field(104, 1267, 1288, "D22.15", 		"m/s",		"input state vector velocity Vz compcnent");
			fields[104] = ERS_Field(105, 1289, 1292,     "I4", 		"-",		"input state vector type flag:  (0 = ascending node state vectors - predicted orbit, 1 = near scene state vectors -restituted orbit)" );
			fields[105] = ERS_Field(106, 1293, 1308,  "F16.7", 		"-",		"window coefficient for range-matched filter");
			fields[106] = ERS_Field(107, 1309, 1324,  "F16.7",  	"-",		"window coefficient for azimuth-matched filter");
			fields[107] = ERS_Field(108, 1325, 1328,     "I4", 		"chirps",	"update period of range-matched filter");
			fields[108] = ERS_Field(109, 1329, 1344,  "F16.7", 		"-",		"look scalar gains (up to 8 looks)"  ); //109-1
			fields[109] = ERS_Field(109, 1345, 1360,  "F16.7", 		"-",		"look scalar gains (up to 8 looks)"  ); //109-2
			fields[110] = ERS_Field(109, 1361, 1376,  "F16.7", 		"-",		"look scalar gains (up to 8 looks)"  ); //109-3
			fields[111] = ERS_Field(109, 1377, 1392,  "F16.7", 		"-",		"look scalar gains (up to 8 looks)"  ); //109-4
			fields[112] = ERS_Field(109, 1393, 1408,  "F16.7", 		"-",		"look scalar gains (up to 8 looks)"  ); //109-5
			fields[113] = ERS_Field(109, 1409, 1414,  "F16.7", 		"-",		"look scalar gains (up to 8 looks)"  ); //109-6
			fields[114] = ERS_Field(109, 1415, 1430,  "F16.7", 		"-",		"look scalar gains (up to 8 looks)"  ); //109-7
			fields[115] = ERS_Field(109, 1431, 1456,  "F16.7", 		"-",		"look scalar gains (up to 8 looks)"  ); //109-8
			fields[116] = ERS_Field(110, 1457, 1460,     "I4", 		"nanosec",	"sampling window start time bias"  );
			fields[117] = ERS_Field(111, 1461, 1482, "D22.15", 		"Hz/sec^3",	"doppler centroid cubic coefficient"  );
			fields[118] = ERS_Field(112, 1483, 1486,     "I4", 		"-",		"PRF code of first range line (telemetry value)" );
			fields[119] = ERS_Field(113, 1487, 1490,     "I4", 		"-",		"PRF code of last range line (telemetry value)");
			fields[120] = ERS_Field(114, 1491, 1494,     "I4", 		"-",		"sampling window start time code of first range line (telemetry value)" );
			fields[121] = ERS_Field(115, 1495, 1498,     "I4", 		"-",		"sampling window start time code of last range line (telemetry value)" );
			fields[122] = ERS_Field(116, 1499, 1502,     "I4", 		"-",		"calibration system gain of last processed line (telemetry value)" );
			fields[123] = ERS_Field(117, 1503, 1506,     "I4", 		"-",		"receiver gain of last processed line (telemetry value)");
			fields[124] = ERS_Field(118, 1507, 1510,     "I4", 		"-",		"first processed range sample" );
			fields[125] = ERS_Field(119, 1511, 1514,     "I4", 		"-",		"azimuth FFT/IFFT ratio" );
			fields[126] = ERS_Field(120, 1515, 1518,     "I4", 		"blocks",	"number of azimuth blocks processed" );
			fields[127] = ERS_Field(121, 1519, 1526,     "I8", 		"lines",	"number of input raw data lines" );
			fields[128] = ERS_Field(122, 1527, 1530,     "I4", 		"-",		"initial doppler ambiguity number" );
			fields[129] = ERS_Field(123, 1531, 1546,  "F16.7", 		"pixels",		"chirp quality thresholds: Pulse width of the chirp CCF (10-3)");
			fields[130] = ERS_Field(123, 1547, 1562,  "F16.7", 		"dB",		"chirp quality thresholds: First sidelobe of the chirp CCF (10-3)");
			fields[131] = ERS_Field(123, 1563, 1578,  "F16.7", 		"dB",		"chirp quality thresholds: ISLR of the chirp CCF (10-3)");
			fields[132] = ERS_Field(123, 1579, 1594,  "F16.7", 		"-",		"input data statistic thresholds: mean of input I data in fraction of maximum absolute");
			fields[133] = ERS_Field(123, 1595, 1610,  "F16.7", 		"-",		"input data statistic thresholds: mean of input Q data in fraction of maximum absolute");
			fields[134] = ERS_Field(123, 1611, 1626,  "F16.7", 		"-",		"input data statistic thresholds: standard deviation of input I data in fraction of maximum absolute of input data (10-3)");
			fields[135] = ERS_Field(123, 1527, 1642,  "F16.7", 		"-",		"input data statistic thresholds: standard deviation of input Q data in fraction ofmaximum absolute of input data (10-3)");
			fields[136] = ERS_Field(123, 1643, 1658,  "F16.7",		"-",		"doppler ambiguity confidence lower threshold (10-3)"  );
			fields[137] = ERS_Field(123, 1659, 1674,  "F16.7",		"-",		"doppler ambiguity confidence upper threshold (10-3)"  );
			fields[134] = ERS_Field(123, 1675, 1690,  "F16.7",		"-",		"output data statistic thresholds mean of outout data (10-3)" );
			fields[135] = ERS_Field(123, 1691, 1706,  "F16.7", 		"-",		"output data statistic thresholds tandard deviation of output data (10-3)"  );
			fields[136] = ERS_Field(124, 1707, 1722,    "I16", 		"-",		"satellite binary time of first range line (telemetry value)" );
			fields[137] = ERS_Field(125, 1723, 1726,     "I4", 		"pixels",	"number of valid pixels per range line (the remaining pixels are zero padded)"  );
			fields[138] = ERS_Field(126, 1727, 1730,     "I4", 		"samples",	"number of range samples discarded during" );
			fields[139] = ERS_Field(127, 1731, 1746,  "F16.7", 		"-",		"I/Q gain imbalance lower bound" );
			fields[140] = ERS_Field(128, 1747, 1762,  "F16.7", 		"-",		"I/Q gain imbalance upper bound" );
			fields[141] = ERS_Field(129, 1763, 1778,  "F16.7", 		"degrees",	"I/Q quadrature departure lower bound" );
			fields[142] = ERS_Field(130, 1779, 1794,  "F16.7", 		"degrees",	"I/Q quadrature departure upper bound" );
			fields[143] = ERS_Field(131, 1795, 1810,  "F16.7", 		"Hz",		"3-dB look bandwidth" );
			fields[144] = ERS_Field(132, 1811, 1826,  "F16.7", 		"Hz",		"3-dB processed doppler bandwidth" );
			fields[145] = ERS_Field(133, 1827, 1830,     "I4", 		"-",		"range spreading loss compensation flag (O == no compensation)" );
			fields[146] = ERS_Field(134, 1831, 1831,     "I1", 		"-",		"datation flag (1 . azimuth timing improved based on timing information of range line specified in field 136)" );
			fields[147] = ERS_Field(135, 1832, 1838,     "I7", 		"nanosec",	"maximum error of range line timing");
			fields[148] = ERS_Field(136, 1839, 1845,     "I7", 		"-",		"format number of range line used to synchronize azimuth timing" );
			fields[149] = ERS_Field(137, 1846, 1846,     "I1", 		"-",		"automatic look scal gain flag  (1 = automatically calculated)");
			fields[150] = ERS_Field(138, 1847, 1850,     "I4", 		"-",		"maximum value of look scalar gain before the look scalar gains are normalised" );
			fields[151] = ERS_Field(139, 1851, 1854,     "I4", 		"-",		"replica normalisation method flag  (0 = normalized by replica power,i.e. Z' = Z*(c/Ar) where Ar is the replica power and c is specified in field 54, t1 = normalised by the square root of replica power, Z' = Z /Ar)" );
			fields[152] = ERS_Field(140, 1855, 1974, "E20.10", 		"-",		"1st coefficient of the ground range to slant range conversion polynamial");
			fields[153] = ERS_Field(140, 1875, 1894, "E20.10", 		"-",		"2nd coefficient of the ground range to slant range conversion polynamial");
			fields[154] = ERS_Field(140, 1895, 1914, "E20.10", 		"-",		"3rd coefficient of the ground range to slant range conversion polynamial");
			fields[155] = ERS_Field(140, 1915, 1934, "E20.10", 		"-",		"4th coefficient of the ground range to slant range conversion polynamial");
			fields[156] = ERS_Field(141, 1935, 1954, "E20.10", 		"-",		"1st coefficient of the antenna elevation attern polyhomial");
			fields[157] = ERS_Field(141, 1955, 1974, "E20.10", 		"-",		"2nd coefficient of the antenna elevation attern polyhomial");
			fields[158] = ERS_Field(141, 1975, 1994, "E20.10", 		"-",		"3rd coefficient of the antenna elevation attern polyhomial");
			fields[159] = ERS_Field(141, 1995, 2014, "E20.10", 		"-",		"4th coefficient of the antenna elevation attern polyhomial");
			fields[160] = ERS_Field(141, 2015, 2034, "E20.10", 		"-",		"5th coefficient of the antenna elevation attern polyhomial");
			fields[161] = ERS_Field(142, 2035, 2050,  "E16.7", 		"sec",		"range time of origin of antenna elevation pattern polynomial" );
			fields[162] = ERS_Field(143, 2051, 12288,"A10238", 		"-",		"Spare");	
		}
		
		length = getRecordLength(data);
		int i=0;
		while(		i < num_fields
			  &&	 fields[i].end <= length){
			values[i] = getERSValue(data, fields[i]);
			++i;
		}
	}
};

class FacilityRelatedDataRecordPCSQualityDefinition : public  ERS_Record{
	
public:
	FacilityRelatedDataRecordPCSQualityDefinition(char* data){
		//get ESA-Header:			
		if (		((unsigned char*)data)[4]==18 
			&&	((unsigned char*)data)[5]==200
			&&	((unsigned char*)data)[6]==18
			&&	((unsigned char*)data)[7]==50	){
			
			//cout << "FacilityRelatedDataRecordPCSQualityDefinition::FacilityRelatedDataRecordPCSQualityDefinition: Alles gut, Header scheint zu passen (Revision A)!\n";
		}
		else{
			cout << "FacilityRelatedDataRecordPCSQualityDefinition::FacilityRelatedDataRecordPCSQualityDefinition: Fehler, Header scheint nicht zu passen!\n";
		}
		
		unsigned int num_fields=8;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[  0] = ERS_Field(  1,    1,    4,     "B4",		"-",	"Record sequence number");
		fields[  1] = ERS_Field(  2,    5,    5,     "Bl",		"-",	"1-st record subtype code");
		fields[  2] = ERS_Field(  3,    6,    6,     "Bl",		"-",	"Record type code"  );
		fields[  3] = ERS_Field(  4,    7,    7,     "Bl",		"-",	"2-nd subtype code"  );
		fields[  4] = ERS_Field(  5,    8,    8,     "Bl",		"-",	"3-rd subtype code"  );
		fields[  5] = ERS_Field(  6,    9,   12,     "B4",		"-",	"Length of this record" );
		fields[  6] = ERS_Field(  7,   13,   76,    "A64",		"-",	"Name of this facility related data record   $RELATED$DATA$RECORD$GENERAL$TYPE$..$" );
		fields[  7] = ERS_Field(  8,   77,   12288, "A12212",	"-",	"reserved" ); 
		
		length = getRecordLength(data);
		int i=0;
		while(		i < num_fields
			  &&	 fields[i].end <= length){
			values[i] = getERSValue(data, fields[i]);
			++i;
		}
	}
};

class FacilityRelatedDataRecord11 : public  ERS_Record{
	
public:
	FacilityRelatedDataRecord11(char* data){
		//get ESA-Header:			
		if (		((unsigned char*)data)[4]==18 
			&&	((unsigned char*)data)[5]==200
			&&	((unsigned char*)data)[6]==18
			&&	((unsigned char*)data)[7]==70	){
						
		}
		else{
			cout << "FacilityRelatedDataRecord11::FacilityRelatedDataRecord11: Fehler, Header scheint nicht zu passen!\n";
		}
		char * temp_char = new char[4];
		memcpy(temp_char, data+13, 4); //get facility_related_id
		int facility_related_id = atoi(temp_char);
		
		if(facility_related_id==11){
		
			unsigned int num_fields=39;
			fields.resize(num_fields);
			values.resize(num_fields);
			fields[  0] = ERS_Field(  1,    1,    4,     "B4",		"-",	"Record sequence number");
			fields[  1] = ERS_Field(  2,    5,    5,     "Bl",		"-",	"1-st record subtype code");
			fields[  2] = ERS_Field(  3,    6,    6,     "Bl",		"-",	"Record type code"  );
			fields[  3] = ERS_Field(  4,    7,    7,     "Bl",		"-",	"2-nd subtype code"  );
			fields[  4] = ERS_Field(  5,    8,    8,     "Bl",		"-",	"3-rd subtype code"  );
			fields[  5] = ERS_Field(  6,    9,   12,     "B4",		"-",	"Length of this record" );
			fields[  6] = ERS_Field(  7,   13,   16,    "I4",		"-",	"Facility related data record sequence number" );
			for(int i=0; i<10; ++i){
				fields[  7+i] = ERS_Field(  8+i,   17+20*i,  36+20*i, "E20.10",	"-",	string("coefficient a") + lexical_cast<string>(i) +" to convert from map to pixel P = a0+a1*N+a2*E+a3*N*E+a4*N^2+a5*E^2+a6*N^2*E+a7*N*E^2+a8*N^3+a9*E^3" ); 
			}
			for(int i=0; i<10; ++i){
				fields[  17+i] = ERS_Field(  18+i,   217+20*i,  236+20*i, "E20.10",	"-",	string("coefficient b") + lexical_cast<string>(i) +" to convert from map to line L = b0+b1*N+b2*E+b3*N*E+b4*N^2+b5*E^2+b6*N^2*E+b7*N*E^2+b8*N^3+b9*E^3" ); 
			}
			fields[  27] = ERS_Field(  28   ,417, 420, "I4", "-", "Calibration data indicator (0: no calibration data, 1: including calibration data at the edge of upper image, 2: including calibration data at the edge of lower image , 3: including calibration data at the edges of upper and lower image)");
			fields[  28] = ERS_Field(  29   ,421,428, "I8", "-", "Start line number of calibration at upper image (In case of no calibration data, always 0)");
			fields[  29] = ERS_Field(  30   ,429,436, "I8", "-", "top line number of calibration at upper image (In case of no calibration data, always 0)");
			fields[  30] = ERS_Field(  31   ,437,444, "I8", "-", "Start line number of calibration at bottom image (In case of no calibration data, always 0)");
			fields[  31] = ERS_Field(  32   ,445,452, "I8", "-", "Stop line number of calibration at bottom image (In case of no calibration data, always 0)");
			fields[  32] = ERS_Field(  33   ,453,456, "I4", "-", "PRF switching indicator (0: a fixed PRF 1: variable PRFs except Wide observation mode, 2: wide observation mode)");
			fields[  33] = ERS_Field(  34   ,457,464, "I8", "-", "Line locator of PRF switching (1: a fixed PRF, 0: Wide observation mode)");
			fields[  34] = ERS_Field(  35   ,465,472, "I8", "-", "SIGMA- SAR processing start line number");
			fields[  35] = ERS_Field(  36   ,473,480, "I8", "-", "Number of loss lines (Level 1.0)");
			fields[  36] = ERS_Field(  37   ,481,488, "I8", "-", "Number of loss lines (range for processing in Level 1.1, Level 1.5)");
			fields[  37] = ERS_Field(  38   ,489,800, "A312", "-", "Always blank filled");
			fields[  38] = ERS_Field(  39   ,801,1024, "A224", "-", "system reserve");
			
			length = getRecordLength(data);
			int i=0;
			while(		i < num_fields
				  &&	 fields[i].end <= length){
				values[i] = getERSValue(data, fields[i]);
				++i;
			}
		}
		else{
			length = getRecordLength(data);
			cout << "Warning: Only supporting FacilityRelatedDataRecord11 (not 1-10) for PALSAR Data!\n";
		}
		
	}
};

#endif