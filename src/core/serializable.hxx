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
#include <vector>
#include <map>

#include <QDateTime>
#include <QIODevice>
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
 * Splits a string using a given separator.
 *
 * \param str the QString to be splitted
 * \param sep the separator, where we want to split
 *
 * \return If sep is found n times, a QStringList with n+1 items, each QString before the
 *         separator and each QString after the separator. If not found, the list
 *         just contains one element, namely the given string.
 */
GRAIPE_CORE_EXPORT QStringList split_string(const QString & str, const QString & sep);

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
 * Encodes a QString due to the HTML-Get encoding style. 
 * This is used to store QStrings in models' serializations to get 
 * rid of the newline and other problems.
 *
 * \param str the QString to be encoded
 *
 * \return the URL-encoded QString
 */
GRAIPE_CORE_EXPORT QString encode_string(const QString & str);

/**
 * Decodes a QString due to the HTML-Get decoding style.
 * This is used to restore QStrings from models' serializations to get
 * rid of the newline and other problems.
 *
 * \param str the URL-encoded QString
 *
 * \return the decoded string
 */
GRAIPE_CORE_EXPORT QString decode_string(const QString & str);

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
 * Writes a QString onto a QIODevice.
 * We wirte the characters by means of UTF8 encoding.
 *
 * \param str the QString, which shall be written.
 * \param dev the QIODevice, where we read from.
 */
GRAIPE_CORE_EXPORT void write_on_device(const QString& str, QIODevice& dev);

/**
 * Read a QString of a given length from a QIODevice.
 * We assume, that the QIODevice can read characters by means of UTF8.
 *
 * \param dev the QIODevice, where we read from.
 * \param len How many characters should being read?
 * 
 * \return the QString read from the stream
 */
GRAIPE_CORE_EXPORT QString read_from_device(QIODevice& dev, int len);

/**
 * Reads from a QIODevice until a certain QString is found or one line is finished.
 * We assume, that the QIODevice can read characters by means of UTF8.
 *
 * \param dev the QIODevice, where we read from.
 * \param str the QString where we stop reading from the device
 * \param one_line If true, only one line is read at maximum.
 *
 * \return All characters read sofar (if the matching was successful, else ""
 */
GRAIPE_CORE_EXPORT QString read_from_device_until(QIODevice& dev, const QString& str, bool one_line=true);


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
         * Getter for the filename of a serializable
         *
         * \return the filename or an empty QString if none is assigned
         */
        virtual QString filename() const;
    
        /**
         * Setter for the filename of a serializable
         *
         * \param new_filename  the new filename of this serializable
         */
        virtual void setFilename(const QString& new_filename);
    
        /**
         * Since we want to identify, we assign the Serializable class an uid.
         * Should be overwritten in iheriting classes.
         *
         * \return "Serializable" as a QString
         */
        virtual QString typeName() const;
    
        /**
         * Returns the unique magic id of a Serializable instance. 
         * Has to be specialized in inheriting classes.
         *
         * \return The unique magicID.
         */
        virtual QString magicID() const = 0;
    
        /**
         * Deserialization from an input device.
         * Has to be specialized in inheriting classes.
         *
         * \param in The input device.
         * \return True, if the object could be successfully restored from the device.
         */
        virtual bool deserialize(QXmlStreamReader& xmlReader) = 0;
    
        /**
         * Serialization on to an output device.
         * Has to be specialized in inheriting classes.
         *
         * \param out The output device.
         */
        virtual void serialize(QXmlStreamWriter& xmlWriter) const = 0;
    
    protected:
        /** the filename of this serializable instance **/
        QString m_filename;
};
    
}//end of namespace

/**
 * @}
 */

#endif //GRAIPE_CORE_SERIALIZABLE_HXX
