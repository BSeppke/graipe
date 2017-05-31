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

#ifndef GRAIPE_IMAGES_GEOCODING_HXX
#define GRAIPE_IMAGES_GEOCODING_HXX

#include <math.h>

#include <QString>
#include <QFile>
#include <QTextStream>

#include "gdal_priv.h"

namespace graipe {


/**
 * Templated class (template argument is precision) for the 
 * representation of Ground Control Points (GCPs). Each GCP 
 * links a pixel position in an image to a ground location on Earth
 */
    
template <typename T = float>
class GCP
{
	public:
		/**
         * Default constructor. Constructs empty GCP
         */
		GCP()
		{
		}

		/**
         * Basic constructor. Constructs a GCP as a mapping from img_x, img_y to 
         * Earth x, y and z. An additional id and description is mandatory.
         *
         * \param id A unique identifier.
         * \param description Some description of the GCP.
         * \param img_x The image's x position of this GCP.
         * \param img_y The image's y position of this GCP.
         * \param map_x The map's x position of this GCP.
         * \param map_y The map's y position of this GCP.
         * \param map_z The map's z position of this GCP.
         */
		GCP(int id, const QString& description, const T& img_x, const T& img_y, const T& map_x, const T& map_y, const T& map_z)
		:	m_id(id), m_description(description),
			m_img_x(img_x),m_img_y(img_y),
			m_map_x(map_x),m_map_y(map_y),m_map_z(map_z)
		{
		}

		/**
         * Derived constructor. Constructs a GCP from the corresponding GDAL data
         * format.
         */
		GCP(const GDAL_GCP& gdal_gcp_struct)
			:	m_id(atoi(gdal_gcp_struct.pszId)), m_description(gdal_gcp_struct.pszInfo),
				m_img_x(gdal_gcp_struct.dfGCPPixel),m_img_y(gdal_gcp_struct.dfGCPLine),
				m_map_x(gdal_gcp_struct.dfGCPX),m_map_y(gdal_gcp_struct.dfGCPY),m_map_z(gdal_gcp_struct.dfGCPZ)
		{
		}

        /**
         * Getter for the identifier of this GCP.
         *
         * \return The id of the GCP.
         */
		int id() const
        {
            return m_id;
        }
    
        /**
         * Getter for the description of this GCP.
         *
         * \return The description of the GCP.
         */
		const QString& description() const
        {
            return m_description;
        }
    
        /**
         * Getter for the image's x position of this GCP.
         *
         * \return The image's x position of the GCP.
         */
		const T& img_x() const
        {
            return m_img_x;
        }
        
        /**
         * Getter for the image's y position of this GCP.
         *
         * \return The image's y position of the GCP.
         */
		const T& img_y() const
        {
            return m_img_y;
        }
        
        /**
         * Getter for the map's x position of this GCP.
         *
         * \return The map's x of the GCP.
         */
		const T& map_x() const
        {
            return m_map_x;
        }
        
        /**
         * Getter for the map's y position of this GCP.
         *
         * \return The map's y of the GCP.
         */
		const T& map_y() const
        {
            return m_map_y;
        }
        
        /**
         * Getter for the map's z position of this GCP.
         *
         * \return The map's z of the GCP.
         */
		const T& map_z() const
        {
            return m_map_z;
        }

		/**
         * Derived getter for map's x coordinate
         * with geographic naming: longitude.
         *
         * \return The map's x of the GCP.
         */
		const T& lon() const
        {
            return m_map_x;
        }

		/**
         * Derived getter for map's y coordinate
         * with geographic naming: latitude.
         *
         * \return The map's y of the GCP.
         */
		const T& lat() const
        {
            return m_map_y;
        }

		/**
         * Derived getter for map's z coordinate
         * with geographic naming: height.
         *
         * \return The map's z of the GCP.
         */
		const T& height() const
        {
            return m_map_z;
        }
    
		/**
         * Derived Getter (for back-conversion to GDAL_GCP)
         *
         * \return The GDAL_GCP representation of this GCP
         */
		GDAL_GCP toGDAL_GCP() const
        {
           char  buffer[20];
           sprintf(buffer,"%d",m_id);
           return GDAL_GCP(buffer, m_description.toStdString().c_str(), img_x, img_y, map_x, map_y, m_map_z);
        }
		
		/**
         * Predicate to check if the id is non-zero.
         *
         * \return true, if id != 0
         */
		bool hasId() const
        {
            return !(m_id == 0);
        }
        
		/**
         * Predicate to check if the description is of positive length.
         *
         * \return true, if size of description != 0
         */
		bool hasInfo() const
        {
            return !m_description.isEmpty();
        }
        
		/**
         * Predicate to check if the height is unequal zero.
         *
         * \return true, if height() != 0
         */
		bool hasHeight() const
        {
            return m_map_z == 0;
        }

		/**
         * Definition of validity w.r.t. a GCP.
         *
         * \return true, if id != 0
         */
		bool isValid() const
        {
            return hasId();
        }

        virtual QString typeName() const
        {
            return "GCP";
        }

	private:
		unsigned int m_id;
		QString m_description;
		T m_img_x, m_img_y, m_map_x, m_map_y, m_map_z;
};

} //end of namespace graipe

#endif // GRAIPE_IMAGES_GEOCODING_HXX
