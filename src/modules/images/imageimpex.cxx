/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2016 by Benjamin Seppke                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    This file is part of the GrAphical Image Processing Enviroment.   */
/*    The GRAIPE Website may be found at:                               */
/*        https://github.com/bseppke/graipe                             */
/*    Please direct questions, bug reports, and contributions to        */
/*    the GitHub page and use the methods provided there.               */
/*                                                                      */
/*    Permission is hereby granted, free of charge, to any person       */
/*    obtaining a copy of this software and associated documentation    */
/*    files (the "Software"), to deal in the Software without           */
/*    restriction, including without limitation the rights to use,      */
/*    copy, modify, merge, publish, distribute, sublicense, and/or      */
/*    sell copies of the Software, and to permit persons to whom the    */
/*    Software is furnished to do so, subject to the following          */
/*    conditions:                                                       */
/*                                                                      */
/*    The above copyright notice and this permission notice shall be    */
/*    included in all copies or substantial portions of the             */
/*    Software.                                                         */
/*                                                                      */
/*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND    */
/*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES   */
/*    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND          */
/*    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT       */
/*    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,      */
/*    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      */
/*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR     */
/*    OTHER DEALINGS IN THE SOFTWARE.                                   */
/*                                                                      */
/************************************************************************/

#include "core/serializable.hxx"

//image type
#include "images/imageimpex.hxx"
#include "images/geocoding.hxx"
#include "vigra/error.hxx"

//gdal and ogr
#include "gdal_priv.h"
#include "ogr_spatialref.h"

namespace graipe {

/** 
 * Type traits for converting standard C++ types
 * to GDAL type identifier using templates only.
 * The template parameter T is denotes the C++ type here.
 */
template <class T> struct GDALTraits                { static GDALDataType gdalTypeID() { return GDT_Unknown; } };
template <>        struct GDALTraits<float>         { static GDALDataType gdalTypeID() { return GDT_Float32; } };
template <>        struct GDALTraits<int>           { static GDALDataType gdalTypeID() { return GDT_Int32;   } };
template <>        struct GDALTraits<unsigned char> { static GDALDataType gdalTypeID() { return GDT_Byte;    } };
    

/**
 * Template function for the filling of image contents using the data that are
 * in the raster of the GDAL image represenatation. In case of an image, which
 * exceeds the maximal width and height, just the maximal upper left region is
 * loaded.
 * This function may throw errors if something went wrong.
 *
 * \param poBand A const pointer to a GDALRasterBand holding the data to be loaded.
 * \param image_band A non-const View to a vigra::MultiArray
 * \return Rescaling of the lowerRight boundaries. If the image can be loaded as a whole,
 *         this returns (1,1), otherwise values between 0..1 for each dimension.
 */
template <typename T>
std::pair<double,double> fillImageBandFromBandData( GDALRasterBand * const poBand, vigra::MultiArrayView<2,T> image_band){
	
	int nXSize = std::min(poBand->GetXSize(),MAX_IMAGE_WIDTH),
		nYSize = std::min(poBand->GetYSize(),MAX_IMAGE_HEIGHT);
	
	//resize image and read in image data
	image_band = vigra::MultiArray<2,T>(typename vigra::MultiArray<2,T>::difference_type(nXSize,nYSize));
	
	CPLErr error = poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize,
                                    image_band.data(), nXSize, nYSize, GDALTraits<T>::gdalTypeID(),
                                    0,0);
    vigra_precondition(error == CE_None, "ImageImpex::fillImageBandFromBandData: Image could not be imported into memory!");
    
	if ( nXSize != poBand->GetXSize() || nYSize != poBand->GetYSize() )
	{
		
        qCritical("ImageImpex::fillImageBandFromBandData: Image is too big, to fit in memory once (%d x %d pixel) - just loading the upper left (%d x %d) pixel region!", poBand->GetXSize() , poBand->GetYSize(), MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT);
		return std::pair<double, double>(MAX_IMAGE_WIDTH/(double)poBand->GetXSize(),MAX_IMAGE_HEIGHT/(double)poBand->GetYSize());
	}
	return std::pair<double, double>(1,1);
}

/**
 * Imports an image from harddisk into the graipe image format. Uses GDAL/OGR
 * to support as many image formats as possible.
 * The template paramter T determines the pixel type of each imported image.
 *
 * \param filename The filename of the image to be loaded.
 * \param image the image, which we fill using the data on harddisk.
 * \return true, if the import was successful, else otherwise.
 */
template<class T>
bool ImageImpex::importImage(const QString & filename, Image<T>& image)
{
	if(!filename.isEmpty())
	{
		try
		{
			//find out basename and file extension, create log  
			int found_ext = filename.lastIndexOf(".");

			QString file_base = filename.left(found_ext),
                    file_ext  = filename.mid(found_ext+1);
			
			//Initialize Image Metrics and log
			float global_left = 0,
                  global_top = 0,
                  global_right = 1,
                  global_bottom = -1;
            
			typename Image<T>::Units_Type units = "px";
			
            qInfo() << "Trying to load file '" << filename << "' from FS\n";
			
			//Search for timestamp of the image using the image's filename (base name)
            QDateTime image_timestamp = QDateTime::fromString(file_base, "yyyyMMdd_HHmmss");
            
            if (!image_timestamp.isValid())
            {
                image_timestamp = QDateTime::fromString(file_base, "yyyy_MM_dd_HH_mm_ss");
            }

            if (!image_timestamp.isValid())
            {
                image_timestamp = QDateTime::fromString(file_base, "yyyyMMdd_HHmm");
            }
            
            if (!image_timestamp.isValid())
            {
                image_timestamp = QDateTime::fromString(file_base, "yyyy_MM_dd_HH_mm");
            }
    
            if (image_timestamp.isValid())
            {
				qInfo()	<< "Found DateTime inside the filename: " << file_base << " => converted to std format => " << image_timestamp.toString() << "\n";
            }
			//Search for georeferences using the GDAL/OGR library
			bool foundGeoRef = false;
			
            GDALDataset  *poDataset;
			double       adfGeoTransform[6];
			
            GDALAllRegister();
			poDataset = (GDALDataset *) GDALOpen(filename.toStdString().c_str(), GA_ReadOnly );
			
			if(poDataset)
            {
				foundGeoRef = true;
								
				//report success
                qInfo()	<< "Image '" << filename << "' can be loaded by GDAL/OGR\n"
                    << "    Size is:\n"
					<< "        [" << poDataset->GetRasterXSize() << ", " << poDataset->GetRasterYSize() << "]\n"
					<< "        raster_count (# of bands): " << poDataset->GetRasterCount() << "\n";
				
				if(poDataset->GetDriver())
					qInfo() << "    using the Driver: \n"
						<< "        " << poDataset->GetDriver()->GetDescription() << "\n"
						<< "        " << poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) << "\n";
				
				//First try: The projection is saved directly at the dataset
				if( poDataset->GetProjectionRef() != NULL && strcmp(poDataset->GetProjectionRef(),"") != 0 )
                {
					qInfo() << "Projection is directly saved inside the dataset:\n"
						<< "    " << poDataset->GetProjectionRef() << "\n";
				
					//Check if there is a proper Geometrical transformation matrix
					if(	poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
                    {
						char* ang_unitnames, *lin_unitnames;
						OGRSpatialReference  	spaRef(  poDataset->GetProjectionRef() );
						double ang = spaRef.GetAngularUnits (&ang_unitnames);
						double lin = spaRef.GetLinearUnits (&lin_unitnames);
						int isGeo =		spaRef.IsGeographic();
						int isProj =	spaRef.IsProjected();
						int isLoc  = 	spaRef.IsLocal(); 
						
						qInfo()	<< "Projection details:\n"
							<< "    angular units: "<< ang << "  " << ang_unitnames << "\n"
							<< "    linear units:  "<< lin << "  " << lin_unitnames << "\n"
							<< "    isGeographic:  "<<  (isGeo?"True":"False") << "\n"
							<< "    isProjected:   "<<  (isProj?"True":"False") << "\n"
							<< "    isLocal:       "<<  (isLoc?"True":"False") << "\n"
							<< "Image-origin: [ " << adfGeoTransform[0] << " , " << adfGeoTransform[3] << "]\n"
							<< "Pixel size:   [ " << adfGeoTransform[1] << " , " << adfGeoTransform[5] << "]\n"
							<< "Image projection matrix:\n"
							<< "    | " << adfGeoTransform[0] << " |  +  | " << adfGeoTransform[1] << " " << adfGeoTransform[2] << " |  *  | x |  =  | x' |\n"
							<< "    | " << adfGeoTransform[3] << " |     | " << adfGeoTransform[4] << " " << adfGeoTransform[5] << " |     | y |     | y' |\n";
						
						//Determine world coordinates of the image
						OGRSpatialReference oTargetS;
						OGRCoordinateTransformation *poCT;
						double   x_i, x_p, x_w, y_i, y_p, y_w;
						oTargetS.SetWellKnownGeogCS( "WGS84" );
						poCT = OGRCreateCoordinateTransformation( &spaRef, &oTargetS );
					
						x_i=0.5;						
						y_i = 0.5;
						x_w = x_p = adfGeoTransform[0]+adfGeoTransform[1]*x_i+adfGeoTransform[2]*y_i;
						y_w = y_p = adfGeoTransform[3]+adfGeoTransform[4]*x_i+adfGeoTransform[5]*y_i;
					
					
						if( poCT == NULL || !poCT->Transform( 1, &x_w, &y_w ) ){
							qInfo() << "Calculation of transformation chain failed.\n";
						}
						else{
							qInfo() << "Transformation chain (for upper left pixel):\n"
								<< "    [ "<< x_i <<" "<< y_i <<" ]  -- c.proj.--> [ "<< x_p <<" "<< y_p <<" ] -- w.proj.--> [  "<< x_w <<" "<< y_w <<" ]\n";
							
							global_left = x_w;
							global_top  = y_w;
						}
						
						x_i = (poDataset->GetRasterXSize()-0.5);
						y_i = (poDataset->GetRasterYSize()-0.5);
						x_w = x_p = adfGeoTransform[0]+adfGeoTransform[1]*x_i+adfGeoTransform[2]*y_i;
						y_w = y_p = adfGeoTransform[3]+adfGeoTransform[4]*x_i+adfGeoTransform[5]*y_i;
						
						
						if( poCT == NULL || !poCT->Transform( 1, &x_w, &y_w ) )
                        {
							qInfo() << "Calculation of transformation chain failed.\n";
						}
						else
                        {
							qInfo() << "Transformation chain (for lower right pixel):\n"
								<< "    [ "<< x_i <<" "<< y_i <<" ]  -- c.proj.--> [ "<< x_p <<" "<< y_p <<" ] -- w.proj.--> [  "<< x_w <<" "<< y_w <<" ]\n";
							
							global_right = x_w;
							global_bottom = y_w;
						}
					}
				}
				//Second try: maybe there are GCPs given to calculate the projection
				else if(poDataset->GetGCPProjection() != NULL && strcmp(poDataset->GetGCPProjection(),"") != 0 )
                {
					qInfo() << "Projection is not directly saved  inside the dataset!\n\n"
					    << "GCP Projection is: \n"
						<< "    " << poDataset->GetGCPProjection() << "\n"
						<< "    now showing GCPs:\n";
					std::vector<GCP<float> > groundControlPoints(poDataset->GetGCPCount());
					for(int i=0;i< poDataset->GetGCPCount(); ++i)
                    {
						GDAL_GCP currentGCP = poDataset->GetGCPs()[i];
						groundControlPoints[i] = GCP<float>(currentGCP);
						//log << "        id: "<< groundControlPoints[i].id()<<" \t info: "<< groundControlPoints[i].description() << " \t ( " << groundControlPoints[i].img_x() << " , \t " << groundControlPoints[i].img_y() << " ) ->  ( "  << groundControlPoints[i].map_x() << " , \t " << groundControlPoints[i].map_y() << " )  @height " << groundControlPoints[i].map_z() << "\n";
					}
				
					//Checking Geometrical transformation matrix
					if(	GDALGCPsToGeoTransform( poDataset->GetGCPCount(), poDataset->GetGCPs(), adfGeoTransform, 0) == CE_None)
                    {
						char* ang_unitnames, *lin_unitnames;
						OGRSpatialReference  	spaRef( poDataset->GetGCPProjection() );
						double ang = spaRef.GetAngularUnits (&ang_unitnames);
						double lin = spaRef.GetLinearUnits (&lin_unitnames);
						int isGeo =		spaRef.IsGeographic();
						int isProj =	spaRef.IsProjected();
						int isLoc  = 	spaRef.IsLocal(); 
						
						qInfo()	<< "Projection details:\n"
							<< "    angular units: "<< ang << "  " << ang_unitnames << "\n"
							<< "    linear units:  "<< lin << "  " << lin_unitnames << "\n"
							<< "    isGeographic:  "<<  (isGeo?"True":"False") << "\n"
							<< "    isProjected:   "<<  (isProj?"True":"False") << "\n"
							<< "    isLocal:       "<<  (isLoc?"True":"False") << "\n"
							<< "Image-origin: [ " << adfGeoTransform[0] << " , " << adfGeoTransform[3] << "]\n"
							<< "Pixel size:   [ " << adfGeoTransform[1] << " , " << adfGeoTransform[5] << "]\n"
							<< "Image projection matrix:\n"
							<< "    | " << adfGeoTransform[0] << " |  +  | " << adfGeoTransform[1] << " " << adfGeoTransform[2] << " |  *  | x |  =  | x' |\n"
							<< "    | " << adfGeoTransform[3] << " |     | " << adfGeoTransform[4] << " " << adfGeoTransform[5] << " |     | y |     | y' |\n";
						
						//Determine world coordinates of the image
						OGRSpatialReference oTargetS;
						OGRCoordinateTransformation *poCT;
						double   x_i, x_p, x_w, y_i, y_p, y_w;
						oTargetS.SetWellKnownGeogCS( "WGS84" );
						poCT = OGRCreateCoordinateTransformation( &spaRef, &oTargetS );
						
						x_i=0.5;						
						y_i = 0.5;
						x_w = x_p = adfGeoTransform[0]+adfGeoTransform[1]*x_i+adfGeoTransform[2]*y_i;
						y_w = y_p = adfGeoTransform[3]+adfGeoTransform[4]*x_i+adfGeoTransform[5]*y_i;
						
						if( poCT == NULL || !poCT->Transform( 1, &x_w, &y_w ) ){
							qInfo() << "Calculation of transformation chain failed.\n";
						}
						else{
							qInfo() << "Transformation chain (for upper left pixel):\n"
							<< "    [ "<< x_i <<" "<< y_i <<" ]  -- c.proj.--> [ "<< x_p <<" "<< y_p <<" ] -- w.proj.--> [  "<< x_w <<" "<< y_w <<" ]\n";
							
							global_left = x_w;
							global_top  = y_w;
						}
						
						x_i = (poDataset->GetRasterXSize()-0.5);
						y_i = (poDataset->GetRasterYSize()-0.5);
						x_w = x_p = adfGeoTransform[0]+adfGeoTransform[1]*x_i+adfGeoTransform[2]*y_i;
						y_w = y_p = adfGeoTransform[3]+adfGeoTransform[4]*x_i+adfGeoTransform[5]*y_i;
						
						if( poCT == NULL || !poCT->Transform( 1, &x_w, &y_w ) ){
							qInfo() << "Calculation of transformation chain failed.\n";
						}
						else{
							qInfo() << "Transformation chain (for lower right pixel):\n"
							<< "    [ "<< x_i <<" "<< y_i <<" ]  -- c.proj.--> [ "<< x_p <<" "<< y_p <<" ] -- w.proj.--> [  "<< x_w <<" "<< y_w <<" ]\n";
							
							global_right = x_w;
							global_bottom = y_w;
						}
					
						double err=0;
						for(int i=0;i< poDataset->GetGCPCount(); ++i){
							GDAL_GCP currentGCP = poDataset->GetGCPs()[i];
							double	soll_x = currentGCP.dfGCPX, 
									soll_y= currentGCP.dfGCPY,
									
									ist_x =  adfGeoTransform[0]+adfGeoTransform[1]*currentGCP.dfGCPPixel+adfGeoTransform[2]*currentGCP.dfGCPLine, 
									ist_y =  adfGeoTransform[3]+adfGeoTransform[4]*currentGCP.dfGCPPixel+adfGeoTransform[5]*currentGCP.dfGCPLine;
							//add squared error
							err+= (ist_x-soll_x)*(ist_x-soll_x) + (ist_y-soll_y)*(ist_y-soll_y);
						}
						//mean squared error
						err=err/poDataset->GetGCPCount();
						
						//mean error
						err = sqrt(err);
						
						qInfo() << "Mean error by using this projection:  " << err << "\n";
					}
				}
				
				//Try to read the metadata
				char** metadata = poDataset->GetMetadata();
				char** p_meta = metadata;
				
                QString meta_type;
				if(p_meta)
				{
					while (*p_meta)
					{
                        QString str(*p_meta);
						QStringList lst = str.split("=");
                        
                        if(lst.size() > 1)
                        {
                            QDateTime val_datetime = qDateTimeFromSatelliteDateTime(lst[1]);
                            if(val_datetime.isValid())
                            {
                                meta_type = "date_time";
                            }
                        }
						else
                        {
                            /*
                            bool ok;
                            
                            int	val_i = lst[1].toInt(&ok);
                            if (ok)
                            {
                                meta_type = "int";
							}
                            else
                            {
                                long val_l = lst[1].toLong(&ok);
                                if (ok)
                                {
                                    meta_type = "long";
                                }
                                else
                                {
                                    float val_f = lst[1].toFloat(&ok);
                                    if (ok)
                                    {
                                        meta_type = "float";
                                    }
                                    else
                                    {
                                        double val_d = lst[1].toDouble(&ok);
                                        if (ok)
                                        {
                                            meta_type = "double";
                                        }
                                    }
                                }
							}
                            */
						}
                        if (meta_type.isEmpty())
                        {
                            meta_type = "string";
                        }
                        
						qInfo() << lst[0]  << "\t=\t" << lst[1] << " (" << meta_type << ")\n";
						p_meta++;
					}
				}
                
				//Local embedding
				if(		image.left() == 0
				   &&	image.top() == 0
				   &&	image.right() == 0
				   &&	image.bottom() == 0)
				{
					image.setLeft( 0);
					image.setTop( 0 );
					image.setRight( poDataset->GetRasterXSize()>MAX_IMAGE_WIDTH? MAX_IMAGE_WIDTH:  poDataset->GetRasterXSize());
					image.setBottom(poDataset->GetRasterYSize()>MAX_IMAGE_HEIGHT?MAX_IMAGE_HEIGHT: poDataset->GetRasterYSize());
				}
				
				//width, height and number of bands are still determined by the image's data
				image.setWidth( poDataset->GetRasterXSize()>MAX_IMAGE_WIDTH? MAX_IMAGE_WIDTH:  poDataset->GetRasterXSize());
				image.setHeight(poDataset->GetRasterYSize()>MAX_IMAGE_HEIGHT?MAX_IMAGE_HEIGHT: poDataset->GetRasterYSize());
				image.setNumBands(poDataset->GetRasterCount());
				
				std::pair<double, double> rescale;
				//fill image
				for(unsigned int c=0; c< image.numBands(); c++)
				{
					GDALRasterBand* poBand = poDataset->GetRasterBand( c+1 );
					rescale = fillImageBandFromBandData(poBand, image.band(c));
				}
				
				
				global_right  = global_left + (global_right  - global_left)*rescale.first;
				global_bottom = global_top  + (global_bottom - global_top)*rescale.second;
				
				//Set filename in each case!
				image.setID(QString::number((long int)&image));
				
                QFileInfo fi(filename);
                
				image.setName("Image: "+ fi.fileName());
				image.setDescription(image.description() + "\nThe image was loaded from: "+ filename);
				
				//Model global coords
				if(		image.globalLeft() == 0
				   &&	image.globalTop() == 0
				   &&	image.globalRight() == 0
				   &&	image.globalBottom() == 0)
				{
					image.setGlobalLeft(global_left);
					image.setGlobalTop(global_top);
					image.setGlobalRight(global_right);
					image.setGlobalBottom(global_bottom);
				}
				//Model units
				if( image.units().isEmpty())
				{
					image.setUnits(units);
				}
                
				//Remove GDAL file handle
				GDALClose(poDataset);
				
				return true;
				
			};
		}
        catch (vigra::StdException & e)
        {
			qInfo() << "Error: File: " << filename << " was not loaded because: "<< e.what() << "\n";
		}
	}
	return false;
}

/**
 * Exports an image from the graipe image format onto harddisk. Uses GDAL/OGR
 * to support as many image formats as possible.
 * The template paramter T determines the pixel type of each exported image.
 *
 * \param image the image, which we want so store on harddisk
 * \param filename The filename of the image.
 * \param format the image format. Defaults to "GTIFF" (aka GeoTiff). For more options,
 *        look at "http://www.gdal.org/formats_list.html".
 * \return true, if the export was successful, else otherwise.
 */
template <typename T>
bool ImageImpex::exportImage(const Image<T>& image, const QString& filename, const QString& format)
{
    GDALDriver *poDriver;
    char **papszMetadata;
	
	GDALAllRegister();
    poDriver = GetGDALDriverManager()->GetDriverByName(format.toStdString().c_str());
	
    if( poDriver == NULL )
        return false;
	
    papszMetadata = poDriver->GetMetadata();
    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
	{
			
		GDALDataset *poDstDS;       
		char **papszOptions = NULL;
		
		if (format == "GTiff")    
			papszOptions = CSLSetNameValue( papszOptions, "COMPRESS", "LZW" );
		
		// double adfGeoTransform[6] = { 444720, 30, 0, 3751320, 0, -30 };
		// OGpatialReference oS;
		// char *pszS_WKT = NULL;
		// poDstDS->SetGeoTransform( adfGeoTransform );
		// oS.SetUTM( 11, TRUE );
		// oS.SetWellKnownGeogCS( "NAD27" );
		// oS.exportToWkt( &pszS_WKT );
		// poDstDS->SetProjection( pszS_WKT );
		// CPLFree( pszS_WKT );
		
		poDstDS = poDriver->Create( filename.toStdString().c_str(), image.width(), image.height(), image.numBands(), GDALTraits<T>::gdalTypeID(),
								   papszOptions );
		
		if(poDstDS)
		{
			GDALRasterBand *poBand;
			for(unsigned int c=1; c<=image.numBands(); c++)
			{
				poBand = poDstDS->GetRasterBand(c);
				if (poBand)
                {	
					CPLErr error = poBand->RasterIO(GF_Write, 0, 0,image.width(), image.height(),
									 (void*)image.band(c-1).data(), image.width(), image.height(), GDALTraits<T>::gdalTypeID(), 0, 0 );
                    
                    vigra_precondition(error == CE_None, "ImageImpex::fillImageBandFromBandData: Image could not be imported into memory!");
				}
				else
				{
					break;					
				}
			}
		}
		// Once we're done, close properly the dataset
		GDALClose( (GDALDatasetH) poDstDS );
		return true;
	}
	return false;
}


//Promote image import facilities for all three main image types:
template bool ImageImpex::importImage(const QString & filename, Image<float>& image);
template bool ImageImpex::importImage(const QString & filename, Image<int>& image);
template bool ImageImpex::importImage(const QString & filename, Image<unsigned char>& image);
    
//Promote image export facilities for all three main image types:
template bool ImageImpex::exportImage<float>(const Image<float>& image, const QString & filename, const QString& format);
template bool ImageImpex::exportImage<int>(const Image<int>& image, const QString & filename, const QString& format);
template bool ImageImpex::exportImage<unsigned char>(const Image<unsigned char>& image, const QString & filename, const QString& format);


/**
 * Default constructor for the image importer class.
 */
ImageImporter::ImageImporter(Environment* env)
:   Algorithm(env),
    m_filename(new FilenameParameter("Image filename", "", NULL)),
    m_pixeltype(NULL)
{
    m_parameters->addParameter("filename", m_filename);
    
    QStringList types;
		types.append("float");
		types.append("int");
		types.append("unsigned char");
    m_pixeltype = new EnumParameter("Image pixel type:",types,0);
    m_parameters->addParameter("pixeltype",m_pixeltype);
    m_results.push_back(new Image<float>(env));
    
    connect(m_pixeltype, SIGNAL(valueChanged()), this, SLOT(pixelTypeChanged()));
}

/**
 * Specialization of the running phase of this algorithm.
 */
void ImageImporter::run()
{
    try 
    {
        emit statusMessage(0.0, QString("started"));
        
        bool res=false;
        
        switch(m_pixeltype->value())
        {
            case 0:
                res=ImageImpex::importImage(m_filename->value(), *static_cast<Image<float>*>(m_results[0]));
                break;
            case 1:
                res=ImageImpex::importImage(m_filename->value(), *static_cast<Image<int>*>(m_results[0]));
                break;
            case 2:
                res=ImageImpex::importImage(m_filename->value(), *static_cast<Image<unsigned char>*>(m_results[0]));
                break;
        }
        
        if(res)
        {
            emit statusMessage(100.0, QString("finished computation"));
            emit finished();
        }
        else 
        {
            emit errorMessage(QString("Explainable error occured: Image could not be imported"));
        }
    }
    catch(std::exception& e)
    {
        emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
    }
    catch(...)
    {
        emit errorMessage(QString("Non-explainable error occured"));
    }
}

/**
 * This slot is called everytime the pixeltype parameter changes.
 * It is used to present the user the resulting type of the image.
 */
void ImageImporter::pixelTypeChanged()
{
    delete m_results[0];
    m_results.clear();
    
    switch(m_pixeltype->value())
    {
        case 0:
            m_results.push_back(new Image<float>(m_environment));
            break;
        case 1:
            m_results.push_back(new Image<int>(m_environment));
            break;
        case 2:
            m_results.push_back(new Image<unsigned char>(m_environment));
            break;
    }
}

/**
 * Default constructor for the image exporter class.
 */
ImageExporter::ImageExporter(Environment * env)
: Algorithm(env)
{
	QStringList format_names;
		format_names.append("GIF"); format_names.append("GTiff");
		format_names.append("JPEG"); format_names.append("netCDF");
		format_names.append("PNG"); format_names.append("XYZ");

    m_parameters->addParameter("image", new ModelParameter("Image",	"Image, IntImage, ByteImage", NULL, false, env));
    m_parameters->addParameter("filename", new FilenameParameter("Image filename", "", NULL));
    m_parameters->addParameter("format", new EnumParameter("File format", format_names));
}

/**
 * Specialization of the running phase of this algorithm.
 */
void ImageExporter::run()
{
    try 
    {
        emit statusMessage(0.0, QString("started"));
        
        ModelParameter		* param_image      = static_cast<ModelParameter*> ((*m_parameters)["image"]);
        FilenameParameter	* param_filename = static_cast<FilenameParameter*> ((*m_parameters)["filename"]);
        EnumParameter		* param_fileformat = static_cast<EnumParameter*> ((*m_parameters)["format"]);
        
        bool res=false;
        
        if(param_image->value()->typeName() == "Image")
        {
            res = ImageImpex::exportImage(*static_cast<Image<float>*>(param_image->value()),
                                          param_filename->value(),
                                          param_fileformat->toString());
        }
        
        if(param_image->value()->typeName() == "IntImage")
        {
            res = ImageImpex::exportImage(*static_cast<Image<int>*>(param_image->value()),
                                          param_filename->value(),
                                          param_fileformat->toString());
        }
        else if(param_image->value()->typeName() == "ByteImage")
        {
            res = ImageImpex::exportImage(*static_cast<Image<unsigned char>*>(param_image->value()),
                                          param_filename->value(),
                                          param_fileformat->toString());
        }
        
        if(res)
        {
            emit statusMessage(100.0, QString("finished computation"));
            emit finished();				
        }
        else {
            emit errorMessage(QString("Explainable error occured: Image could not be exported!"));
        }

        
    }
    catch(std::exception& e)
    {
        emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
    }
    catch(...)
    {
        emit errorMessage(QString("Non-explainable error occured"));		
    }
}

} //end of namespace graipe
