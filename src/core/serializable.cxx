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
#include "core/impex.hxx"

#include <QUrl>

/**
 * @file
 * @brief This file implements the needed serialization procedures.
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
QStringList split_string(const QString & str, const QString & sep)
{
    return str.split(sep);
}

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
QStringList split_string_once(const QString & str, const QString & sep)
{
    QStringList result;
    
    int index = str.indexOf(sep);
    
    if( index == -1)
    {
        result.push_back(str);
    }
    else
    {
        result.push_back(str.left(index));
        result.push_back(str.mid(index+sep.size()));
    }
    
    return result;
}

/**
 * Encodes a QString due to the HTML-Get encoding style. 
 * This is used to store QStrings in models' serializations to get 
 * rid of the newline and other problems.
 *
 * \param str the QString to be encoded
 *
 * \return the URL-encoded QString
 */
QString encode_string(const QString & str)
{
    return QUrl::toPercentEncoding(str.toUtf8());
}

/**
 * Decodes a QString due to the HTML-Get decoding style.
 * This is used to restore QStrings from models' serializations to get
 * rid of the newline and other problems.
 *
 * \param str the URL-encoded QString
 *
 * \return the decoded string
 */
QString decode_string(const QString & str)
{
    return QUrl::fromPercentEncoding(str.toUtf8());
}

/**
 * Generate a qDateTime from a satellite_format QString.
 * The string has to be of format:  DD-MON-YEAR hh:mm:ss.micsec
 *
 * \param str The formatted DateTime QString
 * 
 * \return QDateTime representation of this string. May be invalid, if conversion fails.
 */
QDateTime qDateTimeFromISODateTime(const QString& str)
{
    //Format is: DD-MON-YEAR hh:mm:ss.micsec
    QDateTime result = QDateTime::fromString(str,
                                             "dd-MMM-yyyy'hh:mm:ss.zzz'''");
    result.setTimeSpec(Qt::UTC);
    return result;
}

/**
 * Generate a qDateTime from a numerically listed QString.
 * The string has to be  of format: yyyyMMddhhmmssmicsec
 *
 * \param str The formatted DateTime QString
 * 
 * \return QDateTime representation of this string. May be invalid, if conversion fails.
 */
QDateTime qDateTimeFromNumberDateTime(const QString& str)
{
    //Format is: yyyyMMddhhmmssmicsec
    QDateTime result = QDateTime::fromString(str,
                                            "yyyyMMMddhhmmsszzz'''");
    result.setTimeSpec(Qt::UTC);
    return result;
}

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
QDateTime qDateTimeFromSatelliteDateTime(const QString& str)
{
    QDateTime result = qDateTimeFromISODateTime(str);
    if (result.isValid())
        return result;
    else
        return qDateTimeFromNumberDateTime(str);
}


/**
 * Writes a QString onto a QIODevice.
 * We wirte the characters by means of UTF8 encoding.
 *
 * \param str the QString, which shall be written.
 * \param dev the QIODevice, where we read from.
 */
void write_on_device(const QString& str, QIODevice& dev)
{
    dev.write(str.toUtf8());
}

/**
 * Read a QString of a given length from a QIODevice.
 * We assume, that the QIODevice can read characters by means of UTF8.
 *
 * \param dev the QIODevice, where we read from.
 * \param len How many characters should being read?
 * 
 * \return the QString read from the stream
 */
QString read_from_device(QIODevice& dev, int len)
{
    QByteArray arr = dev.read(len);
    return QString::fromUtf8(arr);
}

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
QString read_from_device_until(QIODevice& dev, const QString& str, bool one_line)
{
    int match_pos=0;
    
    QString sofar;
    
    while(!dev.atEnd() || match_pos == str.size())
    {
        QByteArray arr = dev.read(1);
        QString current = QString::fromUtf8(arr);
     
        sofar += current;
        
        //break on lines
        if (one_line && current == "\n")
            return "";
        
        //current char matches next char to be found
        if (current == str.at(match_pos))
        {
            match_pos++;
        }
        else
        {
            match_pos=0;
        }
        if(match_pos == str.size())
            return sofar;
    }
    
    return "";
}

/**
 * Getter for the id of a serializable
 *
 * \return the id or an empty QString if none is assigned
 */
QString Serializable::id() const
{
    return m_id;
}

/**
 * Setter for the id of a serializable
 *
 * \param new_id  the new id of this serializable
 */
void Serializable::setID(const QString& new_id)
{
    m_id = new_id;
}

/**
 * Since we want to identify, we assign the Serializable class an uid.
 * Should be overwritten in iheriting classes.
 *
 * \return "Serializable" as a QString
 */
QString Serializable::typeName() const
{
    return "Serializable";
}

} //end of namespace graipe

/**
 * @}
 */
