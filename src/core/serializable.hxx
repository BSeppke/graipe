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

#ifndef GRAIPE_CORE_SERIALIZABLE_HXX
#define GRAIPE_CORE_SERIALIZABLE_HXX

#include "core/config.hxx"

#include <QString>
#include <QDateTime>
#include <QXmlStreamWriter>

/**
 * @file
 * @brief This file holds all data interfaces, which are needed for serialization
 * and deserialization of whatever kind of object. It also defines some 
 * helper functions to split comma-separated QStrings into vectors of chars
 * and to encode/decode QStrings. Additionally conversion of datetime QStrings
 * to QDateTime objects is defined, too.
 *
 * All GRAIPE Models and all ViewControllers should derive form the class
 * Serializable and implement serialization accordingly. You are kindly invited
 * to use man-readable codes like CSV-files with some header...
 *
 * If you implement your models and ViewControllers this way, GRAIPE gives 
 * you model import/export for free. Additionally, it offers you a complete
 * workspace store/restore!
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Splits a string using a given separator on the first occurence only.
 *
 * \param str the QString to be splitted
 * \param sep the separator, where we want to split once
 *
 * \return If sep is found, a QStringList with two items, the QString before the
 *         separator and the QString after the separator. If not found, the list 
 *         just contains one element, namely the given string.
 */
GRAIPE_CORE_EXPORT QStringList split_string_once(const QString & str, const QString & sep);

/**
 * Generate a qDateTime from a satellite_format QString.
 * The string has to be of format:  DD-MON-YEAR hh:mm:ss.micsec
 *
 * \param str The formatted DateTime QString
 * 
 * \return QDateTime representation of this string. May be invalid, if conversion fails.
 */
GRAIPE_CORE_EXPORT QDateTime qDateTimeFromISODateTime(const QString& str);

/**
 * Generate a qDateTime from a numerically listed QString.
 * The string has to be  of format: yyyyMMddhhmmssmicsec
 *
 * \param str The formatted DateTime QString
 * 
 * \return QDateTime representation of this string. May be invalid, if conversion fails.
 */
GRAIPE_CORE_EXPORT QDateTime qDateTimeFromNumberDateTime(const QString& str);
/**
 * Generate a qDateTime from a satellite_format or a numerically listed QString.
 * The string has to be either of format:
 *    DD-MON-YEAR hh:mm:ss.micsec 
 * or:
 *    yyyyMMddhhmmssmicsec
 *
 * \param str The formatted DateTime QString
 * 
 * \return QDateTime representation of this string. May be invalid, if conversion fails.
 */
GRAIPE_CORE_EXPORT QDateTime qDateTimeFromSatelliteDateTime(const QString& str);

/**
 * The nearly pure interface of something that can be serialized
 *
 * The serialization is based on a header/content division of the data:
 * - the first Header Line will be used to determine the data type
 * - the following header lines (up to a \n\n) will be used to store 
 *   header information in a dictionary way: [key]": "[value]\n
 * - after the separation between header and content (\n\n), the content
 *   begins in whatever format, either readable or not-readable
 *
 * For your own classes, you have to implement at least:
 * - header serialization
 * - header deserialization using the dictionary
 * - content serialization
 * - content deserialization using a QString
 * - check/export of the first header line signature
 */
class GRAIPE_CORE_EXPORT Serializable	
{
    public:
        /**
         * Getter for the id of a serializable
         *
         * \return the id or an empty QString if none is assigned
         */
        virtual QString id() const;
    
        /**
         * Setter for the id of a serializable
         *
         * \param new_id  the new id of this serializable
         */
        virtual void setID(const QString& new_id);
    
        /**
         * Since we want to identify, we assign the Serializable class an uid.
         * Should be overwritten in iheriting classes.
         *
         * \return "Serializable" as a QString
         */
        virtual QString typeName() const;
    
        /**
         * Deserialization of a parameter's state from an xml file.
         *
         * \param xmlReader The QXmlStreamReader, where we read from.
         * \return True, if the deserialization was successful, else false.
         */
        virtual bool deserialize(QXmlStreamReader& xmlReader) = 0;
    
        /**
         * Serialization on to an output device.
         * Has to be specialized in inheriting classes.
         *
         * \param xmlWriter The QXmlStreamWriter on which we want to serialize.
         */
        virtual void serialize(QXmlStreamWriter& xmlWriter) const = 0;
    
    protected:
        /** The id (sometimes used as a filename) of this serializable instance **/
        QString m_id;
};
    
}//end of namespace

/**
 * @}
 */

#endif //GRAIPE_CORE_SERIALIZABLE_HXX
