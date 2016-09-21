/************************************************************************/

/************************************************************************/
 
#include <iostream>
#include <fstream>
#include <vector>
#include "ceos_sar/volume_descriptor_record.hxx"
#include "ceos_sar/file_pointer_record.hxx"
#include "ceos_sar/text_record.hxx"
#include "ceos_sar/file_descriptor_record.hxx"
#include "ceos_sar/data_quality_summary_record.hxx"
#include "ceos_sar/data_set_summary_record.hxx"
#include "ceos_sar/map_projection_data_record.hxx"
#include "ceos_sar/platform_position_data_record.hxx"
#include "ceos_sar/attitude_data_record.hxx"
#include "ceos_sar/radiometric_data_record.hxx"
#include "ceos_sar/radiometric_compensation_data_record.hxx"
#include "ceos_sar/radar_parameter_update_data_record.hxx"
#include "ceos_sar/facility_related_data_record.hxx"

using namespace std;


enum ERSFileFormat {
	VOLUME_DESCRIPTOR_RECORD,
	FILE_POINTER_RECORD,
	TEXT_RECORD,
	FILE_DESCRIPTOR_RECORD,
	DATA_QUALITY_SUMMARY_RECORD,
	DATA_SET_SUMMARY_RECORD_B,
	DATA_SET_SUMMARY_RECORD_A,
	MAP_PROJECTION_DATA_RECORD, 
	PLATFORM_POSITION_DATA_RECORD,
	ATTITUDE_DATA_RECORD,
	RADIOMETRIC_DATA_RECORD,
	RADIOMETRIC_COMPENSATION_DATA_RECORD,
	RADAR_PARAMETER_UPDATE_DATA_RECORD,
	FACILITY_RELATED_DATA_RECORD_GENERAL_TYPE,
	FACILITY_RELATED_DATA_RECORD_PCS_QUALITY_TYPE_DEFINITION,
	FACILITY_RELATED_DATA_RECORD_11,
	SAR_BAND_DATA_A,
	SAR_BAND_DATA_B,
	INVALID_RECORD = -1
};

ERSFileFormat getRecordType(unsigned short b1, unsigned short  b2, unsigned short  b3, unsigned short  b4){
	
	//Volume Related Records don't seem to change from CEOS-SAR-CCT Rev. A to Rev. B
	if(b1==192 && b2==192 && b3==18 && b4==18){
		return VOLUME_DESCRIPTOR_RECORD;
	}
	else if(b1==219 && b2==192 && b3==18 && b4==18){
		return FILE_POINTER_RECORD;
	}
	else if(b1==18 && (b2==63 || b2==192)&& b3==18 && b4==18){
		return TEXT_RECORD;
	}	
	
	// The File/ Data related records may change from one Version to another
	// in code of header or meaning of fields!
	else if((b1==11 || b1==63) && b2==192 && b3==18 && b4==18){
		return FILE_DESCRIPTOR_RECORD;
	}
	else if(b1==18 && b2==60 && b3==18 && b4==20){
		return DATA_QUALITY_SUMMARY_RECORD;
	}	   
	else if(b1==10 && b2==10 && b3==31 && b4==20){
		return DATA_SET_SUMMARY_RECORD_B;
	}
	else if(b1==18 && b2==10 && b3==18 && b4==20){
		return DATA_SET_SUMMARY_RECORD_A;
	}
	else if(b1==10 && b2==20 && b3==31 && b4==20){
		return MAP_PROJECTION_DATA_RECORD;
	}
	else if(b1==18 && b2==20 && b3==18 && b4==20){
		return MAP_PROJECTION_DATA_RECORD;
	}
	else if(b1==10 && b2==30 && b3==31 && b4==20){
		return PLATFORM_POSITION_DATA_RECORD;
	}
	else if(b1==18 && b2==30 && b3==18 && b4==20){
		return PLATFORM_POSITION_DATA_RECORD;
	} 
	else if(b1==18 && b2==40 && b3==18 && b4==20){
		return ATTITUDE_DATA_RECORD;
	}
	else if(b1==18 && b2==50 && b3==18 && b4==20){
		return RADIOMETRIC_DATA_RECORD;
	}	
	else if(b1==18 && b2==51 && b3==18 && b4==20){
		return RADIOMETRIC_COMPENSATION_DATA_RECORD;
	}	   
	else if(b1==18 && b2==100 && b3==18 && b4==20){
		return RADAR_PARAMETER_UPDATE_DATA_RECORD;
	}  
	else if(b1==10 && b2==200 && b3==31 && b4==50){
		return FACILITY_RELATED_DATA_RECORD_GENERAL_TYPE;
	}	
	else if(b1==18 && b2==200 && b3==18 && b4==50){
		return FACILITY_RELATED_DATA_RECORD_GENERAL_TYPE;
	}	
	else if(b1==18 && b2==200 && b3==18 && b4==50){
		return FACILITY_RELATED_DATA_RECORD_PCS_QUALITY_TYPE_DEFINITION;
	}	
	else if(b1==18 && b2==200 && b3==18 && b4==70){
		return FACILITY_RELATED_DATA_RECORD_11;
	}		
	else if(b1==192 && b2==192 && b3==63 && b4==18){
		//TODO: Whats this?
	}	
	else if(b1==50 && b2==11 && b3==31 && b4==20){
		return SAR_BAND_DATA_B;
	}	
	else if(b1==50 && b2==11 && b3==18 && b4==20){
		return SAR_BAND_DATA_A;
	}	
	else{
		cout << "INVALID_RECORD (IDs: "<< b1 <<", " << b2 <<", " << b3 <<", " << b4 <<")\n";
	}
	return INVALID_RECORD;
}

void getGenericHeaderInfos(unsigned char * raw_data, unsigned int& record_sequence_number, unsigned short& code1, unsigned short& code2, unsigned short& code3, unsigned short& code4, unsigned int& length){
	unsigned int temp_i=0;
	unsigned short temp_s=0;
	
	//result[  0] = ERS_Field(  1,   1,   4,   "B4", "Record sequence number");
	temp_i= raw_data[3] | raw_data[2] << 8 | raw_data[1] << 16 | raw_data[0] << 24;
	record_sequence_number = temp_i;
	
	//result[  1] = ERS_Field(  2,   5,   5,   "Bl", "1-st record subtype code");
	code1 = raw_data[4];
	//result[  2] = ERS_Field(  3,   6,   6,   "Bl", "Record type code"  );
	code2 = raw_data[5];
	//result[  3] = ERS_Field(  4,   7,   7,   "Bl", "2-nd subtype code"  );
	code3 = raw_data[6];
	//result[  4] = ERS_Field(  5,   8,   8,   "Bl", "3-rd subtype code"  );
	code4 = raw_data[7];
	//result[  5] = ERS_Field(  6,   9,  12,   "B4", "Length of this record" );;
	temp_i=0;
	temp_i= raw_data[11] | raw_data[10] << 8 | raw_data[9] << 16 | raw_data[8] << 24;
	length = temp_i;
	
}

//Main method (program)
int main(int argc, char ** argv)
{
    if(argc != 2){
        cout	<< "This program tries to extract the information out of a SAR header file.\n"
				<< "Usage: " << argv[0] << " filename\n";        
        return 1;
    }
    
    try{		
		const string filename(argv[1]);
		
		unsigned int length;
		char * buffer;
		
		ifstream filestr;
		filestr.open (filename.c_str(), ios::binary );
		
		if (!filestr.is_open()){
			cout << "Unable to open file: " << filename << "\n";
			return 1; 
		}
		// get length of file:
		filestr.seekg (0, ios::end);
		length = filestr.tellg();
		filestr.seekg (0, ios::beg);
		
		// allocate memory:
		buffer = new char [length];
		
		// read data as a block:
		filestr.read (buffer,length);
		
		//record sequence number
		
		vector<ERS_Field> record;
		char * temp_r = NULL;
		
		ERSFileFormat	last_record_type = INVALID_RECORD, 
						current_record_type = INVALID_RECORD;
		
		ERS_Record * current_record = NULL;
		
		for(unsigned int offset=0; offset < length;){
			if(temp_r)
				delete[] temp_r;
			
			unsigned char temp_h[12];  
			filestr.seekg (offset, ios::beg);
			filestr.read ((char*)temp_h,12);
			//for (int i=0; i<12; ++i)
			//	std::cout<< "header_byte (" << i << "):\t "<< (unsigned int) (temp_h[i]) << "\n";
			
			unsigned int nr,l;
			unsigned short  c1,c2,c3,c4;
			getGenericHeaderInfos(temp_h,nr, c1,c2,c3,c4,l);
			//cerr << "Current Interval;" << offset<< ","<<l << "\n";
			
			filestr.seekg (offset, ios::beg);
			temp_r= new char[l];
			filestr.read (temp_r,l); 
			
			current_record_type = getRecordType(c1,c2,c3,c4);
			
			if (current_record){ 
				delete current_record;
				current_record = NULL;
			}
			switch(current_record_type){
				case VOLUME_DESCRIPTOR_RECORD :
					current_record = new VolumeDescriptorRecord(temp_r);
					cout << "VOLUME_DESCRIPTOR_RECORD\n"<< current_record->toCSV(); 
					break;
				case FILE_POINTER_RECORD :
					current_record = new  FilePointerRecord(temp_r);
					cout << "FILE_POINTER_RECORD\n"<<current_record->toCSV(); 
					break;
				case TEXT_RECORD :
					current_record = new TextRecord(temp_r);
					cout << "TEXT_RECORD\n"<<current_record->toCSV(); 
					break;
				case FILE_DESCRIPTOR_RECORD :
					current_record = new FileDescriptorRecord(temp_r);
					cout << "FILE_DESCRIPTOR_RECORD\n"<<current_record->toCSV(); 
					break;
				case DATA_QUALITY_SUMMARY_RECORD :
					current_record = new DataQualitySummaryRecord(temp_r);
					cout << "DATA_QUALITY_SUMMARY_RECORD\n"<< current_record->toCSV(); 
					break;
				case DATA_SET_SUMMARY_RECORD_A : 
				case DATA_SET_SUMMARY_RECORD_B :
					current_record = new DataSetSummaryReport(temp_r);
					cout << "DATA_SET_SUMMARY_RECORD\n"<< current_record->toCSV(); 
					break;
				case MAP_PROJECTION_DATA_RECORD :
					current_record = new MapProjectionDataRecord(temp_r);
					cout << "MAP_PROJECTION_DATA_RECORD\n"<<current_record->toCSV(); 
					break;
				case PLATFORM_POSITION_DATA_RECORD :
					current_record = new PlatformPositionDataRecord(temp_r);
					cout << "PLATFORM_POSITION_DATA_RECORD\n"<< current_record->toCSV();  
					break;
				case ATTITUDE_DATA_RECORD :
					current_record = new AttitudeDataRecord(temp_r);
					cout << "ATTITUDE_DATA_RECORD\n"<< current_record->toCSV();  
					break;
				case RADIOMETRIC_DATA_RECORD :
					current_record = new RadiometricDataRecord(temp_r);
					cout << "RADIOMETRIC_DATA_RECORD\n"<< current_record->toCSV(); 
					break;
				case RADIOMETRIC_COMPENSATION_DATA_RECORD :
					current_record = new RadiometricCompensationDataRecord(temp_r);
					cout << "RADIOMETRIC_COMPENSATION_DATA_RECORD\n"<< current_record->toCSV(); 
					break;
				case RADAR_PARAMETER_UPDATE_DATA_RECORD :
					current_record = new RadarParameterUpdateDataRecord(temp_r);
					cout << "RADAR_PARAMETER_UPDATE_DATA_RECORD\n" << current_record->toCSV(); 
					break;
				case  FACILITY_RELATED_DATA_RECORD_GENERAL_TYPE:
					current_record = new FacilityRelatedDataRecordGeneralType(temp_r);
					cout << "FACILITY_RELATED_DATA_RECORD_GENERAL_TYPE\n" << current_record->toCSV();  
					break;
				case  FACILITY_RELATED_DATA_RECORD_11:
					current_record = new FacilityRelatedDataRecord11(temp_r);
					cout << "FACILITY_RELATED_DATA_RECORD11\n" << current_record->toCSV();  
					break;
                default:
                    cerr << "UNSUPPORTED RECORD TYPE ENCOUNTERED.";
                    break;
			}
			last_record_type = current_record_type;
			// For "really" variant fields, the real length of the record can only be determined by
			// reding in the record!
			//offset+=l;
			if(current_record){
				//cerr << "1. case: offset = " << offset <<" += " << current_record->getLength() << "\n";
				offset+=current_record->getLength();
			}
			else{
				//cerr << "2. case: offset = " << offset <<" += " << l << "\n";
				offset+=l;
			}
			
		}
		filestr.close();
		
		
		delete[] buffer;
		
	}	
    catch (std::exception & e){
        // catch any errors that might have occured and print their reason
        std::cout << e.what() << std::endl;
        return 1;
    }
    
return 0;
}