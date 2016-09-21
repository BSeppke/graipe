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

#ifndef GRAIPE_IMAGES_IMAGEIMPEX_HXX
#define GRAIPE_IMAGES_IMAGEIMPEX_HXX

#include "images/image.hxx"
#include "images/config.hxx"

namespace graipe {
    
/**
 * The ImageImpex class is just a frame for two static template functions, which
 * are using GDAL/OGR to import or export images into the graipe-format.
 */
class GRAIPE_IMAGES_EXPORT ImageImpex
{
    public:
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
        static bool importImage(const QString & filename, Image<T> & image);

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
        template<class T>
        static bool exportImage(const Image<T> & image, const QString & filename, const QString& format="GTiff");
};

    
/**
 * Image import implemented by means of algorithm.
 * This class implements an algorithm to import images.
 */
class ImageImporter
:   public Algorithm
{
    Q_OBJECT

    public:
        /**
         * Default constructor for the image importer class.
         */
        ImageImporter();
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run();

    protected slots:
        /**
         * This slot is called everytime the pixeltype parameter changes.
         * It is used to present the user the resulting type of the image.
         */
        void pixelTypeChanged();

    private:
        //Additional parameters
        FilenameParameter* m_filename;
        EnumParameter* m_pixeltype;
};

    
/**
 * Image export implemented by means of algorithm.
 * This class implements an algorithm to export images.
 */
class ImageExporter
:   public Algorithm
{
    public:
        /**
         * Default constructor for the image exporter class.
         */
        ImageExporter();
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run();
};

} //end of namespace graipe

#endif //GRAIPE_IMAGES_IMAGEIMPEX_HXX