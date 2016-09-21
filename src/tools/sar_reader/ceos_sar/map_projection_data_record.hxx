/*
 *  text_record.hxx
 *  
 *
 *  Created by seppke on 03.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MAP_PROJECTION_DATA_RECORD_HXX
#define MAP_PROJECTION_DATA_RECORD_HXX

#include "ers_field.hxx"
#include "ers_record.hxx"
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;

class MapProjectionDataRecord : public ERS_Record{
	
public:
	MapProjectionDataRecord(char* data){
		//get ESA-Header:		
		if (		((unsigned char*)data)[4]==18 
			&&	((unsigned char*)data)[5]==20
			&&	((unsigned char*)data)[6]==18
			&&	((unsigned char*)data)[7]==20	){
			
			//cout << "MapProjectionDataRecord::MapProjectionDataRecord: Alles gut, Header scheint zu passen (Revision A)!\n";
		}
		else if (		((unsigned char*)data)[4]==10 
					&&	((unsigned char*)data)[5]==20
					&&	((unsigned char*)data)[6]==31
					&&	((unsigned char*)data)[7]==20	){
			//cout << "MapProjectionDataRecord::MapProjectionDataRecord: Alles gut, Header scheint zu passen (Revision B)!\n";
		}
		else{
			cout << "MapProjectionDataRecord::MapProjectionDataRecord: Fehler, Header scheint nicht zu passen!\n";
		}
		
		unsigned int num_fields=96;
		fields.resize(num_fields);
		values.resize(num_fields);
		fields[  0] = ERS_Field(  1,   1,   4,  "B4",		"-",		"Record sequence number");
		fields[  1] = ERS_Field(  2,   5,   5,  "B1",		"-",		"1-st record subtype code");
		fields[  2] = ERS_Field(  3,   6,   6,  "B1",		"-",		"Record type code"  );
		fields[  3] = ERS_Field(  4,   7,   7,  "B1",		"-",		"2-nd subtype code"  );
		fields[  4] = ERS_Field(  5,   8,   8,  "B1",		"-",		"3-rd subtype code"  );
		fields[  5] = ERS_Field(  6,   9,  12,  "B4",		"bytes",	"Length of this record" );
		fields[  6] = ERS_Field(  7,  13,  28,  "A16",		"-",		"spare");
		fields[  7] = ERS_Field(  8,  29,  60,	"A32",		"-",		"Map projection descriptor (MLD, MLC: GROUND$RANGE$$$..., SLC, RSD: SLANT$RANGE$$$..., GEC, GTC: UTM, SSMI, or UPS)" );
		fields[  8] = ERS_Field(  9,  61,  76,  "I16",		"pixels",	"Number of pixels per line of image" );
		fields[  9] = ERS_Field( 10,  77,  92,  "I16",		"lines",	"Number of lines" );
		fields[ 10] = ERS_Field( 11,  93, 108,  "F16.7",	"m",		"Nominal inter-pixel distance in output scene" );
		fields[ 11] = ERS_Field( 12, 109, 124,  "F16.7",	"m",		"Nominal inter-line distance in output scene" );
		fields[ 12] = ERS_Field( 13, 125, 140,  "F16.7",	"degrees",	"Orientation at output scene center, for geocoded  products this is simply the convergence of the meridians, tie. the angle between geographic north and map grid north (Angle of projection axis from true North)" );
		fields[ 13] = ERS_Field( 14, 141, 156,  "F16.7",	"degrees",	"Actual platform orbital inclination" );
		fields[ 14] = ERS_Field( 15, 157, 172,  "F16.7", 	"degrees",	"Actual ascending node (longitude at equator)" );
		fields[ 15] = ERS_Field( 16, 173, 188,  "F16.7", 	"km",		"Distance of platform at input scene center from the geocenter" );
		fields[ 16] = ERS_Field( 17, 189, 204,  "F16.7", 	"km",		"Geodedic altitude of the platform relative to the ellipsoid" );
		fields[ 17] = ERS_Field( 18, 205, 220,  "F16.7", 	"km/sec",	"Actual ground speed at nadir at input scene center time" );
		fields[ 18] = ERS_Field( 19, 221, 236,  "F16.7", 	"degrees",	"Platform heading: effective subplatform track direction angle relative to true north, including the effects of orbital inclination and skew due to earth rotation." );
		//PROJECTION ELLIPSOID PARAMETERS
		fields[ 19] = ERS_Field( 20, 237,  268,   "A32",	"-",		"Name of reference ellipsoid (e.g. GEM 10B)");
		fields[ 20] = ERS_Field( 21, 269,  284, "F16.7",  	"km",		"Semimajor axis of ref. ellipsoid");
		fields[ 21] = ERS_Field( 22, 285,  300, "F16.7", 	"km",		 "Semiminor axis of ref. ellipsoid" );
		fields[ 22] = ERS_Field( 23, 301,  316, "F16.7",  	"m",		"Datum shift parameter referenced to Greenwich. dx");
		fields[ 23] = ERS_Field( 24, 317,  332, "F16.7",  	"m",		"Datum shift parameter perpendicular to Greenwich. dy");
		fields[ 24] = ERS_Field( 25, 333,  348, "F16.7",  	"m",		"Datum shift parameter direction of the rotation axis. dz");
		fields[ 25] = ERS_Field( 26, 349,  364, "F16.7",  	"-",		"Additional datum shift parameter 1st rotation angle");
		fields[ 26] = ERS_Field( 27, 365,  380, "F16.7",  	"-",		"Additional datum shift parameter 2nd rotation angle");
		fields[ 27] = ERS_Field( 28, 381,  396, "F16.7",  	"-",		"Additional datum shift parameter 3rd rotation angle");
		fields[ 28] = ERS_Field( 29, 397,  412, "F16.7",  	"-",		"Scale factor of reference ellipsoid" );
		//MAP PROJECTION DESIGNATOR
		fields[ 29] = ERS_Field( 30, 413,  444,   "A32",  	"-",		"Alphanumeric description of map projection" );
		//UTM,  PROJECTION (1ST DEFAULT)
		fields[ 30] = ERS_Field( 31, 445,  476,   "A32",  	"-",		 "UTM descriptor" );
		fields[ 31] = ERS_Field( 32, 477,  480,    "A4",   	"-",		"Signature of the UTM zone" );
		fields[ 32] = ERS_Field( 33, 481,  496, "F16.7",   	"m",		"Map origin (false easting: 500,000 m)" );
		fields[ 33] = ERS_Field( 34, 497,  512, "F16.7",   	"m",		"Map origin (false northing: 0 m if northern hemisphere, 10,000,000 m if southern hemisphere)" );
		fields[ 34] = ERS_Field( 35, 513,  528, "F16.7",   	"degrees",	"Center of projection longitude" );
		fields[ 35] = ERS_Field( 36, 529,  544, "F16.7",   	"degrees",	"Center of projection latitude" );
		fields[ 36] = ERS_Field( 37, 545,  560, "F16.7",   	"degrees",	"1st standard parallel" );
		fields[ 37] = ERS_Field( 38, 561,  576, "F16.7",   	"degrees",	"2nd standard parallel" );
		fields[ 38] = ERS_Field( 39, 577,  592, "F16.7",   	"-",		"Scale factor");
		//UPS,  PROJECTION (2ND DEFAULT)
		fields[ 39] = ERS_Field( 40, 593,  624,   "A32",   	"-",		"UPS descriptor");
		fields[ 40] = ERS_Field( 41, 625,  640, "F16.7",   	"degrees",	"Center of projection longitude");
		fields[ 41] = ERS_Field( 42, 641,  656, "F16.7",   	"degrees",	"Center of projection latitude");
		fields[ 42] = ERS_Field( 43, 657,  672, "F16.7",   	"-",		"Scale factor" );
		//NATIONAL SYSTEMS PROJECTION (any others, especially SSMI)
		fields[ 43] = ERS_Field( 44, 673,  704,   "A32",   	"-",		"Projection descriptor" );
		fields[ 44] = ERS_Field( 45, 705,  720, "F16.7",   	"-",		"Map origin (false easting)" );
		fields[ 45] = ERS_Field( 46, 721,  736, "F16.7",   	"-",		"Map origin (false northing)" );
		fields[ 46] = ERS_Field( 47, 737,  752, "F16.7",   	"degrees",	"Center of projection longitude" );
		fields[ 47] = ERS_Field( 48, 753,  768, "F16.7",   	"degrees",	"Center of projection latitude" );
		fields[ 48] = ERS_Field( 49, 769,  784, "F16.7",   	"-",		"Standard parallels (deg, default: ,  9999.99)" );
		fields[ 49] = ERS_Field( 50, 785,  800, "F16.7",   	"-",		"Standard parallels (deg, default: ,  9999.99)" );
		fields[ 50] = ERS_Field( 51, 801,  816, "F16.7",    "-",		"Standard parallels (deg, default: ,  9999.99)" );
		fields[ 51] = ERS_Field( 52, 817,  832, "F16.7",   	"-",		"Standard parallels (deg, default: ,  9999.99)" );
		fields[ 52] = ERS_Field( 53, 833,  848, "F16.7",   	"-",		"Central meridian (deg, default: ,  9999.99)" );
		fields[ 53] = ERS_Field( 54, 849,  864, "F16.7",   	"-",		"Central meridian (deg, default: ,  9999.99)" );
		fields[ 54] = ERS_Field( 55, 865,  880, "F16.7",   	"-",		"Central meridian (deg, default: ,  9999.99)" );
		fields[ 55] = ERS_Field( 56, 881,  896,   "A16",   	"-",		"Spares");
		fields[ 56] = ERS_Field( 57, 897,  912,   "A16",   	"-",		"Spares");
		fields[ 57] = ERS_Field( 58, 913,  928,   "A16",   	"-",		"Spares");
		fields[ 58] = ERS_Field( 59, 928,  944,   "A16",   	"-",		"Spares");
		//COORDINATES OF FOUR CORNER POINTS
		fields[ 59] = ERS_Field( 60,  945,  960, "F16.7",   "m",		"Top left corner northing" );
		fields[ 60] = ERS_Field( 61,  961,  976, "F16.7",   "m",		"Top left corner easting" );
		fields[ 61] = ERS_Field( 62,  977,  992, "F16.7",   "m",		"Top right corner northing" );
		fields[ 62] = ERS_Field( 63,  993, 1008, "F16.7",   "m",		"Top right corner easting" );
		fields[ 63] = ERS_Field( 64, 1009, 1024, "F16.7",   "m",		"Bottom right corner northing" );
		fields[ 64] = ERS_Field( 65, 1025, 1040, "F16.7",   "m",		"Bottom right corner easting" );
		fields[ 65] = ERS_Field( 66, 1041, 1056, "F16.7",   "m",		"Bottom left corner northing" );
		fields[ 66] = ERS_Field( 67, 1057, 1072, "F16.7",   "m",		"Bottom left corner easting" );
		fields[ 67] = ERS_Field( 68, 1073, 1088, "F16.7",   "degrees",	"Near range early time latitude" );
		fields[ 68] = ERS_Field( 69, 1089, 1104, "F16.7",   "degrees",	"Near range early time longitude" );
		fields[ 69] = ERS_Field( 70, 1105, 1120, "F16.7",   "degrees",	"Far range early time latitude" );
		fields[ 70] = ERS_Field( 71, 1121, 1136, "F16.7",   "degrees",	"Far range early time longitude" );
		fields[ 71] = ERS_Field( 72, 1137, 1152, "F16.7",   "degrees",	"Far range late time latitude" );
		fields[ 72] = ERS_Field( 73, 1153, 1168, "F16.7",   "degrees",	"Far range late time longitude" );
		fields[ 73] = ERS_Field( 74, 1169, 1184, "F16.7",   "degrees",	"Near range late time latitude" );
		fields[ 74] = ERS_Field( 75, 1185, 1200, "F16.7",   "degrees",	"Near range late time longitude" );
		fields[ 75] = ERS_Field( 76, 1201, 1216, "F16.7",   "m",		"Top left corner terrain height relative to ellipsoid" );
		fields[ 76] = ERS_Field( 77, 1217, 1232, "F16.7",   "m",		"Top right corner terrain height" );
		fields[ 77] = ERS_Field( 78, 1233, 1248, "F16.7",   "m",		"Bottom right corner height" );
		fields[ 78] = ERS_Field( 79, 1249, 1264, "F16.7",   "m",		"Bottom left corner height" );
		//COEFFS. FOR IMAGE TO MAP TO IMAGE CONVERSION
		fields[ 79] = ERS_Field( 80, 1265, 1284, "E20.10",  "-",		 "Image->map coordinate A11 of: E = A11 + A12*L + A13*C + A14*L*C" );
		fields[ 80] = ERS_Field( 81, 1285, 1304, "E20.10",	"-",		 "Image->map coordinate A12 of: E = A11 + A12*L + A13*C + A14*L*C" );
		fields[ 81] = ERS_Field( 82, 1305, 1324, "E20.10",  "-",		 "Image->map coordinate A13 of: E = A11 + A12*L + A13*C + A14*L*C" );
		fields[ 82] = ERS_Field( 83, 1325, 1344, "E20.10",  "-",		 "Image->map coordinate A14 of: E = A11 + A12*L + A13*C + A14*L*C" );
		fields[ 83] = ERS_Field( 84, 1345, 1364, "E20.10",  "-",		 "Image->map coordinate A21 of: N = A21 + A22*L + A23*C + A24*L*C" );
		fields[ 84] = ERS_Field( 85, 1365, 1384, "E20.10",  "-",		 "Image->map coordinate A22 of: N = A21 + A22*L + A23*C + A24*L*C" );
		fields[ 85] = ERS_Field( 86, 1385, 1404, "E20.10",  "-",		 "Image->map coordinate A23 of: N = A21 + A22*L + A23*C + A24*L*C" );
		fields[ 86] = ERS_Field( 87, 1405, 1424, "E20.10",  "-",		 "Image->map coordinate A24 of: N = A21 + A22*L + A23*C + A24*L*C" ); 
		fields[ 87] = ERS_Field( 88, 1425, 1444, "E20.10",  "-",		 "Map->image coordinate B11 of: L = B11 + B12*E + B13*N + B14*E*N" );
		fields[ 88] = ERS_Field( 89, 1445, 1464, "E20.10",  "-",		 "Map->image coordinate B12 of: L = B11 + B12*E + B13*N + B14*E*N" );
		fields[ 89] = ERS_Field( 90, 1465, 1484, "E20.10",  "-",		 "Map->image coordinate B13 of: L = B11 + B12*E + B13*N + B14*E*N" );
		fields[ 90] = ERS_Field( 91, 1485, 1504, "E20.10",  "-",		 "Map->image coordinate B14 of: L = B11 + B12*E + B13*N + B14*E*N" );
		fields[ 91] = ERS_Field( 92, 1505, 1524, "E20.10",  "-",		 "Map->image coordinate B21 of: P = B21 + B22*E + B23*N + B24*E*N" );
		fields[ 92] = ERS_Field( 93, 1525, 1544, "E20.10",  "-",		 "Map->image coordinate B22 of: P = B21 + B22*E + B23*N + B24*E*N" );
		fields[ 93] = ERS_Field( 94, 1545, 1564, "E20.10",  "-",		 "Map->image coordinate B23 of: P = B21 + B22*E + B23*N + B24*E*N" );
		fields[ 94] = ERS_Field( 95, 1565, 1584, "E20.10",  "-",		 "Map->image coordinate B24 of: P = B21 + B22*E + B23*N + B24*E*N" ); 
		fields[ 95] = ERS_Field( 96, 1585, 1620,    "A36",  "-",		 "Spares");
		
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