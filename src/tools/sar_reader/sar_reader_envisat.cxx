/************************************************************************/

/************************************************************************/
 
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "envisat_sar/Main_Product_Header.hxx"
#include "envisat_sar/ASAR_Image_Products_SPH.hxx"

#include "envisat_sar/MAIN_PROCESSING_PARAMS_ADS.hxx"
#include "envisat_sar/MDS1_SQ_ADS.hxx"
#include "envisat_sar/MDS1_ANTENNA_ELEV_PATT_ADS.hxx"
#include "envisat_sar/GEOLOCATION_GRID_ADS.hxx"
#include "envisat_sar/DOP_CENTROID_COEFFS_ADS.hxx"
#include "envisat_sar/SR_GR_ADS.hxx"
#include "envisat_sar/CHIRP_PARAMS_ADS.hxx"

using namespace std;

//Main method (program)
int main(int argc, char ** argv)
{
    if(argc != 2){
        cout	<< "This program tries to extract the information out of a ENVISAT SAR header file.\n"
				<< "Usage: " << argv[0] << " filename\n";        
        return 1;
    }
    
    try{		
		const string filename(argv[1]);
		
		int length;
		
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
		
		//temporary record
		unsigned char * temp_r = NULL;
		
		for(int offset=0; offset < length;){
			if(temp_r)
				delete[] temp_r;
			
			unsigned char temp_h[9];  
			filestr.seekg (offset, ios::beg);
			filestr.read ((char*)temp_h,8);
			//terminate CString
			temp_h[8]=0;
			
			unsigned int l=0;
			//determine length of record:
			if ( strcmp((const char*)temp_h,"PRODUCT=")==0){
				cout << "--\n MAIN_PRODUCT_DESCRIPTION \n--\n";
				l = 1247;
			}
			else if (  strcmp((const char*)temp_h,"SPH_DESC")==0){
				cout << "--\n SPECIFIC_PRODUCT_DESCRIPTION \n--\n";
				l = 1059;
			}else if (  strcmp((const char*)temp_h,"DS_NAME=")==0){
				l = 280;
			}
			if (!l)
				break;
			filestr.seekg (offset, ios::beg);
			temp_r= new unsigned char[l+1];
			filestr.read ((char*)temp_r,l); 
			//terminate CString
			temp_r[l] = 0;
			
			if(l == 1247){
				Main_Product_Header ds((char*)temp_r);
				cout << ds.toCSV();
			}
			if(l == 1059){
				ASAR_Image_Products_SPH ds((char*)temp_r);
				cout << ds.toCSV();
			}
			
			if (  strcmp((const char*)temp_h,"DS_NAME=")==0){
				//Determine size of  "--\n FOLLOWING_DATASET_HEADER \n--\n";
				char p_ds_name[29];
				memcpy(p_ds_name, temp_r+9,28);
				p_ds_name[28]=0;
				string ds_name((const char*) p_ds_name);
				trim(ds_name);
				
				char p_dsr_size[11];
				memcpy(p_dsr_size, temp_r+229,10);
				p_dsr_size[10]=0;
				unsigned int dsr_size = atoi(p_dsr_size);
				
				if(dsr_size){
					/*cout << "--\n FOLLOWING_DATASET_HEADER \n--\n"
					     << temp_r << endl << ds_name << endl;
					*/					
					char p_ds_offset[21];
					memcpy(p_ds_offset, temp_r+134,20);
					p_ds_offset[20]=0;
					unsigned int ds_offset = atoi(p_ds_offset);
					
					char p_ds_size[21];
					memcpy(p_ds_size, temp_r+171,20);
					p_ds_size[20]=0;
					unsigned int ds_size = atoi(p_ds_size);
					if (ds_name != "MDS1"){
						cout <<ds_name << "\n";
						for(unsigned int i = 0; i< ds_size/dsr_size;++i){
							char * temp_dsr = new char[dsr_size+1];
							filestr.seekg (ds_offset+i*dsr_size, ios::beg);
							filestr.read (temp_dsr,dsr_size); 
							temp_dsr[dsr_size]=0;
							
							cout << i+1 << ". record of dataset: \n";
							
							if(ds_name == "MDS1 SQ ADS"){
								MDS1_SQ_ADS ds(temp_dsr);
								cout << ds.toCSV();
							}
							
							if(ds_name == "GEOLOCATION GRID ADS"){
								GEOLOCATION_GRID_ADS ds(temp_dsr);
								cout << ds.toCSV();
							}
							
							if(ds_name == "MDS1 ANTENNA ELEV PATT ADS"){
								MDS1_ANTENNA_ELEV_PATT_ADS ds(temp_dsr);
								cout << ds.toCSV();
							}
							if(ds_name == "MAIN PROCESSING PARAMS ADS"){
								MAIN_PROCESSING_PARAMS_ADS ds(temp_dsr);
								cout << ds.toCSV();
							}
							if(ds_name == "DOP CENTROID COEFFS ADS"){
								DOP_CENTROID_COEFFS_ADS ds(temp_dsr);
								cout << ds.toCSV();
							}
							if(ds_name == "SR GR ADS"){
								SR_GR_ADS ds(temp_dsr);
								cout << ds.toCSV();
							}
							if(ds_name == "CHIRP PARAMS ADS"){
								CHIRP_PARAMS_ADS ds(temp_dsr);
								cout << ds.toCSV();
							}
							/*
							for(unsigned int c=0; c< dsr_size;++c){
								cout << (int)((unsigned char*)temp_dsr)[c] << " ";
							}
							*/
							cout <<"\n";
						}
					}
				}
			}
			else{
				/*string temp_s((const char*)temp_r);
				erase_all(temp_s,"\"") ;
				replace_all(temp_s,"=", ";");
				replace_all(temp_s,"\n\n", "\n");
				cout << temp_s << endl;*/
			}
			
			offset+=l;
			
		}
		filestr.close();		
	}	
    catch (std::exception & e){
        // catch any errors that might have occured and print their reason
        std::cout << e.what() << std::endl;
        return 1;
    }
    
return 0;
}