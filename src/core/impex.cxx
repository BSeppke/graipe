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

#include <QFile>
#include "core/qt_ext/qiocompressor.hxx"

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
 * Basic import procedure for all types, which implements the serializable interface.
 *
 * \param filename The filename of the stored object.
 * \param object   The object, which shall be deserialized.
 * \param compress If true, the file will be read using the GZip decompressor.
 * \return True, if the loading of the object was successful.
 */
bool Impex::load(const QString & filename, Serializable * object, bool compress)
{
    bool success=false;
    
    if(!filename.isEmpty())
    {
        QFile file(filename);
        
        if(compress)
        {
            QIOCompressor compressor(&file);
            compressor.setStreamFormat(QIOCompressor::GzipFormat);
            
            QXmlStreamReader xmlReader(&compressor);
            
            if (compressor.open(QIODevice::ReadOnly))
            {
                object->setFilename(filename);
                success = object->deserialize(xmlReader);
                compressor.close();
            }
        }
        else
        {
            QXmlStreamReader xmlReader(&file);
            
            if(file.open(QIODevice::ReadOnly))
            {
                object->setFilename(filename);
                success = object->deserialize(xmlReader);
                file.close();
            }
        }
    }
    
    return success;
}


/**
 * Basic import procedure of a settings dictionary from a file.
 * A dictionary is defined by means of a mapping from QString keys
 * to QString values.
 *
 * \param filename The filename to be read.
 * \param compress If true, the file will be read using the GZip decompressor.
 */
std::map<QString,QString> Impex::dictFromFile(const QString & filename, bool compress)
{
    std::map<QString,QString> result;
    
    if(!filename.isEmpty())
    {
        QFile file(filename);
        
        if(compress)
        {
            QIOCompressor compressor(&file);
            compressor.setStreamFormat(QIOCompressor::GzipFormat);
            
            QXmlStreamReader xmlReader(&compressor);
            
            if (compressor.open(QIODevice::ReadOnly))
            {
                result = dictFromStream(xmlReader);
                compressor.close();
            }
            else
            {
                compressor.close();
            }
        }
        else
        {
            QXmlStreamReader xmlReader(&file);
            
            if(file.open(QIODevice::ReadOnly))
            {
                result = dictFromStream(xmlReader);
                file.close();
            }
            else
            {
                file.close();
            }
        }
    }
    return result;
}

/**
 * Basic import procedure of a settings dictionary from a given QString
 * A dictionary is defined by means of a mapping from QString keys
 * to QString values.
 *
 * \param contents The input QString.
 * \param separator The seaparator, which will be used to split the key/value pairs, default is ": "
 */
std::map<QString,QString> Impex::dictFromStream(QXmlStreamReader & xmlReader)
{
    std::map<QString,QString> result;
    
    if(xmlReader.readNextStartElement())
    {
        qDebug() << "Impex::dictFromStream readFirstStartElement" << xmlReader.name().toString();
        //First start element: ViewController's name
        result.insert(std::pair<QString,QString>("Type", xmlReader.name().toString()));
        
        for(QXmlStreamAttribute attr : xmlReader.attributes())
        {
            result.insert(std::pair<QString,QString>(attr.name().toString(), attr.value().toString()));
        }
    }
    else
    {
        qWarning("Could not find a single XML start element!");
    }
    return result;
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
    bool success = false;
	
    if(!filename.isEmpty())
	{
		QFile file(filename);
		
        object->setFilename(filename);
        
		if(compress)
		{
			QIOCompressor compressor(&file);
			compressor.setStreamFormat(QIOCompressor::GzipFormat);
		
            QXmlStreamWriter xmlWriter(&compressor);
            
			if (compressor.open(QIODevice::WriteOnly))
			{
                object->serialize(xmlWriter);
				compressor.close();
				
				success = true;
			}
		}
		else if(file.open(QIODevice::WriteOnly))
		{
            QXmlStreamWriter xmlWriter(&file);
            object->serialize(xmlWriter);
			file.close();
				
			success = true;
		}
	}
    
    if(success && object)
    {
    }
	return success;
}

}//end of namespace graipe

/**
 * @}
 */
