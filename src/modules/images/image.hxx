/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2017 by Benjamin Seppke                 */
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

#ifndef GRAIPE_IMAGES_IMAGE_HXX
#define GRAIPE_IMAGES_IMAGE_HXX

#include "core/core.h"
#include "images/config.hxx"

#include "vigra/multi_array.hxx"

#include <QDateTime>

namespace graipe {

/**
 * @addtogroup graipe_images
 * @{
 *
 * @file
 * @brief Header file for image classes
 */

/** 
 * Implementation of the standard image format for (remote sensing) images.
 * An image herein consist of a set of parameters and a list of image bands.
 * Each image band describes one channel of the image.
 *
 * This class extends the RasteredModel class, the template argument is
 * defining the pixel type.
 */
template<class T>
class GRAIPE_IMAGES_EXPORT Image
:   public RasteredModel
{
	public:
        //Some convenient TypeDefs
		typedef QDateTime       DateTime_Type;
		typedef QString         String_Type;
		typedef String_Type     Comment_Type;
		typedef String_Type     Units_Type;
		typedef float           Scale_Type;
		typedef vigra::Shape2	Size_Type;
		
		/**
         * Default constructor. Constructs an empty Image
         * with zero size and no bands at all.
         */
		Image(Workspace* wsp);
		
		/**
         * Copy constructor. Constructs a new Image from another Image.
         *
         * \param img The other image.
         */
        Image(const Image<T> & img);
    
        /**
         * A more flexible contructor.
         * Constructs an image of a given size. Also allows to set the other
         * parameter's values, too.
         *
         * \param size The image's shape.
         * \param numBands The image's band count.
         
         */
		Image(Size_Type size, unsigned int numBands, Workspace* wsp);
    
        /**
         * Specialization of the virtual destructor of RasteredModel
         */
		~Image();
    
    
        /**
         * Returns unique typeNames for each Image type. Currently, we define 
         * three different typeNames:
         *    Image<unsigned char> ---> "ByteImage"
         *    Image<unsigned int>  ---> "IntIamge
         *    Image<float>         ---> "Image" (default)
         *
         * \return the typeName of the Image.
         */
        QString typeName() const;
    
        /**
         * Check if an Image has  > 0 pixels
         *
         * \return true, if numBands() and size() are non-zero
         */
        bool isEmpty() const;
    
        /**
         * Access the shape of one band of this image.
         *
         * \return The shape of one band of this image.
         */
        Size_Type size() const;
    
        /**
         * Constant/reading access to a band of the image at a given band_id.
         * If no band_id is given, the first band (band_id=0) will be returned.
         * This function may throw an error, if the band_id is out of bounds.
         *
         * \param band_id The id of the band.
         * \return The band, as a const vigra::MultiArrayView.
         */
		const vigra::MultiArrayView<2,T>& band( unsigned int band_id = 0) const;
    
        /**
         * Setting access to a band of the image at a given band_id.
         * This function may throw an error, if the band_id is out of bounds.
         *
         * \param band_id The id of the band.
         * \param band The band, as a const vigra::MultiArrayView.
         */
		void setBand(unsigned int band_id, const vigra::MultiArrayView<2,T>& band);
        
        /**
         * Non-constant/reading&writing access to a band of the image at a given band_id.
         * If no band_id is given, the first band (band_id=0) will be returned.
         * This function may throw an error, if the band_id is out of bounds.
         * This function returns an invalid view if the model is locked!
         *
         * \param band_id The id of the band.
         * \return The band, as a vigra::MultiArrayView. If the model is locked, an invalid one.
         *
		vigra::MultiArrayView<2,T> band(unsigned int band_id = 0);
        */
    
        /**
         * Getter for the number of bands of an Image.
         *
         * \return The number of bands of the Image.
         */
		unsigned int numBands() const;
		
        /**
         * Setter for the number of bands of an Image.
         *
         * \param badnds The new number of bands of the Image.
         */
        void setNumBands(unsigned int bands);

        /**
         * Getter for the timestamp of an Image.
         *
         * \return The timestamp of the Image.
         */
        DateTime_Type timestamp() const;
        
        /**
         * Getter for the comment of an Image.
         *
         * \return The comment of the Image.
         */
		Comment_Type comment() const;
        
        /**
         * Getter for the units of an Image.
         *
         * \return The units of the Image.
         */
		Units_Type units() const;
    
        /**
         * Getter for the scale of an Image.
         *
         * \return The scale of the Image.
         */
		Scale_Type scale() const;
    
        /**
         * Setter for the timestamp of an Image.
         *
         * \param timestamp The new timestamp of the Image.
         */
		void setTimestamp(const DateTime_Type& timestamp);
    
        /**
         * Setter for the comment of an Image.
         *
         * \param timestamp The new comment of the Image.
         */
		void setComment(const Comment_Type& comment);
        
        /**
         * Setter for the units of an Image.
         *
         * \param timestamp The new units of the Image.
         */
		void setUnits(const Units_Type& units);
        
        /**
         * Setter for the scale of an Image.
         *
         * \param timestamp The new scale of the Image.
         */
		void setScale(Scale_Type scale);
	
        /**
         * Copies the complete metadata (defined by the parameters) from another Model.
         *
         * \other The other model.
         */
		void copyMetadata(Model& other) const;
    
        /**
         * Const copy model's complete data (and metadata) to another model.
         *
         * \param other The other model.
         */
		void copyData(Model& other) const;
	
        /**
         * Serialization of the Image to an xml file.
         * The serialization is just a binary stream of all bands, one after the other.
         *
         * \param xmlWriter The QXmlStreamWriter where we will put our output on.
         */
		void serialize_content(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization of a list of polygons from an xml file.
         * Since the serialization is just a binary stream of all bands, one after the other
         * and we already know the size and count of bands, it is quite easy the deserialize.
         *
         * \param xmlReader The QXmlStreamReader, where we will read from.
         */
		bool deserialize_content(QXmlStreamReader& xmlReader);
	
    public slots:
        /**
         * Specialization of RasterModel's updateModel procedure. 
         * Is called everytime, the model changes.
         */
        void updateModel();
    
	protected:
        /**
         * Add the other parameters to the ones of the RasteredModel
         */
        void appendParameters();
    
        //Storage of the image bands
		std::vector<vigra::MultiArray<2,T> > m_imagebands;
		
        //Filename (without parameter/hidden)
		String_Type			m_filename;
    
        //Additional parameters
        IntParameter        * m_numBands;
        DateTimeParameter   * m_timestamp;
        DoubleParameter     * m_scale;
        LongStringParameter * m_comment;
        StringParameter     * m_units;
};

/**
 * @}
 */

} //end of namespace graipe

#endif
