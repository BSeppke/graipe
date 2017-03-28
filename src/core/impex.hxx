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

#ifndef GRAIPE_CORE_IMPEX_HXX
#define GRAIPE_CORE_IMPEX_HXX

#include "core/model.hxx"
#include "core/config.hxx"

#include <map>

#include <QString>

/**
 * @file
 * @brief Header file for the import and export of data
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * This defines a complete static class for import/export of all
 * serializable objects (of class Serializable). Since each model has
 * to fulfill this interface, the export is quite simple:
 *
 *     1. Write the object to file using the serialize method.
 *        and compress the file (if wanted) using zip-compression.
 *     2. If successful, return true, else false.
 *
 * The import is managed in this way:
 *
 *     1. Uncompress data if selected.
 *     2. Read the first line of the file.
 *     3. If the type matches with the given ptr->Object, try to
 *        deserialize the complete header and data.
 *     4. If successful, return true, else false.
 *
 * Since the headers are usually organized in a dictionary style, 
 * some helper methods are defined here, too.
 */
class GRAIPE_CORE_EXPORT Impex
{ 
	public:
        /**
         * Basic import procedure for all types, which implement the serializable interface
         *
         * \param filename The filename of the stored object.
         * \param object   The object, which shall be deserialized.
         * \param compress If true, the file will be read using the GZip decompressor.
         * \return True, if the loading of the object was successful.
         */
        static bool load(const QString & filename, Serializable * rs_obj, bool compress=true);
    
        /**
         * Reads the content of (maybe compressed) file to a QString.
         * Mainly needed for views and their serialization.
         *
         * \param filename The filename to be read.
         * \param compress If true, the file will be read using the GZip decompressor.
         */
        static QString readToString(const QString & filename, bool compress=true);
    
        /**
         * Basic import procedure of a settings dictionary from a file.
         * A dictionary is defined by means of a mapping from QString keys
         * to QString values.
         *
         * \param filename The filename to be read.
         * \param compress If true, the file will be read using the GZip decompressor.
         */
        static std::map<QString,QString> dictFromFile(const QString & filename, bool compress=true);
    
        /**
         * Basic import procedure of a settings dictionary from a given QString
         * A dictionary is defined by means of a mapping from QString keys
         * to QString values.
         *
         * \param contents The input QString.
         * \param separator The seaparator, which will be used to split the key/value pairs, default is ": "
         */
         static std::map<QString,QString> dictFromString(const QString & contents, QString separator=": ");
            
        /**
         * Standard exporter for everything, which implements the Serializable interface.
         *
         * \param object   The object, which shall be serialized.
         * \param filename The filename, where the object shall be stored.
         * \param compress If true, the file will be read using the GZip decompressor.
         * \return True, if the storage of the object was successful.
         */
		static bool save(Serializable * rs_obj, const QString & filename, bool compress=true);
};

} //end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_IMPEX_HXX
