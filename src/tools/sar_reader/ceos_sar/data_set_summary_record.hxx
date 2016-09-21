/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DATA_SET_SUMMARY_RECORD_HXX
#define DATA_SET_SUMMARY_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class DataSetSummaryReport : public ERS_Record{
	
public:
	DataSetSummaryReport(char* data){
		char revision= 0; //Start with unknown revision
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==18 
				&&	((unsigned char*)data)[5]==10
				&&	((unsigned char*)data)[6]==18
				&&	((unsigned char*)data)[7]==20	){
			revision='A';
			//cout << "DataSetSummaryReport::DataSetSummaryReport: Alles gut, Header scheint zu passen (Revision A)!\n";
		}
		else if (		((unsigned char*)data)[4]==10 
					&&	((unsigned char*)data)[5]==10
					&&	((unsigned char*)data)[6]==31
					&&	((unsigned char*)data)[7]==20	){
			revision='B';
			//cout << "DataSetSummaryReport::DataSetSummaryReport: Alles gut, Header scheint zu passen (Revision B)!\n";
		}
		else{
			cout << "DataSetSummaryReport::DataSetSummaryReport: Fehler, Header scheint nicht zu passen!\n";
		}
		
		unsigned int num_fields=131;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[  0] = ERS_Field(  1,   1,   4,  "B4",		"-",		"Record sequence number");
		fields[  1] = ERS_Field(  2,   5,   5,  "B1",		"-",		"1-st record subtype code");
		fields[  2] = ERS_Field(  3,   6,   6,  "B1",		"-",		"Record type code"  );
		fields[  3] = ERS_Field(  4,   7,   7,  "B1",		"-",		"2-nd subtype code"  );
		fields[  4] = ERS_Field(  5,   8,   8,  "B1",		"-",		"3-rd subtype code"  );
		fields[  5] = ERS_Field(  6,   9,  12,  "B4",		"bytes",	"Length of this record" );
		fields[  6] = ERS_Field(  7,  13,  16,  "I4",		"-",		"Data Set Summary Record sequence number (starts at 1)");
		fields[  7] = ERS_Field(  8,  17,  20,  "I4",		"-",		"SAR channel indicator");
		//scene parameters
		fields[  8] = ERS_Field(  9,  21,  36,	"A16",		"-",		"Reserved");
		fields[  9] = ERS_Field( 10,  37,  68,  "A32",		"-",		"Scene reference number (e.g. orbit ,  frame number)");
		fields[ 10] = ERS_Field( 11,  69, 100,  "A32",		"UTC",		"Scene centre time (UTC) <YYYYMMDDhhmmssttt>");
		fields[ 11] = ERS_Field( 12, 101, 116,  "A16",		"-",		"Spare");
		fields[ 12] = ERS_Field( 13, 117, 132,	"F16.7",	"degrees",	"Processed scene centre geodetic latitude (positive for North latitude, negative for South latitude)");
		fields[ 13] = ERS_Field( 14, 133, 148,	"F16.7",	"degrees",	"Processed scene centre geodetic longitude (negative for West longitude)");
		fields[ 14] = ERS_Field( 15, 149, 164,	"F16.7",	"-",		"Processed scene centre true heading as calculated relative to North");
		fields[ 15] = ERS_Field( 16, 165, 180,	"A16",		"-",		"Ellipsoid designator");
		fields[ 16] = ERS_Field( 17, 181, 196,	"F16.7",	"km",		"Ellipsoid semimajor axis");
		fields[ 17] = ERS_Field( 18, 197, 212,	"F16.7",	"km",		"Ellipsoid semiminor axis");
		fields[ 18] = ERS_Field( 19, 213, 228,	"F16.7",	"kg*m/s^2",	"Earth mass times gravitational constant (M . G)");
		fields[ 19] = ERS_Field( 20, 229, 244,	"A16",		"-",		"Spare");
		fields[ 20] = ERS_Field( 21, 245, 260,	"F16.7", 	"-",		"Ellipsoid J2 parameter");
		fields[ 21] = ERS_Field( 22, 261, 276,	"F16.7", 	"-",		"Ellipsoid J3 parameter");
		fields[ 22] = ERS_Field( 23, 277, 292,	"F16.7", 	"-",		"Ellipsoid J4 parameter");
		fields[ 23] = ERS_Field( 24, 293, 308,	"A16",		"-",		"Spare");
		fields[ 24] = ERS_Field( 25, 309, 324,	"F16.7", 	"-",		"Reserved");
		fields[ 25] = ERS_Field( 26, 325, 332,	"I8",		"-",		"Scene centre line number (the line number at the scene centre including zero fill) (variable)");
		fields[ 26] = ERS_Field( 27, 333, 340,	"I8",		"-",		"Scene centre pixel number (the pixel number at the scene centre including zero fill) (nominal)");
		fields[ 27] = ERS_Field( 28, 341, 356,	"F16.7", 	"km",		"Processed scene length including zero fill (nominal)");
		fields[ 28] = ERS_Field( 29, 357, 372,	"F16.7", 	"km",		"Processed scene width including zero fill (nominal) ");
		fields[ 29] = ERS_Field( 30, 373, 388,	"A16",		"-",		"Spare");
		//GENERAL MISSION / SENSOR PARAMETERS
		fields[ 30] = ERS_Field( 31, 389, 392,	"I4",		"-",		"Number of SAR channels");
		fields[ 31] = ERS_Field( 32, 393, 396,	"A4",		"-",		"Spare");
		fields[ 32] = ERS_Field( 33, 397, 412,	"A16",		"-",		"Sensor platform mission identifier");
		fields[ 33] = ERS_Field( 34, 413, 444,	"A32",		"-",		"Sensor ID and mode of operation for this channel <AAAAAA-BB-CC-DD-EF> where : AAAAAA = sensor identifier, BB = SAR band CC = resolution mode code, DD = imaging mode code, E = transmit polarisation, F = receiver polarisation, SAR,  C, HR, IM, VV");
		fields[ 34] = ERS_Field( 35, 445, 452,	"A8", 		"-",		"Orbit number");
		fields[ 35] = ERS_Field( 36, 453, 460,	"F8.3",		"degrees",	"Sensor platform geodetic latitude at nadir corresponding to scene centre (positive for North latitude)");
		fields[ 36] = ERS_Field( 37, 461, 468,	"F8.3",		"degrees",	"Sensor platform longitude at nadir corresponding to scene centre (negative for West longitude)");
		fields[ 37] = ERS_Field( 38, 469, 476,	"F8.3",		"degrees",	"Sensor platform heading at nadir corresponding to scene centre (clockwise positive from North)");
		fields[ 38] = ERS_Field( 39, 477, 484,	"F8.3",		"degrees",	"Sensor clock angle as measured relative to sensor platform flight direction");
		fields[ 39] = ERS_Field( 40, 485, 492,	"F8.3",  	"degrees",	"Incidence angle at scene centre as derived from sensor platform orientation");
		fields[ 40] = ERS_Field( 41, 493, 500,	"F8.3",  	"GHz",		"Radar frequency");
		fields[ 41] = ERS_Field( 42, 501, 516,	"F16.7",  	"m",		"Radar wavelength");
		fields[ 42] = ERS_Field( 43, 517, 518,	"A2",		"-",		"Motion compensation indicator: 00 = no compensation, 01 = on board compensation,  10 = in processor compensation, 11 = both on board and in processor");
		fields[ 43] = ERS_Field( 44, 519, 534,	"A16",		"-",		"Range pulse code specifier");
		fields[ 44] = ERS_Field( 45, 535, 550,	"E16.7",	"sec",		"Nominal range pulse (chirp) amplitude coefficient constant term");
		fields[ 45] = ERS_Field( 46, 551, 566,	"E16.7",	"sec",		"Nominal range pulse (chirp) amplitude coefficient linear term");
		fields[ 46] = ERS_Field( 47, 567, 582,	"E16.7",	"sec",		"Nominal range pulse (chirp) amplitude coefficient quadratic term");
		fields[ 47] = ERS_Field( 48, 583, 598,	"E16.7",	"sec",		"Nominal range pulse (chirp) amplitude coefficient cubic term");
		fields[ 48] = ERS_Field( 49, 599, 614,	"E16.7",	"sec",		"Nominal range pulse (chirp) amplitude coefficient quartic term");
		fields[ 49] = ERS_Field( 50, 615, 630,	"E16.7",	"cycles",	"Nominal range pulse (chirp) phase coefficient constant term");
		fields[ 50] = ERS_Field( 51, 631, 646,	"E16.7",	"Hz",		"Nominal range pulse (chirp) phase coefficient linear term");
		fields[ 51] = ERS_Field( 52, 647, 662,	"E16.7",	"Hz/sec",	"Nominal range pulse (chirp) phase coefficient quadratic term");
		fields[ 52] = ERS_Field( 53, 663, 678,	"E16.7",	"Hz/sec^2",	"Nominal range pulse (chirp) phase coefficient cubic term");
		fields[ 53] = ERS_Field( 54, 679, 694,	"E16.7",	"Hz/sec^3",	"Nominal range pulse (chirp) phase coefficient quartic term");
		fields[ 54] = ERS_Field( 55, 695, 702,	"I8", 		"samples",	"Down linked data chirp extraction index");
		fields[ 55] = ERS_Field( 56, 703, 710,	"A8",		"-",		"Spare");
		fields[ 56] = ERS_Field( 57, 711, 726,	"F16.7",	"MHz",		"Range sampling rate");
		fields[ 57] = ERS_Field( 58, 727, 742,	"F16.7", 	"-",		"Range gate delay at early edge (in time) at the start of the image (Not provided by the VMP)");
		fields[ 58] = ERS_Field( 59, 743, 758,	"F16.7", 	"microsec",	"Range pulse length");
		fields[ 59] = ERS_Field( 60, 759, 762,	"A4",		"-",		"Reserved");
		fields[ 60] = ERS_Field( 61, 763, 766,	"A4", 		"-",		"Range compressed flag (YES = range compressed data)");
		fields[ 61] = ERS_Field( 62, 767, 782,	"F16.7", 	"-",		"Reserved"); 
		fields[ 62] = ERS_Field( 63, 783, 798,	"F16.7",	"-",		"Reserved"); 
		fields[ 63] = ERS_Field( 64, 799, 806,	"I8",		"bits",		"Quantization per channel I & Q (5I 5Q/6I 6Q for OGRC/OBRC)");
		fields[ 64] = ERS_Field( 65, 807, 818,	"A12",		"-",		"Quantizer descriptor (e.g. UNIFORM,Q$)");
		fields[ 65] = ERS_Field( 66, 819, 834,	"F16.7",	"-",		"DC Bias for I, component (actual value)");
		fields[ 66] = ERS_Field( 67, 835, 850,	"F16.7",	"-",		"DC Bias for Q, component (actual value)");
		fields[ 67] = ERS_Field( 68, 851, 866,	"F16.7", 	"-",		"Gain imbalance for I & Q (actual value)");
		fields[ 68] = ERS_Field( 69, 867, 882,	"F16.7",	"-",		"Spare");
		fields[ 69] = ERS_Field( 70, 883, 898,	"F16.7",	"-",		"Spare");
		fields[ 70] = ERS_Field( 71, 899, 914,	"F16.7", 	"-",		"Reserved");
		fields[ 71] = ERS_Field( 72, 915, 930,	"F16.7",	"degrees",	"Antenna mechanical boresight angle relative to platform vertical axis at the start of the image positive to the right, negative to the left");
		fields[ 72] = ERS_Field( 73, 931, 934,	"A4", 		"-",		"Reserved");
		fields[ 73] = ERS_Field( 74, 935, 950,	"F16.7",	"Hz",		"Pulse Repetition Frequency (PRF) (actual value)");
		fields[ 74] = ERS_Field( 75, 951, 966,	"F16.7",	"-",		"Reserved");
		fields[ 75] = ERS_Field( 76, 967, 982,	"F16.7",	"-",		"Reserved");
		//SENSOR SPECIFIC PARAMETERS
		fields[ 76] = ERS_Field( 77, 983, 998,	"I16",		"-",		"Satellite encoded binary time code");
		fields[ 77] = ERS_Field( 78, 999, 1030, "A32",		"UTC",		"Satellite clock time <YYYYMMDDhhmmssttt>");
		fields[ 78] = ERS_Field( 79, 1031, 1038,"I8",		"nanosec",	"Satellite clock step length");
		fields[ 79] = ERS_Field( 80, 1039, 1046,"A8",		"-",		"Spare");
		//GENERAL PROCESSING PARAMETERS
		fields[ 80] = ERS_Field( 81, 1047, 1062, "A16", 	"-",		"Processing facility identifier (KS, FS, MS, D, PAF, IP, ES, UK, PAF)");
		fields[ 81] = ERS_Field( 82, 1063, 1070, "A8",		"-",		"Processing system identifier (VMP for D, PAF, UK, PAF and ES)");
		fields[ 82] = ERS_Field( 83, 1071, 1078, "A8",		"-",		"Processing version identifier");
		fields[ 83] = ERS_Field( 84, 1079, 1094, "A16",		"-",		"Reserved");
		fields[ 84] = ERS_Field( 85, 1095, 1110, "A16",		"-",		"Reserved");
		fields[ 85] = ERS_Field( 86, 1111, 1142, "A32",		"-",		"Product type specifier");
		fields[ 86] = ERS_Field( 87, 1143, 1174, "A32",		"-",		"Processing algorithm identifier");
		fields[ 87] = ERS_Field( 88, 1175, 1190, "F16.7", 	"looks",	"Nominal number of looks processed in azimuth");
		fields[ 88] = ERS_Field( 89, 1191, 1206, "F16.7", 	"looks",	"Nominal number of looks processed in range");
		fields[ 89] = ERS_Field( 90, 1207, 1222, "F16.7", 	"Hz",		"Bandwidth per look in azimuth (null, to, null)");
		fields[ 90] = ERS_Field( 91, 1223, 1238, "F16.7", 	"MHz",		"Bandwidth per look in range");
		fields[ 91] = ERS_Field( 92, 1239, 1254, "F16.7", 	"Hz",		"Total processor bandwidth in azimuth");
		fields[ 92] = ERS_Field( 93, 1255, 1270, "F16.7", 	"MHz",		"Total processor bandwidth in range");
		fields[ 93] = ERS_Field( 94, 1271, 1302,   "A32", 	"-",		"Weighting function designator in azimuth");
		fields[ 94] = ERS_Field( 95, 1303, 1334,   "A32", 	"-",		"Weighting function designator in range");
		fields[ 95] = ERS_Field( 96, 1335, 1350,   "A16", 	"-",		"Data input source");
		fields[ 96] = ERS_Field( 97, 1351, 1366, "F16.7", 	"m",		"Nominal resolution in range (3-dB width)");
		fields[ 97] = ERS_Field( 98, 1367, 1382, "F16.7",	"m",		"Nominal resolution in azimuth (3-dB width)");
		fields[ 98] = ERS_Field( 99, 1383, 1398, "F16.7", 	"-",		"Reserved");
		fields[ 99] = ERS_Field(100, 1399, 1414, "F16.7", 	"-",		"Reserved");
		fields[100] = ERS_Field(101, 1415, 1430, "F16.7", 	"Hz",		"Along track Doppler frequency centroid at early edge of image constant term (Not provided by the VMP)");
		fields[101] = ERS_Field(102, 1431, 1446, "F16.7", 	"Hz/sec",	"Along track Doppler frequency centroid at early edge of image linear term (Not provided by the VMP)");
		fields[102] = ERS_Field(103, 1447, 1462, "F16.7", 	"Hz/sec^2",	"Along track Doppler frequency centroid at early edge of image quadratic term (Not provided by the VMP)");
		fields[103] = ERS_Field(104, 1463, 1478,   "A16", 	"-",		"Spare");
		fields[104] = ERS_Field(105, 1479, 1494, "F16.7", 	"Hz",		"Cross track Doppler frequency centroid at early edge of image constant term (Doppler centroid)");
		fields[105] = ERS_Field(106, 1495, 1510, "F16.7", 	"Hz/sec",	"Cross track Doppler frequency centroid at early edge of image linear term (Slope of Doppler centroid)");
		fields[106] = ERS_Field(107, 1511, 1526, "F16.7", 	"Hz/sec^2",	"Cross track Doppler frequency centroid at early edge of image quadratic term");
		fields[107] = ERS_Field(108, 1527, 1534,    "A8", 	"-",		"Time direction indicator along pixel direction");
		fields[108] = ERS_Field(109, 1535, 1542,    "A8", 	"-",		"Time direction indicator along line direction");
		fields[109] = ERS_Field(110, 1543, 1558, "F16.7", 	"Hz/sec",	"Along track Doppler frequency rate at early edge of image constant term (Not provided by the VMP)");
		fields[110] = ERS_Field(111, 1559, 1574, "F16.7", 	"Hz/sec^2",	"Along track Doppler frequency rate at early edge of image linear term (Not provided by the VMP)");
		fields[111] = ERS_Field(112, 1575, 1590, "F16.7", 	"Hz/sec^3",	"Along track Doppler frequency rate at early edge of image quadratic term (Not provided by the VMP)");
		fields[112] = ERS_Field(113, 1591, 1606,   "A16", 	"-",		"Spare");
		fields[113] = ERS_Field(114, 1607, 1622, "F16.7", 	"Hz/sec",	"Cross track Doppler frequency rate at early edge of image constant term(Azimuth FM rate)");
		fields[114] = ERS_Field(115, 1623, 1638, "F16.7", 	"Hz/sec^2",	"Cross track Doppler frequency rate at early edge of image linear term(Slope of Az. FM rate)");
		fields[115] = ERS_Field(116, 1639, 1654, "F16.7", 	"Hz/sec^3",	"Cross track Doppler frequency rate at early edge of image quadratic term (Not provided by the VMP)");
		fields[116] = ERS_Field(117, 1655, 1670, "F16.7", 	"microsec",	"HH channel electronic delay time (RSD product only)" );
		fields[117] = ERS_Field(118, 1671, 1678,    "A8", 	"-",		"Line content indicator ('RANGE$$$' or 'EASTING$' if geocoded image)");
		fields[118] = ERS_Field(119, 1679, 1682,    "A4", 	"-",		"Clutter lock applied flag (MLD, MLC, SLC: 'YES$', RSD: 'NONE')");
		fields[119] = ERS_Field(120, 1683, 1686,    "A4", 	"-",		"Autofocussing applied flag (MLD, MLC, SLC: 'YES$', RSD: 'NONE')");
		fields[120] = ERS_Field(121, 1687, 1702, "F16.7", 	"m",		"Line spacing");
		fields[121] = ERS_Field(122, 1703, 1718, "F16.7", 	"m",		"Pixel spacing");
		fields[122] = ERS_Field(123, 1719, 1734,   "A16", 	"-",		"Processor range compression designator ('DIGITAL$CHIRP', 'ANALYTIC$CHIRP', or 'MEASURED$CHIRP')");
		fields[123] = ERS_Field(124, 1735, 1750,   "A16", 	"-",		"Spares" );
		fields[124] = ERS_Field(125, 1751, 1766,   "A16", 	"-",		"Spares");
		//SENSOR SPECIFIC LOCAL USE SEGMENT
		if(revision=='A'){
			fields[125] = ERS_Field(126, 1767, 1782, "F16.7", 	"millisec",		"Zero-Doppler range time (two, way) of first range pixel");                //  126/1
			fields[126] = ERS_Field(126, 1783, 1798, "F16.7", 	"millisec",		"Zero-Doppler range time (two, way) of centre range pixel");               //  126/2
			fields[127] = ERS_Field(126, 1799, 1814, "F16.7", 	"millisec",		"Zero-Doppler range time (two, way) of last valid range pixel");           //  126/3
			fields[128] = ERS_Field(127, 1815, 1830, "F16.7", 	"millisec",		"Zero-Doppler azimuth time of first azimuth pixel"); 
			fields[129] = ERS_Field(127, 1831, 1846, "F16.7", 	"millisec",		"Zero-Doppler azimuth time of centre azimuth pixel");
			fields[130] = ERS_Field(127, 1847, 1862, "F16.7", 	"millisec",		"Zero-Doppler azimuth time of last azimuth pixel"); 
		}
		else if(revision=='B'){
			fields[125] = ERS_Field(126, 1767, 1782, "F16.7",	"millisec",		"Zero-Doppler range time (two, way) of first range pixel");                //  126/1
			fields[126] = ERS_Field(126, 1783, 1798, "F16.7",	"millisec",		"Zero-Doppler range time (two, way) of centre range pixel");               //  126/2
			fields[127] = ERS_Field(126, 1799, 1814, "F16.7",	"millisec",		"Zero-Doppler range time (two, way) of last valid range pixel");           //  126/3
			fields[128] = ERS_Field(127, 1815, 1838, "A24",		"UTC",			"Zero-Doppler azimuth time of first azimuth pixel <dd, MMM, yyyy$hh:mm:ss.ttt>"); //  126/4
			fields[129] = ERS_Field(127, 1839, 1862, "A24",		"UTC",			"Zero-Doppler azimuth time of centre azimuth pixel <dd, MMM, yyyy$hh:mm:ss.ttt>");//  126/5
			fields[130] = ERS_Field(127, 1863, 1886, "A24", 	"UTC",			"Zero-Doppler azimuth time of last azimuth pixel <dd, MMM, yyyy$hh:mm:ss.ttt>");  //  126/6
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

#endif