#include <iostream>  				//for console input-output
#include <stdio.h>
#include <string.h>					//for string comparison

#include "vigra/impex.hxx"			//import-export image facilities

#include "vigra/multi_convolution.hxx"	//gaussian gradient + gaussian smooth
#include "vigra/convolution.hxx"	//gaussian gradient + gaussian smooth
#include "vigra/resizeimage.hxx"
#include "vigra/distancetransform.hxx"

#include "tinyarrow.hxx"			//Representation & drawing of arrows	
#include "motiontarget.hxx"			//Repr. of possible feature targets
#include "vectorfieldpainter.hxx"	//Used to print a vectorfield to an image
#include "vectorsmoothing.hxx"		//smoothing operation for vectorfields
#include "correlation.hxx"			//fast-ncc
#include "monotony.hxx"				//the monotony-operator
#include "opticalflow.hxx"				//optical flow approaches
#include "registration.hxx"				//optical flow approaches
#include "sift.hxx"				//optical flow approaches

#include <fstream.h>

using namespace vigra; 
using namespace std; 

int splitString(const string& input, 
       const string& delimiter, vector<string>& results, 
       bool includeEmpties)
{
    int iPos = 0;
    int newPos = -1;
    int sizeS2 = (int)delimiter.size();
    int isize = (int)input.size();

    if( 
        ( isize == 0 )
        ||
        ( sizeS2 == 0 )
    )
    {
        return 0;
    }

    vector<int> positions;

    newPos = input.find (delimiter, 0);

    if( newPos < 0 )
    { 
        return 0; 
    }

    int numFound = 0;

    while( newPos >= iPos )
    {
        numFound++;
        positions.push_back(newPos);
        iPos = newPos;
        newPos = input.find (delimiter, iPos+sizeS2);
    }

    if( numFound == 0 )
    {
        return 0;
    }

    for( int i=0; i <= (int)positions.size(); ++i )
    {
        string s("");
        if( i == 0 ) 
        { 
            s = input.substr( i, positions[i] ); 
        }
        int offset = positions[i-1] + sizeS2;
        if( offset < isize )
        {
            if( i == positions.size() )
            {
                s = input.substr(offset);
            }
            else if( i > 0 )
            {
                s = input.substr( positions[i-1] + sizeS2, 
                      positions[i] - positions[i-1] - sizeS2 );
            }
        }
        if( includeEmpties || ( s.size() > 0 ) )
        {
            results.push_back(s);
        }
    }
    return numFound;
}

int main(int argc, char ** argv)
{    
    /*std::string output_path("C:\\Users\\Benjamin\\Desktop\\vectorfields\\"),
                input_path("C:\\Users\\Benjamin\\Desktop\\satellitenbilder\\California_Currents\\");*/
    std::string output_path("/Users/seppke/Desktop/vectorfields/"),
                input_path("/Users/seppke/Desktop/");
    std::string start_name("start.tif"),
                reg_name("reg.tif");
	
	/*std::string start_name("ice_cut2003_04_07_0715.tif"),
                reg_name("ice_cut2003_04_08_0645.tif");
  */

    int roi_x    = 3000,
        roi_y    = 2000,
        work_w   = 1000,
        work_h   = 1000,
        mask_w   = 61,
        mask_h   = 61,
        search_w = 195,
        search_h = 195,
        n_candidates = 10;
    
    bool resize_image=true;
    
    float gaussian_sigma = 2.5,
		  display_thresh = 0.6,
		  smoothing_radius = 60; 
	
	
	
	vigra::TinyVector<double,2> ul1,ul2,lr1,lr2;
	
	
    vigra::ImageImportInfo imginfo1("/Users/seppke/Desktop/SeaIceDrift2003/SAR-data/2003-04-07_07:15_RS1-SCW/dat_01.001.polstereo.raw.png"),
                           imginfo2("/Users/seppke/Desktop/SeaIceDrift2003/SAR-data/2003-04-07_20:00_ASAR-WSM/ASA_WSM_1PPIPA20030407_200033_000000672015_00200_05765_0326.N1.polstereo.raw.png");
	/*vigra::ImageImportInfo imginfo1("D:/development/RemoteSensing/images/dat_01.001.polstereo.raw.png"),
                           imginfo2("D:/development/RemoteSensing/images/ASA_WSM_1PPIPA20030407_200033_000000672015_00200_05765_0326.N1.polstereo.raw.png");
	*/
	
	fstream file_op1("/Users/seppke/Desktop/SeaIceDrift2003/SAR-data/2003-04-07_07:15_RS1-SCW/dat_01.001.polstereo+lonlat.corners.txt",ios::in),
	        file_op2("/Users/seppke/Desktop/SeaIceDrift2003/SAR-data/2003-04-07_20:00_ASAR-WSM/ASA_WSM_1PPIPA20030407_200033_000000672015_00200_05765_0326.N1.polstereo+lonlat.corners.txt",ios::in);
	vigra::FImage img1(imginfo1.size()),img2(imginfo2.size()), reg(imginfo2.size());
	
	importImage(imginfo1,destImage(img1));
	importImage(imginfo2,destImage(img2));
	
	
	char str[2000];
	
			vector<string> coords, ul_coords, lr_coords;
	int line=0;
	while(!file_op1.eof()) {
		file_op1.getline(str,2000);

		if(++line == 9){
			string lineStr(str);
			splitString(lineStr, string(","), coords,false);
			
			splitString(coords[0], string("/"), ul_coords,false);
			splitString(coords[3], string("/"), lr_coords,false);
			ul1[0] = atof(ul_coords[0].c_str());
			ul1[1] = atof(ul_coords[1].c_str());
			lr1[0] = atof(lr_coords[0].c_str());
			lr1[1] = atof(lr_coords[1].c_str());
			cout << "1: upperleft: "<< ul1 << " lowerright: " << lr1 <<endl;
		}
	}         
	file_op1.close();
    cout <<endl;
	
	line=0;	
	while(!file_op2.eof()) {
		
		file_op2.getline(str,2000);
        
		if(++line == 9){
			string lineStr(str);
			splitString(lineStr, string(","), coords,false);
			
			splitString(coords[0], string("/"), ul_coords,false);
			splitString(coords[3], string("/"), lr_coords,false);
			ul2[0] = atof(ul_coords[0].c_str());
			ul2[1] = atof(ul_coords[1].c_str());
			lr2[0] = atof(lr_coords[0].c_str());
			lr2[1] = atof(lr_coords[1].c_str());
			
		cout << "2: upperleft: "<< ul2 << " lowerright: " << lr2 <<endl;
		}
	}         
	file_op2.close();
    cout <<endl;
/*
	ul1[0]=485.05160;
	ul1[1]=-408.12620;
		
	ul2[0]=612.99017;
	ul2[1]=-510.10529;
	
	lr1[0]=1164.7514;
	lr1[1]=-1069.3238;
	
	lr2[0]=1134.4623;
	lr2[1]=-1016.3898;
	
*/	
	vigra::ImageImportInfo in1("./test/box.png");
	vigra::FImage input(in1.size()), output(in1.width()*2, in1.height()*2);
	importImage(in1, destImage(input));
	
	extractSIFT(input,output);
	vigra::exportImage(srcImageRange(output), ImageExportInfo("sift.png"));
	
	registerFromGeoReference(img1, img2, reg,  ul1,ul2,  lr1,lr2);
	//combineTwoImages(srcImageRange(img2),srcImage(reg), destImage(reg), std::plus<vigra::BImage::PixelType>());
	vigra::exportImage(srcImageRange(reg), ImageExportInfo("images/reg_test.png"));
	return 0;
	//unittest_box_ofceLK();
	//unittest_box_ofceHS();
	//unittest_box_ofceHS_warp_hierarchy();
	//unittest_tennis_ofceHS_warp_hierarchy();
	/*unittest_tennis_ofceLK();
	unittest_tennis_ofceHS();
	unittest_tennis_ofceNA();
	/*
	unittest_tennis_ofceLK();
	unittest_tennis_ofceHS();
	//unittest_tennis_ofceHS_hierarchy();
	
	unittest_tennis_ofceLK_hierarchy();
	unittest_tennis_ofceHS_hierarchy();
	unittest_tennis_ofceNA_hierarchy();
	/*
	unittest_tennis_ofceLK_warp_hierarchy();
	unittest_tennis_ofceHS_warp_hierarchy();
	unittest_tennis_ofceNA_warp_hierarchy();
	
	return 0;
	*/
	
	
    vigra::ImageImportInfo info_start( (input_path + start_name).c_str()),
                           info_reg(   (input_path + reg_name).c_str());
	
	vigra::FImage full_size(info_start.size()),
                  start_cut(work_w,work_h),
                  reg_cut(work_w,work_h),
                  temp(work_w,work_h);
				  
    //Import and cut first image              
    vigra::importImage(info_start, destImage(full_size));
    if(resize_image)
        resizeImageLinearInterpolation( full_size.upperLeft(), full_size.lowerRight(), full_size.accessor(),
                                        start_cut.upperLeft(), start_cut.lowerRight(), start_cut.accessor());
    else
        copyImage( srcIterRange( full_size.upperLeft() + vigra::Diff2D(roi_x, roi_y),
                                 full_size.upperLeft() + vigra::Diff2D(roi_x+work_w, roi_y+work_h)),
                   destImage(start_cut));
                 
    //Import and cut second Image
    vigra::importImage(info_reg, destImage(full_size));
    if(resize_image)
        resizeImageLinearInterpolation( full_size.upperLeft(), full_size.lowerRight(), full_size.accessor(),
                                        reg_cut.upperLeft(), reg_cut.lowerRight(), reg_cut.accessor());
    else
        copyImage( srcIterRange( full_size.upperLeft() + vigra::Diff2D(roi_x, roi_y),
                                 full_size.upperLeft() + vigra::Diff2D(roi_x+work_w, roi_y+work_h)),
                   destImage(reg_cut));
    
	
	//discMedian(srcImageRange(start_cut), destImage(temp), 5);
    vigra::gaussianSmoothing(srcImageRange(start_cut), destImage(start_cut), gaussian_sigma);
	
	//discMedian(srcImageRange(reg_cut), destImage(temp), 5);
    vigra::gaussianSmoothing(srcImageRange(reg_cut), destImage(reg_cut),gaussian_sigma);
    //save
    vigra::exportImage(srcImageRange(start_cut), vigra::ImageExportInfo( (output_path+ std::string("ice_start_cut.png")).c_str() ));
    vigra::exportImage(srcImageRange(reg_cut), vigra::ImageExportInfo( (output_path+ std::string("ice_reg_cut.png")).c_str() ));
    
    //find features:
    vigra::BImage feature_img(work_w,work_h),
                  arrows(work_w,work_h); 
    vigra::FImage mask(mask_w,mask_h), search_field(search_w,search_h);
    vigra::BasicImage<double> result(search_w,search_h);
    
    std::cout << "Searching Features\n";
    monotony_operator(start_cut, feature_img,1);
    
    vigra::exportImage(srcImageRange(feature_img), vigra::ImageExportInfo( (output_path+ std::string("ice_features.png")).c_str() ));
    
    int counter=0, candidates=0, progress=0;
	
	std::vector<vigra::Diff2D> features;

    for(int y=search_h/2; y<work_h-search_h/2; y++){
        for(int x=search_w/2; x<work_w-search_w/2; x++){
            if(feature_img(x,y)>7 /*&& start_cut(x,y) < 100*/){
                counter++;
				features.push_back(vigra::Diff2D(x,y));
				
            }
        }
    }
	
	std::vector<MotionTarget> targets(n_candidates);
	std::vector<std::vector<MotionTarget> > feature_targets(counter);
	std::vector<MotionTarget> result_vector(counter);
	
    std::cout << "Found "<< counter <<" Features\n";
    counter=0;
    std::cout << "Starting correlation\n";
    for(std::vector<Diff2D>::iterator iter = features.begin(); iter != features.end(); iter++){
        int x = iter->x, y = iter->y;
		
		int upper = std::max(0,(y-search_h/2)),   left = std::max(0,(x-search_w/2)),
			lower = std::min(work_h-1,(y+search_h/2)), right = std::min(work_w-1,(x+search_w/2));

		copyImage( srcIterRange( start_cut.upperLeft() + vigra::Diff2D(x-mask_w/2, y-mask_h/2),
								start_cut.upperLeft() + vigra::Diff2D(x+mask_w/2, y+mask_h/2)),
				   destImage(mask));
				   
		copyImage( srcIterRange( reg_cut.upperLeft() + vigra::Diff2D(left, upper),
								reg_cut.upperLeft() + vigra::Diff2D(right,lower)),
				   destImage(search_field));

		fast_ncc(mask,search_field,result);

		for(int n=0; n<n_candidates; n++){
			
		   int max_x=0, max_y=0;

			for(int r_y=1; r_y<search_h-1; r_y++){
				for(int r_x=1; r_x<search_w-1; r_x++){
					if(		result(r_x,r_y) >result(max_x,max_y) &&  result(r_x,r_y)<=1 
						&&	result(r_x,r_y) > result(r_x-1,r_y-1) && result(r_x,r_y) > result(r_x,r_y-1)  && result(r_x,r_y) > result(r_x+1,r_y-1) 
						&&  result(r_x,r_y) > result(r_x-1,r_y)											 && result(r_x,r_y) > result(r_x+1,r_y) 
						&&  result(r_x,r_y) > result(r_x-1,r_y+1)  && result(r_x,r_y) > result(r_x,r_y+1) && result(r_x,r_y) > result(r_x+1,r_y+1) ){
						max_x=r_x; max_y=r_y;
					}
				}
			}
			//add candidate to vector
			targets[n]=MotionTarget(x+max_x-search_field.width()/2, y+max_y-search_field.height()/2, result(max_x,max_y));
			//delete that maximum
			result(max_x,max_y)=0;
		}

		feature_targets[counter]=targets;

		if(targets[0].getP()>display_thresh){
			++candidates;
			 printf("%4d: (%4d,%4d) -> (%4d,%4d) corr_value: %1.4f\n",
				counter, x, y,
				(int)targets[0].getPos().x,
				(int)targets[0].getPos().y ,
				targets[0].getP() );
			
						  
			TinyArrow<int>(features[counter],targets[0].getPos()).drawToImage(arrows,(unsigned char)(targets[0].getP()*255.0));
		}
		counter++;
		
        if( counter/(float)features.size()*100 > progress+5){
        	progress	= counter/(float)features.size()*100;
        	std::cerr << "|";
        }
    }
	
    std::cout << "\nFeatures overall: " << counter << "\n";
    std::cout << "# of high corr. : " << candidates << "\n";

    vigra::exportImage(srcImageRange(arrows), vigra::ImageExportInfo( (output_path+ std::string("ice_arrows_before.png")).c_str() ));
    arrows= vigra::BImage(work_w,work_h);
    
    //vigra::MultiArray<2,double> result_array(vigra::MultiArray<2,double>::difference_type(counter,3));
    
    char buffer[100];
    /*
    std::cout << "Starting relaxations\n";
    for (int i=10; i<=100; i+=30){
   	 	relaxation(feature_array, feature_candidates_array,result_array,i,30);
    	std::cout << "Drawing arrows\n";
    	drawArrows(feature_array, result_array, arrows);
    	sprintf(buffer,"/Users/seppke/Desktop/arrows_relax_%d_30.gif",i);
    	vigra::exportImage(srcImageRange(arrows), vigra::ImageExportInfo(buffer));
    	arrows= vigra::BImage(work_w,work_h);
    }
    
	*/
	
	std::cout << "Starting smoothings\n";
    for (int i=1; i<=20; i++){
   		float j=0;
   		//for (float j=0.0; j<=1.0; j+=0.3){
	   	 	arrows= vigra::BImage(work_w,work_h);
	   	 	smoothVectorfield(features, feature_targets,result_vector,i+1,smoothing_radius,j);
	    	std::cout << "Drawing arrows\n";
	    	
			VectorfieldPainter<unsigned char> painter(features, result_vector, arrows);
			painter.drawArrows(display_thresh);
			
	    	sprintf(buffer, (output_path+ std::string("arrows_smooth_%d_30_%1.1f.png")).c_str() ,i,j);
	    	vigra::exportImage(srcImageRange(arrows), vigra::ImageExportInfo(buffer));
   		//}
    }   
	
	arrows= vigra::BImage(work_w,work_h);
	std::vector<Diff2D>::iterator			iter		= features.begin();
	std::vector<std::vector<MotionTarget> >::iterator target_iter = feature_targets.begin();
	for(; iter != features.end(); iter++, target_iter++){


		if( (*target_iter)[0].getP()>display_thresh){						  
			TinyArrow<int>(*iter,(*target_iter)[0].getPos()).drawToImage(arrows,(unsigned char)((*target_iter)[0].getP()*255.0));
		}
    }
	vigra::exportImage(srcImageRange(arrows), vigra::ImageExportInfo( (output_path+ std::string("arrows_before2.png")).c_str() ));
    
	std::cout << "Starting candidate election\n";
    for (int i=1; i<=20; i++){
   		float j=0;
   		//for (float j=0.0; j<=1.0; j+=0.3){
	   	 	arrows= vigra::BImage(work_w,work_h);
	   	 	smoothCandidatesVectorfield(features, feature_targets,result_vector,i+1,smoothing_radius,j);
	    	std::cout << "Drawing arrows\n";
	    	
			VectorfieldPainter<unsigned char> painter(features, result_vector, arrows);
			painter.drawArrows(display_thresh);
			
			sprintf(buffer, (output_path+ std::string("arrows_smooth_candidates_%d_30_%1.1f.png")).c_str() ,i,j);
	    	vigra::exportImage(srcImageRange(arrows), vigra::ImageExportInfo(buffer));
   		//}
    }	
	
	vigra::exportImage(srcImageRange(arrows), vigra::ImageExportInfo( (output_path+ std::string("arrows_before3.png")).c_str() ));
    
	std::cout << "Starting candidate election\n";
    for (int i=1; i<=20; i++){
   		float j=0;
   		//for (float j=0.0; j<=1.0; j+=0.3){
	   	 	arrows= vigra::BImage(work_w,work_h);
	   	 	smoothVectorfieldAllCandidates(features, feature_targets,result_vector,i+1,smoothing_radius,j);
	    	std::cout << "Drawing arrows\n";
	    	
			VectorfieldPainter<unsigned char> painter(features, result_vector, arrows);
			painter.drawArrows(display_thresh);
			
			sprintf(buffer, (output_path+ std::string("arrows_smooth_all_candidates_%d_30_%1.1f.png")).c_str() ,i,j);
	    	vigra::exportImage(srcImageRange(arrows), vigra::ImageExportInfo(buffer));
   		//}
    }
    
    //std::cin >>argc;
    return 0;
}
