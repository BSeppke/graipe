/************************************************************************/

/************************************************************************/
 
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "vigra/impex.hxx"
#include "vigra/stdimage.hxx"

using namespace std;

bool file_exists( string filename){
	ifstream filestr;
	filestr.open (filename.c_str(), ios::in );
	
	bool res = filestr.is_open();
	
	filestr.close();
	
	return res;
}

#include <sstream>

int StringToInt(std::string stringValue)
{
    std::stringstream ssStream(stringValue);
    int iReturn;
    ssStream >> iReturn;
	
    return iReturn;
}

std::string IntToString(int iValue)
{
    std::stringstream ssStream;
    ssStream << iValue;
    return ssStream.str();
} 

//Main method (program)
int main(int argc, char ** argv)
{
    if(argc != 3){
        cout	<< "This program tries to extract the image out of a SAR header file.\n"
				<< "Usage: " << argv[0] << " filename output_filename\n";        
        return 1;
    }
    
    try{		
		const string filename(argv[1]);
		const string out_filename(argv[2]);
		
		string envi_datafilename="";
		if(filename.substr(filename.length()-4, 4)==".hdr"){
			envi_datafilename=filename.substr(0,filename.length()-4);
			std::cout << "found ENVI Format:\n"
						<< "Metafile: " << envi_datafilename+ ".hdr" << "\n"
						<< "Image/Datafile: " << envi_datafilename << "\n";
		}
		else if(file_exists(filename + ".hdr")){
			envi_datafilename=filename;
			std::cout << "found ENVI Format:\n"
			<< "Metafile: " << envi_datafilename+ ".hdr" << "\n"
			<< "Image/Datafile: " << envi_datafilename << "\n";
		}
		
		//We've found an ENVI-file -> get size, open and save it!
		if(envi_datafilename!=""){
			
			int height=0, width=0, bands=0;
			string interleave="";
			string line;
			ifstream ifs((envi_datafilename + ".hdr").c_str());
			while (std::getline(ifs, line)) {
				boost::erase_all(line, " ");
				//std::cout << line << std::endl << std::flush;
				
				if (line.size() > 7 && line.substr(0,7) == "samples"){
					std::cout << "found samples: " << line.substr(8) << "\n";
					width = StringToInt(line.substr(8));
				}
				else if (line.size() > 5 && line.substr(0,5) == "lines"){
					std::cout << "found lines: " << line.substr(6) << "\n";
					height = StringToInt(line.substr(6));
				}
				else if (line.size() > 5 && line.substr(0,5) == "bands"){
					std::cout << "found bands: " << line.substr(6) << "\n";
					bands = StringToInt(line.substr(6));
				}	
				else if (line.size() > 10 && line.substr(0,10) == "interleave"){
					std::cout << "found interleave-mode: " << line.substr(11) << "\n";
					interleave = line.substr(11);
				}
			}
			ifs.close();
			
			if(width!=0 && height !=0 && bands==1){
				std::cerr << "Now trying to load image of size (" << width << "," << height <<")\n";
				
				int data_length;
				
				ifstream data_filestr;
				data_filestr.open (envi_datafilename.c_str(), ios::binary );
				
				if (!data_filestr.is_open()){
					std::cout << "Unable to open file: " << envi_datafilename << "\n";
					return 1; 
				}
				// get length of file:
				data_filestr.seekg (0, ios::end);
				data_length = data_filestr.tellg();
				int pixel_size = data_length/(width*height);
				//std::cerr << "Determined pixel_size: " << pixel_size << " bytes\n";
                vigra::ImageExportInfo exportInfo(out_filename.c_str());
                exportInfo.setCompression("DEFLATE");
                
				if(pixel_size== 1){
					std::cerr << "Loading & exporting 8bit-image \n";
					vigra::BasicImage<unsigned char> img_out(width,height);
					data_filestr.seekg (0, ios::beg);
					data_filestr.read ((char*)img_out.begin(),data_length);
					exportInfo.setPixelType("UINT8");
                    vigra::exportImage(srcImageRange(img_out), exportInfo);
				}
				else if(pixel_size == 2){
					std::cerr << "Loading & exporting 16bit-image \n";
					vigra::BasicImage<unsigned short> img_out(width,height);
					data_filestr.seekg (0, ios::beg);
					data_filestr.read ((char*)img_out.begin(),data_length);
					exportInfo.setPixelType("INT16");
                    vigra::exportImage(srcImageRange(img_out), exportInfo);
				}
				else if(pixel_size == 4){
					std::cerr << "Loading & exporting 32bit-image \n";
					vigra::BasicImage<unsigned int> img_out(width,height);
					data_filestr.seekg (0, ios::beg);
					data_filestr.read ((char*)img_out.begin(),data_length);
					exportInfo.setPixelType("INT32");
                    vigra::exportImage(srcImageRange(img_out), exportInfo);
                }
				data_filestr.close();
			}
			else{
				std::cout << "Sorry, but currently only single-band ENVI-Files are supported by the image extractor!\n";
			}
		}
		else{
			std::cout << "Sorry, but currently only ENVI-Files are supported by the image extractor!\n";
		}
	}
    catch (std::exception & e){
        // catch any errors that might have occured and print their reason
        std::cout << e.what() << std::endl;
        return 1;
    }
    
return 0;
}