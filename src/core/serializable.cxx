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

/**
 * @file
 * @brief This file implements the needed serialization procedures.
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
