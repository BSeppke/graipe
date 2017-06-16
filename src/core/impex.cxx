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
#include "core/environment.hxx"

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
 * Basic import procedure for all types, which implements the serializable interface.
 *
 * \param filename The filename of the stored object.
 * \param object   The object, which shall be deserialized.
 * \param compress If true, the file will be read using the GZip decompressor.
 * \return True, if the loading of the object was successful.
 */
Model* Impex::loadModel(const QString & filename, Environment* env)
{
   QIODevice* device = Impex::openFile(filename, QIODevice::ReadOnly);
   Model* model = NULL;

    if(device != NULL)
    {
        QXmlStreamReader xmlReader(device);
        model = loadModel(xmlReader, env);
        device->close();
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
Model* Impex::loadModel(QXmlStreamReader& xmlReader, Environment* env)
{
    //1. Read the name of the xml root
    if(xmlReader.readNextStartElement())
    {
        //First start element: ViewController's name
        QString mod_type = xmlReader.name().toString();

        //3. Create a model using the mod_type and the modelFactory:
        Model* model = NULL;
        
        for(unsigned int i=0; i<env->modelFactory.size(); ++i)
        {
            if(env->modelFactory[i].model_type==mod_type)
            {
                model = env->modelFactory[i].model_fptr(env);
                break;
            }
        }
        
        //  If it was not found: Indicate error
        if(model == NULL)
        {
            qWarning("Impex::loadModel: Model type was not found in modelFactory.");
            return NULL;
        }
        
        if(model->deserialize(xmlReader))
        {
            return model;
        }
        else
        {
            qWarning("Impex::loadModel: Deserialization of Model failed");
            delete model;
            return NULL;
        }
    }
    else
    {
        qWarning("Impex::loadModel: Could not find a single XML start element!");
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
ViewController* Impex::loadViewController(const QString & filename, QGraphicsScene* scene, Environment* env)
{
    QIODevice* device = Impex::openFile(filename, QIODevice::ReadOnly);
    ViewController * vc = NULL;
    
    if(device != NULL)
    {
        QXmlStreamReader xmlReader(device);
        vc = loadViewController(xmlReader, scene, env);
        device->close();
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
ViewController* Impex::loadViewController(QXmlStreamReader & xmlReader, QGraphicsScene* scene, Environment* env)
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
        for(Model* mod : env->models)
        {
            if(     mod
                &&  mod->id() == vc_modelID)
            {
                vc_model = mod;
                break;
            }
        }
        //  If it was not found: Indicate error
        if(vc_model == NULL)
        {
            qWarning("Impex::loadViewController: Model was not found among available ones.");
            return NULL;
        }
        
         //3. Create a controller using the vc_type and the model found above:
        for(unsigned int i=0; i<env->viewControllerFactory.size(); ++i)
        {
            if(env->viewControllerFactory[i].viewController_name==vc_type)
            {
                vc = env->viewControllerFactory[i].viewController_fptr(scene, vc_model, vc_zorder);
                break;
            }
        }
        //  If it was not found: Indicate error
        if(vc == NULL)
        {
            qWarning("Impex::loadViewController: ViewController type was not found in the factory.");
            return NULL;
        }
        
        //4. Restore the parameters
        if(vc->deserialize(xmlReader))
        {
            return vc;
        }
        else
        {
            qWarning("Impex::loadViewController: Deserialization of ViewController failed");
            delete vc;
            return NULL;
        }
    }
    else
    {
        qWarning("Impex::loadViewController: Could not find a single XML start element!");
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
Algorithm* Impex::loadAlgorithm(const QString & filename, Environment* env)
{
    QIODevice* device = Impex::openFile(filename, QIODevice::ReadOnly);
    Algorithm * alg = NULL;
    
    if(device != NULL)
    {
        QXmlStreamReader xmlReader(device);
        alg = loadAlgorithm(xmlReader, env);
        device->close();
    }
    
    return alg;
}


/**
 * Basic import procedure of a settings dictionary from a given QString
 * A dictionary is defined by means of a mapping from QString keys
 * to QString values.
 *
 * \param contents The input QString.
 * \param separator The seaparator, which will be used to split the key/value pairs, default is ": "
 */
Algorithm* Impex::loadAlgorithm(QXmlStreamReader & xmlReader, Environment* env)
{
    Algorithm * alg = NULL;

    //1. Read the root attributes of the xml node as well as the name of the root
    if(     xmlReader.readNextStartElement()
        &&  xmlReader.attributes().hasAttribute("ID"))
    {
        //First start element: ViewController's name
        QString alg_type = xmlReader.name().toString();

        //Create an algorithm using the alg_type:
        for(unsigned int i=0; i<env->algorithmFactory.size(); ++i)
        {
            if(env->algorithmFactory[i].algorithm_type==alg_type)
            {
                alg = env->algorithmFactory[i].algorithm_fptr(env);
                break;
            }
        }
        //If it was not found: Indicate error
        if(alg == NULL)
        {
            qWarning("Impex::loadAlgorithm: Algorithm was not found among available ones.");
            return NULL;
        }
        
        //4. Restore the parameters
        if(alg->deserialize(xmlReader))
        {
            return alg;
        }
        else
        {
            qWarning("Impex::loadAlgorithm: Deserialization of Algorithm failed");
            delete alg;
            return NULL;
        }
    }
    else
    {
        qWarning("Impex::loadAlgorithm: Could not find a single XML start element!");
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
