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

#include "core/impex.hxx"
#include "core/workspace.hxx"

#include <QFile>
#include "core/qt_ext/qiocompressor.hxx"
#include "core/factories.hxx"

#include "core/parameters/longstringparameter.hxx"

/**
 * @file
 * @brief Implementation file for the import and export of data
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Basic open procedure for compressed and uncompressed files.
 *
 * \param filename The filename of the stored object.
 * \param openMode An openind mode, read/write-only etc.
 * \return A valid QIODevice Pointer, if the opening was successful esle NULL.
 */
QIODevice* Impex::openFile(const QString & filename, QIODevice::OpenModeFlag openMode)
{
    QIODevice* device = NULL;
    bool compress = (filename.right(2) == "gz");
    
    if(!filename.isEmpty())
    {
        QFile* file = new QFile(filename);
        
        if(compress)
        {
            QIOCompressor* compressor = new QIOCompressor(file);
            compressor->setStreamFormat(QIOCompressor::GzipFormat);

            if (compressor->open(openMode))
            {
                device = compressor;
            }
        }
        else
        {
            if(file->open(openMode))
            {
                device = file;
            }
        }
    }
    return device;
}


/**
 * Standard exporter for everything, which implements the Serializable interface.
 *
 * \param object   The object, which shall be serialized.
 * \param filename The filename, where the object shall be stored.
 * \param compress If true, the file will be read using the GZip decompressor.
 * \return True, if the storage of the object was successful.
 */
bool Impex::save(Serializable * object, const QString & filename, bool compress)
{
	QIODevice* device = Impex::openFile(filename, QIODevice::WriteOnly);
    
	if (device != NULL)
    {
        QXmlStreamWriter xmlWriter(device);
        object->serialize(xmlWriter);
		device->close();
        return true;
    }
	return false;
}

}//end of namespace graipe

/**
 * @}
 */
