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
#include "core/globals.hxx"

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
 * Basic import procedure for all types, which implements the serializable interface.
 *
 * \param filename The filename of the stored object.
 * \param object   The object, which shall be deserialized.
 * \param compress If true, the file will be read using the GZip decompressor.
 * \return True, if the loading of the object was successful.
 */
Model* Impex::loadModel(const QString & filename)
{
    bool compress =  (filename.right(2) == "gz");
    
    Model* model = NULL;
    
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
                model = loadModel(xmlReader);
                compressor.close();
            }
        }
        else
        {
            QXmlStreamReader xmlReader(&file);
            
            if(file.open(QIODevice::ReadOnly))
            {
                model = loadModel(xmlReader);
                file.close();
            }
        }
    }
    return model;
}
/**
 * Basic import procedure for all types, which implements the serializable interface.
 *
 * \param filename The filename of the stored object.
 * \param object   The object, which shall be deserialized.
 * \param compress If true, the file will be read using the GZip decompressor.
 * \return True, if the loading of the object was successful.
 */
Model* Impex::loadModel(QXmlStreamReader& xmlReader)
{
    //1. Read the name of the xml root
    if(xmlReader.readNextStartElement())
    {
        //First start element: ViewController's name
        QString mod_type = xmlReader.name().toString();

        //3. Create a model using the mod_type and the modelFactory:
        Model* model = NULL;
        
        for(unsigned int i=0; i<modelFactory.size(); ++i)
        {
            if(modelFactory[i].model_type==mod_type)
            {
                model = modelFactory[i].model_fptr();
                break;
            }
        }
        
        //  If it was not found: Indicate error
        if(model == NULL)
        {
            qWarning("Model type was not found in modelFactory.");
            return NULL;
        }
        
        if(model->deserialize(xmlReader))
        {
            return model;
        }
        else
        {
            qWarning("Deserialization of Model failed");
            delete model;
            return NULL;
        }
    }
    else
    {
        qWarning("Could not find a single XML start element!");
        return NULL;
    }
    return NULL;
}

/**
 * Basic import procedure for all types, which implements the serializable interface.
 *
 * \param filename The filename of the stored object.
 * \param object   The object, which shall be deserialized.
 * \param compress If true, the file will be read using the GZip decompressor.
 * \return True, if the loading of the object was successful.
 */
ViewController* Impex::loadViewController(const QString & filename, QGraphicsScene* scene)
{
    bool compress = (filename.right(2) == "gz");
    ViewController * vc = NULL;
    
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
                vc = loadViewController(xmlReader, scene);
                compressor.close();
            }
        }
        else
        {
            QXmlStreamReader xmlReader(&file);
            
            if(file.open(QIODevice::ReadOnly))
            {
                vc = loadViewController(xmlReader, scene);
                file.close();
            }
        }
    }
    
    return vc;
}


/**
 * Basic import procedure of a settings dictionary from a given QString
 * A dictionary is defined by means of a mapping from QString keys
 * to QString values.
 *
 * \param contents The input QString.
 * \param separator The seaparator, which will be used to split the key/value pairs, default is ": "
 */
ViewController* Impex::loadViewController(QXmlStreamReader & xmlReader, QGraphicsScene* scene)
{
    ViewController * vc = NULL;

    //1. Read the root attributes of the xml node as well as the name of the root
    if(     xmlReader.readNextStartElement()
        &&  xmlReader.attributes().hasAttribute("ModelID")
        &&  xmlReader.attributes().hasAttribute("ZOrder"))
    {
        //First start element: ViewController's name
        QString vc_type = xmlReader.name().toString();
        QString vc_modelID = xmlReader.attributes().value("ModelID").toString();
        int vc_zorder = xmlReader.attributes().value("ZOrder").toInt();

        //2. Find the associated model at the model_list (if it was already loaded)
        Model* vc_model = NULL;
        for(Model* mod : models)
        {
            if(     mod
                &&  mod->filename() == vc_modelID)
            {
                vc_model = mod;
                break;
            }
        }
        //  If it was not found: Indicate error
        if(vc_model == NULL)
        {
            qWarning("Model was not found among available ones.");
            return NULL;
        }
        
         //3. Create a controller using the vc_type and the model found above:
        for(unsigned int i=0; i<viewControllerFactory.size(); ++i)
        {
            if(viewControllerFactory[i].viewController_name==vc_type)
            {
                vc = viewControllerFactory[i].viewController_fptr(scene, vc_model, vc_zorder);
                break;
            }
        }
        //  If it was not found: Indicate error
        if(vc == NULL)
        {
            qWarning("ViewController type was not found in the factory.");
            return NULL;
        }
        
        //4. Restore the parameters
        if(vc->deserialize(xmlReader))
        {
            return vc;
        }
        else
        {
            qWarning("Deserialization of ViewController failed");
            delete vc;
            return NULL;
        }
    }
    else
    {
        qWarning("Could not find a single XML start element!");
        return NULL;
    }
    return NULL;
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
