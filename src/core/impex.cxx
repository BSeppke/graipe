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
 * Reads the content of (maybe compressed) file to a QString.
 * Mainly needed for views and their serialization.
 *
 * \param filename The filename to be read.
 * \param compress If true, the file will be read using the GZip decompressor.
 */
QString Impex::readToString(const QString & filename, bool compress)
{
    QString result;
    
    if(!filename.isEmpty())
    {
        QFile file(filename);
        
        if(compress)
        {
            QIOCompressor compressor(&file);
            compressor.setStreamFormat(QIOCompressor::GzipFormat);
            
            if (compressor.open(QIODevice::ReadOnly))
            {
                const QByteArray text = compressor.readAll();
                result = QString(text);
                compressor.close();
            }
            else
            {
                compressor.close();
            }
        }
        else
        {
            if(file.open(QIODevice::ReadOnly))
            {
                const QByteArray text = file.readAll();
                result = QString(text);
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
 * Basic import procedure of a settings dictionary from a file.
 * A dictionary is defined by means of a mapping from QString keys
 * to QString values.
 *
 * \param filename The filename to be read.
 * \param compress If true, the file will be read using the GZip decompressor.
 */
std::map<QString,QString> Impex::dictFromFile(const QString & filename, bool compress)
{
    
    QString contents = Impex::readToString(filename, compress);
    return Impex::dictFromString(contents);
}

/**
 * Basic import procedure of a settings dictionary from a given QString
 * A dictionary is defined by means of a mapping from QString keys
 * to QString values.
 *
 * \param contents The input QString.
 * \param separator The seaparator, which will be used to split the key/value pairs, default is ": "
 */
std::map<QString,QString> Impex::dictFromString(const QString & contents, QString separator)
{
    std::map<QString,QString> result;
    QStringList lines = contents.split("\n");
    
    if (lines.size() !=0)
    {
        // [Graipe::XXXXXXXXXX]
        QString temp = lines[0];
        //remove [Graipe:: and the last char
        temp = temp.mid(9,temp.size()-10);
        
        result.insert(std::pair<QString,QString>("type", temp));
    
        for(int i=1; i<lines.size(); ++i)
        {
            //Important here: do not trim the QStrings!
            QStringList key_value =  split_string_once(lines[i], separator);
            
            if(key_value.size() == 2)
            {
                result.insert(std::pair<QString,QString>(key_value[0], decode_string(key_value[1])));
            }
        }
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
