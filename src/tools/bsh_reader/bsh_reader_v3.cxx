/************************************************************************/

/************************************************************************/
 
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

//date and time 
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/lexical_cast.hpp"


using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

void intarray2floatarray(int* iarr, float* farr, long n, float fac){

	for(long i=0; i<n;++i){
		farr[i] = iarr[i]*fac;
	}
}
void umw_ind_ko (char* gg, int mi, int ni, float & br, float & xla){

	int iphig,
		iphim,
		iphis,
		idphig,
		idphim,
		idphis,
		ilamg,
		ilamm,
		ilams,
		idlamg,
		idlamm,
		idlams;
	
	if(*gg == 'n' && gg[1] == 'o'){
		iphig=65;
		iphim=51;
		iphis=0;
		idphig=0;
		idphim=6;
		idphis=0;
		ilamg=-4;
		ilamm=-5;
		ilams=0;
		idlamg=0;
		idlamm=10;
		idlams=0;
	}
	else{
		iphig=56;
		iphim=23;
		iphis=30;
		idphig=0;
		idphim=1;
		idphis=0; 
		ilamg=6;
		ilamm=10;
		ilams=50;
		idlamg=0;
		idlamm=1 ;
		idlams=40;
	}
	
	float	phi  = float(iphig)+float(iphim)/60.0+float(iphis)/3600.0,
			dphi = float(idphig)+float(idphim)/60.0+float(idphis)/3600.0,
			lam  = float(ilamg)+float(ilamm)/60.0+float(ilams)/3600.0,
			dlam = float(idlamg)+float(idlamm)/60.0+float(idlams)/3600.0;

	br=phi-(mi*dphi);
	xla=lam+(ni*dlam);
}

//Main method (program)
int main(int argc, char ** argv)
{
	if(argc != 3){
        cerr	<< "This program tries to extract the information out of a BSH current file.\n"
				<< "Usage: " << argv[0] << "    noku_tiefen_komp_a_v3    current_filename\n";        
        return 1;
    }
    
    try{		
		string filename(argv[1]);
		
		int length;
		
		ifstream filestr;
		filestr.open (filename.c_str(), ios::binary );
		
		if (!filestr.is_open()){
			cerr << "Unable to open file: " << filename << "\n";
			return 1; 
		}
		
		//we need to offset the data by +1,+1 step in x and y for each model!!!!!!
		int offset_geo_x = 1;
		int offset_geo_y = 1;
		
		// get length of file:
		filestr.seekg (0, ios::end);
		length = filestr.tellg();
		filestr.seekg (0, ios::beg);
		
		const unsigned long	mno=174, nno=207, kno=9, iwno2= 8464+1,iwno3=36703+1, 
							mku=191, nku=312, kku=5, iwku2=27031+1,iwku3=72636+1,
							it= 312, jt= 191,
							its=110, jts=150;
		int m1[mno][nno][kno], m3[mku][nku][kku];
		
		float	hzno[iwno3],
				hzku[iwku3];
		 
		float	wuno[iwno2], wvno[iwno2],
				wuku[iwku2], wvku[iwku2];
		
		int iwuno[iwno2], iwvno[iwno2],iwuku[iwku2], iwvku[iwku2];
		
		float	zno[iwno2], uno[iwno3], vno[iwno3], wno[iwno3],
				zku[iwku2], uku[iwku3], vku[iwku3], wku[iwku3];
		 
		int iznoc[iwno2], iunoc[iwno3], ivnoc[iwno3], iwnoc[iwno3],
			izkuc[iwku2], iukuc[iwku3], ivkuc[iwku3], iwkuc[iwku3];
		/*
		float	tno[iwno3], sno[iwno3], tku[iwku3], sku[iwku3];
		 
		int sno_i4[iwno3], tno_i4[iwno3],
			sku_i4[iwku3], tku_i4[iwku3];
		*/
		float	tinox[mno][nno],
				tikux[mku][nku];
		
		 
		
		 
		char	 ctim[19], rdat[11], datum[10], starta[11], uzeit[5],
				gitter[2],
				dummy[8];
		
		long offset =0;
		 
		//read in m1
		for(int k=0; k< kno; ++k){
			for(int n=0; n< nno; ++n){	
				for(int m=0; m< mno; ++m){
					//jump over carriage ret + line feed
					char tmp[6];  
					filestr.seekg (offset, ios::beg);
					filestr.read ((char*)tmp,6);
					if (*tmp == 0x0D)
						offset+=2;
					
					filestr.seekg (offset, ios::beg);
					filestr.read ((char*)tmp,6);
					
					m1[m][n][k] = atoi(tmp);
					//cerr << "m1[" << m << "]["<< n <<"]["<< k <<"]:\t "<< m1[m][n][k] << "\n";
					
					
					offset+=6;
				}
			}	
		}
		
		//read in hzno
		for(int i=0; i< iwno3; ++i, offset+=8){
			
			//jump over carriage ret + line feed
			char tmp[8];  
			filestr.seekg (offset, ios::beg);
			filestr.read ((char*)tmp,8);
			if (*tmp == 0x0D)
				offset+=2;
			  
			filestr.seekg (offset, ios::beg);
			filestr.read ((char*)tmp,8);
			
			hzno[i] = atof(tmp);
			//cerr << "hzno[" << i <<"]:\t "<< hzno[i] << "\n";
		}
		
		
		//read in m3
		for(int k=0; k< kku; ++k){
			for(int n=0; n< nku; ++n){
				for(int m=0; m< mku; ++m){
						//jump over carriage ret + line feed
						char tmp[6];  
						filestr.seekg (offset, ios::beg);
						filestr.read ((char*)tmp,6);
						if (*tmp == 0x0D)
							offset+=2;
						
					filestr.seekg (offset, ios::beg);
					filestr.read ((char*)tmp,6);
					
					m3[m][n][k] = atoi(tmp);
					//cerr << "m3[" << m << "]["<< n <<"]["<< k <<"]:\t "<< m3[m][n][k] << "\n";
										
					offset+=6;
				}
			}	
		}
		
		//read in hzku
		for(int i=0; i< iwku3; ++i, offset+=8){
			
			//jump over carriage ret + line feed
			char tmp[8];  
			filestr.seekg (offset, ios::beg);
			filestr.read ((char*)tmp,8);
			if (*tmp == 0x0D)
				offset+=2;
			
			filestr.seekg (offset, ios::beg);
			filestr.read ((char*)tmp,8);
			
			hzku[i] = atof(tmp);
			//cerr << "hzku[" << i <<"]:\t "<< hzku[i] << "\n";
		}
		
		//close depth file
		filestr.close();
		
		//fill tinox
		for(int i=0; i<mno; ++i){
			for(int j=0; j<nno; ++j){
				double tiefe=0;
				for(int k=0; k<kno; ++k){
					if(m1[i][j][k] > 0){
						tiefe=tiefe+hzno[m1[i][j][k]];
					}
				}
				tinox[i][j]=tiefe;
				//cerr << "tinox[" << i <<"][" << j << "]:\t "<< tinox[i][j]<< "\n";
			}
		}
		
		//fill tikux
		for(int i=0; i<mku; ++i){
			for(int j=0; j<nku; ++j){
				double tiefe=0;
				for(int k=0; k<kku; ++k){
					if(m3[i][j][k] > 0){
						tiefe=tiefe+hzku[m3[i][j][k]];
					}
				}
				tikux[i][j]=tiefe;
				//cerr << "tikux[" << i <<"][" << j << "]:\t "<< tikux[i][j]<< "\n";
			}
		}
		
		
		//#############################################################################
		//## Open current file
		
		filename = argv[2];
		
		filestr.open (filename.c_str(), ios::binary );
		
		if (!filestr.is_open()){
			cerr << "Unable to open file: " << filename << "\n";
			return 1; 
		}
		
		char buf[255];
		
		filestr.getline(buf, 255); 
		
		//check for valid file format
		if(*buf != 's'){
			cerr << "file: " << filename << " is of wrong format\n";
			return 1;
		}
		char aufdat[6];
		memcpy(aufdat, buf+1, 6);
		
		//read in julia
		int		julia = atoi(aufdat),
					y = int(julia/10000);
		
		date	century(1900,Jan,1);
		ptime	base_year = ptime(century + years(y));
		
		offset = 44;		
		
		ofstream filestr47, filestr472,
		filestr48, filestr482;
		
		filestr48	<< "no-grid: level     1 -- 0-8       meter\n" 
					<< "                   2 -- 8-16      meter\n"
					<< "                   3 -- 16-24     meter\n"
					<< "                   4 -- 24-50     meter\n"
					<< "                   5 -- 50-75     meter\n"
					<< "                   6 -- 75-100    meter\n"
					<< "                   7 -- 100-150   meter\n"
					<< "                   8 -- 150-200   meter\n"
					<< "                   9 -- 200-1150  meter\n";
		
		filestr482	<< "ku-grid: level     1 -- 0-8       meter\n"
					<< "                   2 -- 8-16      meter\n"
					<< "                   3 -- 16-24     meter\n"
					<< "                   4 -- 24-50     meter\n"
					<< "                   5 -- 50-75     meter\n";
		
		for(int ixy=0; ixy<96; ++ixy){
			
			filestr.getline(buf, 255); 
			
			float	h = atof(buf),
					m = (h-int(h))*60;
			
			ptime temp_date = time_from_string(string(buf).substr(0,19));
			cerr << to_iso_string(temp_date).substr(0,15) << "\n";
			
			long lines=0;
			
			//while(atof(buf) !=4680.5){
			//	filestr.getline(buf, 255);
			//	lines++;
			//}
			
			//std::cerr << "lines: " << lines << "offset: " << filestr.tellg() <<" val: " << buf << "\n";
			//return 0;
			
			//read(61,'(50i5)')wuno_i4,wvno_i4
			
			long data_left;
			
			//How many blocks do we have for iw(u,v)no and iw(u,v)ku?
			long	uno_c=iwno2, vno_c=iwno2,
					uku_c=iwku2, vku_c=iwku2;
			char* p_t;
			char val_t[5];
			
			while(vku_c > 0){
				filestr.getline(buf, 255); 
				data_left-=50;
				p_t = buf;
				while (uno_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					iwuno[iwno2-uno_c] = atoi(val_t);
					//cerr << "wuno_i4["<< iwno2-u_c << "]: " << wuno_i4[iwno2-u_c] << "\n";
					p_t +=5;
					uno_c--;
				}
				while (uno_c == 0 && vno_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					iwvno[iwno2-vno_c] = atoi(val_t);
					//cerr << "wvno_i4["<< iwno2-v_c << "]: " << wvno_i4[iwno2-v_c] << "\n";
					p_t +=5;
					vno_c--;
				}
				while (vno_c == 0 && uku_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					iwuku[iwku2-uku_c] = atoi(val_t);
					//cerr << "wuno_i4["<< iwno2-u_c << "]: " << wuno_i4[iwno2-u_c] << "\n";
					p_t +=5;
					uku_c--;
				}
				while (uku_c == 0 && vku_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					iwvku[iwku2-vku_c] = atoi(val_t);
					//cerr << "wvno_i4["<< iwno2-v_c << "]: " << wvno_i4[iwno2-v_c] << "\n";
					p_t +=5;
					vku_c--;
				}
				lines++;
			}
			intarray2floatarray(iwuno, wuno, iwno2,1.0/1000.0);
			intarray2floatarray(iwvno, wvno, iwno2,1.0/1000.0);
			intarray2floatarray(iwuku, wuku, iwku2,1.0/1000.0);
			intarray2floatarray(iwvku, wvku, iwku2,1.0/1000.0);
			
			//How many blocks do we have for i(z,u,v,w)noc?
			long	zno_c=iwno2,
					wno_c=iwno3;
			uno_c=iwno3;
			vno_c=iwno3;
			
			while(wno_c > 0){
				filestr.getline(buf, 255); 
				data_left-=50;
				p_t = buf;
				while (zno_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					iznoc[iwno2-zno_c] = atoi(val_t);
					//cerr << "zno_i4["<< iwno2-z_c << "]: " << zno_i4[iwno2-z_c] << "\n";
					p_t +=5;
					zno_c--;
				}
				while (zno_c == 0 && uno_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					iunoc[iwno3-uno_c] = atoi(val_t);
					//cerr << "uno_i4["<< iwno3-u_c << "]: " << uno_i4[iwno3-u_c] << "\n";
					p_t +=5;
					uno_c--;
				}
				while (uno_c == 0 && vno_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					ivnoc[iwno3-vno_c] = atoi(val_t);					
					//cerr << "vno_i4["<< iwno3-v_c << "]: " << vno_i4[iwno3-v_c] << "\n";
					p_t +=5;
					vno_c--;
				}
				while (vno_c == 0 && wno_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					iwnoc[iwno3-wno_c] = atoi(val_t);
					//cerr << "wno_i4["<< iwno3-w_c << "]: " << wno_i4[iwno3-w_c] << "\n";
					p_t +=5;
					wno_c--;
				}
				lines++;
			}
			intarray2floatarray(iunoc, uno, iwno3,1.0/1000.0);
			intarray2floatarray(ivnoc, vno, iwno3,1.0/1000.0);
			intarray2floatarray(iwnoc, wno, iwno3,1.0/100000.0);
			intarray2floatarray(iznoc, zno, iwno2,1.0/1000.0);	
			
			
			//How many blocks do we have for i(z,u,v,w)kuc?
			long zku_c=iwku2,
				 wku_c=iwku3;
			uku_c=iwku3;
			vku_c=iwku3;
			
			while(wku_c > 0){
				filestr.getline(buf, 255); 
				data_left-=50;
				p_t = buf;
				while (zku_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					izkuc[iwku2-zku_c] = atoi(val_t);
					//cerr << "zku_i4["<< iwku2-z_c << "]: " << zku_i4[iwku2-z_c] << "\n";
					p_t +=5;
					zku_c--;
				}
				while (zku_c == 0 && uku_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					iukuc[iwku3-uku_c] = atoi(val_t);
					//cerr << "uku_i4["<< iwku3-u_c << "]: " << uku_i4[iwku3-u_c] << "\n";
					p_t +=5;
					uku_c--;
				}
				while (uku_c == 0 && vku_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					ivkuc[iwku3-vku_c] = atoi(val_t);
					//cerr << "vku_i4["<< iwku3-v_c << "]: " << vku_i4[iwku3-v_c] << "\n";
					p_t +=5;
					vku_c--;
				}
				while (vku_c == 0 && wku_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					iwkuc[iwku3-wku_c] = atoi(val_t);
					//cerr << "wku_i4["<< iwku3-w_c << "]: " << wku_i4[iwku3-w_c] << "\n"; 
					p_t +=5;
					wku_c--;
				}
				lines++;
			}					
			intarray2floatarray(iukuc, uku, iwku3,1.0/1000.0);
			intarray2floatarray(ivkuc, vku, iwku3,1.0/1000.0);
			intarray2floatarray(iwkuc, wku, iwku3,1.0/100000.0);
			intarray2floatarray(izkuc, zku, iwku2,1.0/1000.0);
			
			//NO-Gitter
			char gitter[3];
			gitter[0]='n';	gitter[1]='o';	gitter[2]=0;
			int	ixm1=0,
				ixn1=0,
				ixm2=mno,
				ixn2=nno;
			
			float	min_la, min_br, max_la, max_br;
			umw_ind_ko(gitter, offset_geo_y,			offset_geo_x, max_br, min_la);
			umw_ind_ko(gitter, ixm2+offset_geo_y,  ixn2+offset_geo_x, min_br, max_la);
			
			stringstream gitter_serialisation;
			gitter_serialisation	<< ";global_left: " << min_la << "\n"
			<< ";global_right: " << max_la << "\n"
			<< ";global_top: " << max_br << "\n"
			<< ";global_bottom: " << min_br << "\n"
			<< ";width: " << ixn2 << "\n"
			<< ";height: " << ixm2 <<"\n";
			
			string out_filename;
			out_filename ="data_zet_no_";
			out_filename +=to_iso_string(temp_date).substr(0,15);
			out_filename +=".csv";
			
			
			filestr47.open (out_filename.c_str());
			filestr47	<< ";Serialization of: RSWeightedPointFeatureList2D\n"
						<< ";time: "<< to_simple_string(temp_date) << "\n"
						<< ";name: bsh-zeta @grid no and time "<< to_simple_string(temp_date) << "\n"
						<< gitter_serialisation.str()
						<< "\n"
						<< ";longitude, latitude, zeta in meter\n"; 
			
			for(int i=ixm1; i<ixm2; ++i){
				for(int j=ixn1; j<ixn2; ++j){
					
					
					double	zeta=0.0,
							wind=0.0;
					
					if(m1[i][j][0] > 0){
						
						float br, xla;
						umw_ind_ko (gitter,i,j,br,xla);
						
						float zeta=zno[m1[i][j][0]],
							 wass=tinox[i][j]+zeta;
						
						if(wass < 0.1)	zeta=0.0;
						
						//write(47,'(f10.6,f10.6,f10.6,f7.2)')xla,br,zeta
						filestr47 << j-1 << ", " << i-1 << ", " << zeta << "\n";
					}
				}
			}
			filestr47.close();
			
			for(int ischicht=0; ischicht<10; ++ischicht){
				
				out_filename ="data_cur_no_";
				out_filename +=to_iso_string(temp_date).substr(0,15);
				out_filename += "_level" + lexical_cast<string>(ischicht+1);
				out_filename +=".csv";
				filestr48.open (out_filename.c_str());
				filestr48	<< ";Serialization of: RSDenseVectorfield2D\n"
							<< ";time: "<< to_simple_string(temp_date) << "\n"
							<< ";name: bsh-vf @grid  " << gitter << " and level " << ischicht+1 << " and time "<< to_simple_string(temp_date) << "\n"
							<< gitter_serialisation.str()
							<< ";scale: 11.111111\n"		//u,v in 1 m/15min -> 100cm/900s = 0.111111 * 100 = 11.11111
							<< "\n"							//                                            (weil Angaben in m, nicht in cm)
							<< ";longitude, latitude, u in meter (p. 15 min), v in meter (p. 15 min)\n";
					
				for(int i=1; i<ixm2; ++i){
					for(int j=1; j<ixn2; ++j){
						float br, xla;
						if( m1[i][j][ischicht] != 0){
							umw_ind_ko (gitter,i,j,br,xla);
						
							float	u   = (uno[m1[i][j-1][ischicht]] + uno[m1[i][j][ischicht]])/2.0,
									v   = -(vno[m1[i-1][j][ischicht]] + vno[m1[i][j][ischicht]])/2.0;
									//vel = sqrt(u*u + v*v),
									//dir = 90-atan2(u,v)/M_PI*180.0;
							
							//if (dir<0.0) dir+=360.0;
							filestr48	<< j-1 << ", " << i-1 <<  ", " << u << ", " << v <<"\n";
						}
					}
				}
				filestr48.close();
			}
						
			//ku-Gitter
			gitter[0]='k'; gitter[1]='u';
			ixm1=0;
			ixn1=0;
			ixm2=mku;
			ixn2=nku;
			
			umw_ind_ko(gitter, offset_geo_y,			offset_geo_x, max_br, min_la);
			umw_ind_ko(gitter, ixm2+offset_geo_y,  ixn2+offset_geo_x, min_br, max_la);
			
			gitter_serialisation.clear();
			gitter_serialisation	<< ";global_left: " << min_la << "\n"
									<< ";global_right: " << max_la << "\n"
									<< ";global_top: " << max_br << "\n"
									<< ";global_bottom: " << min_br << "\n"
									<< ";width: " << ixn2 << "\n"
									<< ";height: " << ixm2 <<"\n";
			
			out_filename ="data_zet_ku_";
			out_filename +=to_iso_string(temp_date).substr(0,15);
			out_filename +=".csv";
			
			filestr472.open(out_filename.c_str());
			filestr472	<< ";Serialization of: RSWeightedPointFeatureList2D\n"
						<< ";time: "<< to_simple_string(temp_date) << "\n"
						<< ";name: bsh-zeta @grid ku and time "<< to_simple_string(temp_date) << "\n"
						<< gitter_serialisation.str()
						<< "\n"
						<< "longitude, latitude, zeta in meter\n"; 
			
			for(int i=ixm1; i<ixm2; ++i){
				for(int j=ixn1; j<ixn2; ++j){
					
					
					double	zeta=0.0,
					wind=0.0;
					
					if(m3[i][j][0] > 0){
						
						float br, xla;
						umw_ind_ko (gitter,i,j,br,xla);
						
						float zeta=zku[m3[i][j][0]],
						wass=tikux[i][j]+zeta;
						
						if(wass < 0.1)	zeta=0.0;
						
						//write(47,'(f10.6,f10.6,f10.6,f7.2)')xla,br,zeta
						filestr472 << j-1 << ", " << i-1 << ", " << zeta << "\n";
					}
				}
			}
			filestr472.close();
			
			for(int ischicht=0; ischicht<5; ++ischicht){
				
				out_filename ="data_cur_ku_";
				out_filename +=to_iso_string(temp_date).substr(0,15);
				out_filename += "_level" + lexical_cast<string>(ischicht+1);
				out_filename +=".csv";
				
				filestr482.open(out_filename.c_str());
				filestr482	<< ";Serialization of: RSDenseVectorfield2D\n"
							<< ";time: "<< to_simple_string(temp_date) << "\n"
							<< ";name: bsh-vf @grid " << gitter << " and level " << ischicht+1 << " and time "<< to_simple_string(temp_date) << "\n"
							<< gitter_serialisation.str()
							<< ";scale: 11.111111\n"		//u,v in 1 m/15min -> 100cm/900s = 0.111111 * 100 = 11.11111
							<< "\n"							//                                            (weil Angaben in m, nicht in cm)
							<< ";longitude, latitude, u in meter (p. 15 min), v in meter (p. 15 min)\n";
				
				for(int i=1; i<ixm2; ++i){
					for(int j=1; j<ixn2; ++j){
						float br, xla;
						umw_ind_ko (gitter,i,j,br,xla);
						
						float	u   = (uku[m3[i][j-1][ischicht]] + uku[m3[i][j][ischicht]])/2.0,
								v   = -(vku[m3[i-1][j][ischicht]] + vku[m3[i][j][ischicht]])/2.0;
								//vel = sqrt(u*u + v*v),
								//dir = 90-atan2(u,v)/M_PI*180.0;
						
						//if (dir<0.0) dir+=360.0;
						filestr482	<< j-1 << ", " << i-1 <<  ", " << u << ", " << v <<"\n";
						
					}
				}
				filestr482.close();
			}
		}	
	}
	catch (std::exception & e){
        // catch any errors that might have occured and print their reason
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
return 0;
}