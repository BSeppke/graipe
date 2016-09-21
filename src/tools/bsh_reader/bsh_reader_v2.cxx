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
		idphim=12;
		idphis=0;
		ilamg=-4;
		ilamm=-5;
		ilams=-24;
		idlamg=0;
		idlamm=20;
		idlams=0;
	}
	else if(gg[0] == 'd' && gg[1] == 'b'){
		iphig=57;
		iphim=45;
		iphis=00;
		idphig=0;
		idphim=6;
		idphis=0; 
		ilamg=5;
		ilamm=24;
		ilams=36;
		idlamg=0;
		idlamm=10;
		idlams=0; 
	}
	else{
		iphig=55;
		iphim=39;
		iphis=30;
		idphig=0;
		idphim=1;
		idphis=0; 
		ilamg=6;
		ilamm=34;
		ilams=10;
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
        cout	<< "This program tries to extract the information out of a BSH current file.\n"
				<< "Usage: " << argv[0] << "    nord_ostsee_tiefen_a_v2    current_filename\n";        
        return 1;
    }
    
    try{		
		string filename(argv[1]);
		
		int length;
		
		ifstream filestr;
		filestr.open (filename.c_str(), ios::binary );
		
		if (!filestr.is_open()){
			cout << "Unable to open file: " << filename << "\n";
			return 1; 
		}
		
		//we need to offset the data by +1,+1 step in x and y for each model!!!!!!
		int offset_geo_x = 1;
		int offset_geo_y = 1;
		
		// get length of file:
		filestr.seekg (0, ios::end);
		length = filestr.tellg();
		filestr.seekg (0, ios::beg);
		
		const unsigned long	mno=88, nno= 105,kno=10,iwno2= 2165+1,iwno3= 9388+1, 
							mdb=45, ndb= 62, kdb=5, iwdb2= 1119+1,iwdb3= 3447+1,
							mku=147,nku=290, kku=4, iwku2=13693+1,iwku3=30265+1;
		
		int m1[mno][nno][kno], m2[mdb][ndb][kdb], m3[mku][nku][kku];
		
		float	hzno[iwno3],
				hzdb[iwdb3],
				hzku[iwku3];
		
		int wuno_i4[iwno2], wvno_i4[iwno2];
		
		float wuno[iwno2], wvno[iwno2];
		
		int uno_i4[iwno3], vno_i4[iwno3], wno_i4[iwno3],
			udb_i4[iwdb3], vdb_i4[iwdb3], wdb_i4[iwdb3],
			uku_i4[iwku3], vku_i4[iwku3], wku_i4[iwku3];
		
		float	uno[iwno3], vno[iwno3], wno[iwno3],
				udb[iwdb3], vdb[iwdb3], wdb[iwdb3],
				uku[iwku3], vku[iwku3], wku[iwku3];
		
		int zno_i4[iwno2],
			zdb_i4[iwdb2],
			zku_i4[iwku2];
		float	zno[iwno2],
				zdb[iwdb2],
				zku[iwku2];
		
		float	tinox[mno][nno],
				tidbx[mdb][ndb],
				tikux[mku][nku];
		
		char	/*aufdat[11],*/ ctim[19], rdat[11], datum[10], starta[11], uzeit[5],
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
					//cout << "m1[" << m << "]["<< n <<"]["<< k <<"]:\t "<< m1[m][n][k] << "\n";
					
					
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
			//cout << "hzno[" << i <<"]:\t "<< hzno[i] << "\n";
		}
		
		
		//read in m2
		for(int k=0; k< kdb; ++k){
			for(int n=0; n< ndb; ++n){
				for(int m=0; m< mdb; ++m){
					//jump over carriage ret + line feed
					char tmp[6];  
					filestr.seekg (offset, ios::beg);
					filestr.read ((char*)tmp,6);
					if (*tmp == 0x0D)
						offset+=2;
					
					filestr.seekg (offset, ios::beg);
					filestr.read ((char*)tmp,6);
					
					m2[m][n][k] = atoi(tmp);
					//cout << "m2[" << m << "]["<< n <<"]["<< k <<"]:\t "<< m2[m][n][k] << "\n";
					
					offset+=6;
				}
			}	
		}
		
		//read in hzdb
		for(int i=0; i< iwdb3; ++i, offset+=8){
			
			//jump over carriage ret + line feed
			char tmp[8];  
			filestr.seekg (offset, ios::beg);
			filestr.read ((char*)tmp,8);
			if (*tmp == 0x0D)
				offset+=2;
			
			filestr.seekg (offset, ios::beg);
			filestr.read ((char*)tmp,8);
			
			hzdb[i] = atof(tmp);
			//cout << "hzdb[" << i <<"]:\t "<< hzdb[i] << "\n";
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
					//cout << "m3[" << m << "]["<< n <<"]["<< k <<"]:\t "<< m3[m][n][k] << "\n";
										
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
			//cout << "hzku[" << i <<"]:\t "<< hzku[i] << "\n";
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
				//cout << "tinox[" << i <<"][" << j << "]:\t "<< tinox[i][j]<< "\n";
			}
		}
		
		//fill tidbx
		for(int i=0; i<mdb; ++i){
			for(int j=0; j<ndb; ++j){
				double tiefe=0;
				for(int k=0; k<kdb; ++k){
					if(m2[i][j][k] > 0){
						tiefe=tiefe+hzdb[m2[i][j][k]];
					}
				}
				tidbx[i][j]=tiefe;
				//cout << "tidbx[" << i <<"][" << j << "]:\t "<< tidbx[i][j]<< "\n";
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
				//cout << "tikux[" << i <<"][" << j << "]:\t "<< tikux[i][j]<< "\n";
			}
		}
		
		
		//#############################################################################
		//## Open current file
		
		filename = argv[2];
		
		filestr.open (filename.c_str(), ios::binary );
		
		if (!filestr.is_open()){
			cout << "Unable to open file: " << filename << "\n";
			return 1; 
		}
		
		char buf[255];
		
		filestr.getline(buf, 255); 
		
		//check for valid file format
		if(*buf != 'V'){
			cout << "file: " << filename << " is of wrong format\n";
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
		
		ofstream filestr47, filestr471, filestr472,
		filestr48, filestr481, filestr482;
		
		filestr48	<< "no-grid: level     1 -- 0-8       meter\n" 
					<< "                   2 -- 8-16      meter\n"
					<< "                   3 -- 16-24     meter\n"
					<< "                   4 -- 24-50     meter\n"
					<< "                   5 -- 50-100    meter\n"
					<< "                   6 -- 100-150   meter\n"
					<< "                   7 -- 150-200   meter\n"
					<< "                   8 -- 250-350   meter\n"
					<< "                   9 -- 350-400   meter\n"
					<< "                  10 -- 400-900   meter\n";
		
		filestr481	<< "db-grid: level     1 -- 0-8       meter\n" 
					<< "                   2 -- 8-16      meter\n"
					<< "                   3 -- 16-24     meter\n"
					<< "                   4 -- 24-50     meter\n"
					<< "                   5 -- 50-100    meter\n";
		
		filestr482	<< "ku-grid: level     1 -- 0-8       meter\n" 
					<< "                   2 -- 8-16      meter\n"
					<< "                   3 -- 16-24     meter\n"
					<< "                   4 -- 24-50     meter\n"
					<< "                   5 -- 50-100    meter\n";
		
		for(int ixy=0; ixy<48; ++ixy){
			
			filestr.getline(buf, 255); 
			
			float	h = atof(buf),
					m = (h-int(h))*60;
			
			ptime temp_date = base_year + hours(h) + minutes(m);
						
			long lines=0;
			/*
			while(atof(buf) !=4680.5){
				filestr.getline(buf, 255);
				lines++;
			}
			
			std::cerr << "lines: " << lines << "offset: " << filestr.tellg() <<" val: " << buf << "\n";
			return 0;
			*/
			//read(61,'(50i5)')wuno_i4,wvno_i4
			
			long data_left;
			
			//How many blocks do we have for w(u,v)no_i4?
			long	u_c=iwno2,
					v_c=iwno2;
			char* p_t;
			char val_t[5];
			
			while(v_c > 0){
				filestr.getline(buf, 255); 
				data_left-=50;
				p_t = buf;
				while (u_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					wuno_i4[iwno2-u_c] = atoi(val_t);
					//cerr << "wuno_i4["<< iwno2-u_c << "]: " << wuno_i4[iwno2-u_c] << "\n";
					p_t +=5;
					u_c--;
				}
				while (u_c == 0 && v_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					wvno_i4[iwno2-v_c] = atoi(val_t);
					//cerr << "wvno_i4["<< iwno2-v_c << "]: " << wvno_i4[iwno2-v_c] << "\n";
					p_t +=5;
					v_c--;
				}
				lines++;
			}
			intarray2floatarray(wuno_i4, wuno, iwno2,1.0/1000.0);
			intarray2floatarray(wvno_i4, wvno, iwno2,1.0/1000.0);
			
			//How many blocks do we have for (z,u,v,w)no_i4?
			long	z_c=iwno2,
					w_c=iwno3;
			u_c=iwno3,
			v_c=iwno3;
			
			while(w_c > 0){
				filestr.getline(buf, 255); 
				data_left-=50;
				p_t = buf;
				while (z_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					zno_i4[iwno2-z_c] = atoi(val_t);
					//cerr << "zno_i4["<< iwno2-z_c << "]: " << zno_i4[iwno2-z_c] << "\n";
					p_t +=5;
					z_c--;
				}
				while (z_c == 0 && u_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					uno_i4[iwno3-u_c] = atoi(val_t);
					//cerr << "uno_i4["<< iwno3-u_c << "]: " << uno_i4[iwno3-u_c] << "\n";
					p_t +=5;
					u_c--;
				}
				while (u_c == 0 && v_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					vno_i4[iwno3-v_c] = atoi(val_t);					
					//cerr << "vno_i4["<< iwno3-v_c << "]: " << vno_i4[iwno3-v_c] << "\n";
					p_t +=5;
					v_c--;
				}
				while (v_c == 0 && w_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					wno_i4[iwno3-w_c] = atoi(val_t);
					//cerr << "wno_i4["<< iwno3-w_c << "]: " << wno_i4[iwno3-w_c] << "\n";
					p_t +=5;
					w_c--;
				}
				lines++;
			}
			intarray2floatarray(uno_i4, uno, iwno3,1.0/1000.0);
			intarray2floatarray(vno_i4, vno, iwno3,1.0/1000.0);
			intarray2floatarray(wno_i4, wno, iwno3,1.0/100000.0);
			intarray2floatarray(zno_i4, zno, iwno2,1.0/1000.0);	
			
			
			//How many blocks do we have for (z,u,v,w)db_i4?
			z_c=iwdb2;
			w_c=iwdb3;
			u_c=iwdb3,
			v_c=iwdb3;
			
			while(w_c > 0){
				filestr.getline(buf, 255); 
				data_left-=50;
				p_t = buf;
				while (z_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					zdb_i4[iwdb2-z_c] = atoi(val_t);
					//cerr << "zdb_i4["<< iwdb2-z_c << "]: " << zdb_i4[iwdb2-z_c] << "\n";
					p_t +=5;
					z_c--;
				}
				while (z_c == 0 && u_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					udb_i4[iwdb3-u_c] = atoi(val_t);
					//cerr << "udb_i4["<< iwdb3-u_c << "]: " << udb_i4[iwdb3-u_c] << "\n";
					p_t +=5;
					u_c--;
				}
				while (u_c == 0 && v_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					vdb_i4[iwdb3-v_c] = atoi(val_t);					
					//cerr << "vdb_i4["<< iwdb3-v_c << "]: " << vdb_i4[iwdb3-v_c] << "\n";
					p_t +=5;
					v_c--;
				}
				while (v_c == 0 && w_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					wdb_i4[iwdb3-w_c] = atoi(val_t);
					//cerr << "wdb_i4["<< iwdb3-w_c << "]: " << wdb_i4[iwdb3-w_c] << "\n";
					p_t +=5;
					w_c--;
				}
				lines++;
			}			
			intarray2floatarray(udb_i4, udb, iwdb3,1.0/1000.0);
			intarray2floatarray(vdb_i4, vdb, iwdb3,1.0/1000.0);
			intarray2floatarray(wdb_i4, wdb, iwdb3,1.0/100000.0);
			intarray2floatarray(zdb_i4, zdb, iwdb2,1.0/1000.0);
			
			//How many blocks do we have for (z,u,v,w)ku_i4?
			z_c=iwku2;
			w_c=iwku3;
			u_c=iwku3,
			v_c=iwku3;
			
			while(w_c > 0){
				filestr.getline(buf, 255); 
				data_left-=50;
				p_t = buf;
				while (z_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					zku_i4[iwku2-z_c] = atoi(val_t);
					//cerr << "zku_i4["<< iwku2-z_c << "]: " << zku_i4[iwku2-z_c] << "\n";
					p_t +=5;
					z_c--;
				}
				while (z_c == 0 && u_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					uku_i4[iwku3-u_c] = atoi(val_t);
					//cerr << "uku_i4["<< iwku3-u_c << "]: " << uku_i4[iwku3-u_c] << "\n";
					p_t +=5;
					u_c--;
				}
				while (u_c == 0 && v_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					vku_i4[iwku3-v_c] = atoi(val_t);
					//cerr << "vku_i4["<< iwku3-v_c << "]: " << vku_i4[iwku3-v_c] << "\n";
					p_t +=5;
					v_c--;
				}
				while (v_c == 0 && w_c > 0 && p_t-buf < 250){
					memcpy(val_t, p_t, 5);
					wku_i4[iwku3-w_c] = atoi(val_t);
					//cerr << "wku_i4["<< iwku3-w_c << "]: " << wku_i4[iwku3-w_c] << "\n"; 
					p_t +=5;
					w_c--;
				}
				lines++;
			}					
			intarray2floatarray(uku_i4, uku, iwku3,1.0/1000.0);
			intarray2floatarray(vku_i4, vku, iwku3,1.0/1000.0);
			intarray2floatarray(wku_i4, wku, iwku3,1.0/100000.0);
			intarray2floatarray(zku_i4, zku, iwku2,1.0/1000.0);
			
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
						
						/*write(47,'(f10.6,f10.6,f10.6,f7.2)')xla,br,zeta*/
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
							<< ";scale: 11.111111\n"   	//u,v in 1 m/15min -> 100cm/900s = 0.111111 * 100 = 11.11111
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
							filestr48	<< j-1 << ", " << i-1 << ", " << u << ", " << v <<"\n";
						}
					}
				}
				filestr48.close();
			}
			
			//db-Gitter
			gitter[0]='d'; gitter[1]='b';
			ixm1=0;
			ixn1=0;
			ixm2=mdb;
			ixn2=ndb;
			
			umw_ind_ko(gitter, offset_geo_y,			offset_geo_x, max_br, min_la);
			umw_ind_ko(gitter, ixm2+offset_geo_y,  ixn2+offset_geo_x, min_br, max_la);
			
			gitter_serialisation.clear();
			gitter_serialisation	<< ";global_left: " << min_la << "\n"
									<< ";global_right: " << max_la << "\n"
									<< ";global_top: " << max_br << "\n"
									<< ";global_bottom: " << min_br << "\n"
									<< ";width: " << ixn2 << "\n"
									<< ";height: " << ixm2 <<"\n";
			
			out_filename ="data_zet_db_";
			out_filename +=to_iso_string(temp_date).substr(0,15);
			out_filename +=".csv";
			
			filestr471.open(out_filename.c_str());
			filestr471	<< ";Serialization of: RSWeightedPointFeatureList2D\n"
						<< ";time: "<< to_simple_string(temp_date) << "\n"
						<< ";name: bsh-zeta @grid db and time "<< to_simple_string(temp_date)  << "\n"
						<< gitter_serialisation.str()
						<< "\n"
						<< "longitude, latitude, zeta in meter\n"; 
			
			for(int i=ixm1; i<ixm2; ++i){
				for(int j=ixn1; j<ixn2; ++j){
					
					
					double	zeta=0.0,
					wind=0.0;
					
					if(m2[i][j][0] > 0){
						
						float br, xla;
						umw_ind_ko (gitter,i,j,br,xla);
						
						float zeta=zdb[m2[i][j][0]],
						wass=tidbx[i][j]+zeta;
						
						if(wass < 0.1)	zeta=0.0;
						
						/*write(47,'(f10.6,f10.6,f10.6,f7.2)')xla,br,zeta*/
						filestr471 << j-1 << ", " << i-1 << ", " << zeta << "\n";
					}
				}
			}
			filestr471.close();
			
			for(int ischicht=0; ischicht<5; ++ischicht){
				out_filename ="data_cur_db_";
				out_filename +=to_iso_string(temp_date).substr(0,15);
				out_filename += "_level" + lexical_cast<string>(ischicht+1);
				out_filename +=".csv";
				filestr481.open(out_filename.c_str());
				filestr481	<< ";Serialization of: RSDenseVectorfield2D\n"
							<< ";time: "<< to_simple_string(temp_date) << "\n"
							<< ";name: bsh-vf @grid " << gitter << " and level " << ischicht+1 << " and time "<< to_simple_string(temp_date)   << "\n"
							<< gitter_serialisation.str()
							<< ";scale: 11.111111\n"		//u,v in 1 m/15min -> 100cm/900s = 0.111111 * 100 = 11.11111
							<< "\n"							//                                            (weil Angaben in m, nicht in cm)
							<< ";longitude, latitude, u in meter (p. 15 min), v in meter (p. 15 min)\n";
				
				for(int i=1; i<ixm2; ++i){
					for(int j=1; j<ixn2; ++j){
						float br, xla;
						umw_ind_ko (gitter,i,j,br,xla);
						
						float	u   = (udb[m2[i][j-1][ischicht]] + udb[m2[i][j][ischicht]])/2.0,
								v   = -(vdb[m2[i-1][j][ischicht]] + vdb[m2[i][j][ischicht]])/2.0;
								//vel = sqrt(u*u + v*v),
								//dir = 90-atan2(u,v)/M_PI*180.0;
						
						//if (dir<0.0) dir+=360.0;
						filestr481	<< j-1 << ", " << i-1 << ", " << u << ", " << v <<"\n";
						
					}
				}	
				filestr481.close();			
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
						
						/*write(47,'(f10.6,f10.6,f10.6,f7.2)')xla,br,zeta*/
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
						filestr482	<< j-1 << ", " << i-1 << ", " << u << ", " << v <<"\n";
						
					}
				}
				filestr482.close();
			}
		}	
	}
	catch (std::exception & e){
        // catch any errors that might have occured and print their reason
        std::cout << e.what() << std::endl;
        return 1;
    }
    
return 0;
}