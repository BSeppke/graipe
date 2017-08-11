/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2017 by Benjamin Seppke                 */
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

#include "images/imagebandparameter.hxx"
#include "core/workspace.hxx"

namespace graipe {

/**
 * @addtogroup graipe_images
 * @{
 *     @file
 *     @brief Implementation file for image band selection parameter class
 * @}
 */

ImageBandParameterBase::ImageBandParameterBase(QString name, Parameter* parent, bool invert_parent, Workspace* wsp)
:	Parameter(name, parent, invert_parent),
    m_delegate(NULL),
	m_cmbImage(NULL),
	m_spbBand(NULL)
{
}

ImageBandParameterBase::~ImageBandParameterBase()
{
    if(m_delegate != NULL)
    {
        //Also deletes other widget, since they are owned
        //by the assigned layout.
        delete m_delegate;
        m_delegate=NULL;
    }
}

QWidget* ImageBandParameterBase::delegate()
{
    if( m_delegate == NULL)
    {
        m_delegate = new QWidget;
        m_cmbImage = new QComboBox;
        m_spbBand  = new QSpinBox;
    
        QHBoxLayout *layout = new QHBoxLayout(m_delegate);
    
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(m_cmbImage);
        layout->addWidget(m_spbBand);
        
        initConnections();
    }
    return m_delegate;
}

void ImageBandParameterBase::updateImage()
{
    handleUpdateImage();
}

void ImageBandParameterBase::updateBandId()
{
    handleUpdateBandId();
}

void ImageBandParameterBase::initConnections()
{
    connect(m_cmbImage, SIGNAL(currentIndexChanged(int)), this, SLOT(updateImage()));
    connect(m_spbBand,  SIGNAL(valueChanged(int)),        this, SLOT(updateBandId()));
    
    Parameter::initConnections();
}











template <class T>
ImageBandParameter<T>::ImageBandParameter(QString name, Parameter* parent, bool invert_parent, Workspace* wsp)
:	ImageBandParameterBase(name, parent, invert_parent,wsp),
    m_image(NULL),
    m_bandId(0)
{
    if(wsp->models.size())
    {
        m_allowed_images.clear();
        
        Image<T> * img = new Image<T>(wsp);
        QString typeName = img->typeName();
        delete img;
        img=NULL;
        
        for(Model* model :wsp->models )
        {
            if(model->typeName() ==typeName)
            {
                m_allowed_images.push_back(static_cast<Image<T>*>(model));
            }
        }
            
        if(m_allowed_images.size())
        {
            m_image = m_allowed_images.front();
        }
    }
}

template <class T>
ImageBandParameter<T>::~ImageBandParameter()
{
    //Widgets will be destroyed in parent (base) class
}

template <class T>
Image<T>* ImageBandParameter<T>::image() const
{
    return m_image;
}

template <class T>
unsigned int ImageBandParameter<T>::bandId() const
{
    return m_bandId;
}

template <class T>
const vigra::MultiArrayView<2,T>& ImageBandParameter<T>::value() const
{
    if(m_image != NULL && m_bandId < m_image->numBands())
    {
        return m_image->band(m_bandId);
    }
    return m_empty_image;
}

template <class T>
void ImageBandParameter<T>::setImage(Image<T> * image)
{
    for (int i=0; i<m_allowed_images.size(); ++i)
    {
        if (m_allowed_images[i] == image)
        {
            m_image = image;
            
            if(m_delegate != NULL)
            {
                m_cmbImage->setCurrentIndex(i);
            }
        }
    }
}

template <class T>
void ImageBandParameter<T>::setBandId(unsigned int bandid)
{
    if (m_image != NULL && bandid < m_image->numBands())
    {
        m_bandId = bandid;
    }
}

template <class T>
QString ImageBandParameter<T>::toString() const
{
	if(!isValid())
	{
		return "invalid";
	}
	else
	{
		return QString("%1 (band %2)").arg(m_image->name()).arg(m_bandId);
	}
}

template <class T>
void ImageBandParameter<T>::serialize(QXmlStreamWriter& xmlWriter) const
{    
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeAttribute("ID", id());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("ImageID", m_image->id());
    xmlWriter.writeTextElement("BandID", QString::number(m_bandId));
    xmlWriter.writeEndElement();
}

template <class T>
bool ImageBandParameter<T>::deserialize(QXmlStreamReader& xmlReader)
{
    bool success = false;
    
    try
    {
        if(xmlReader.name() == typeName())
        {
            for(int i=0; i!=3; ++i)
            {
                xmlReader.readNextStartElement();
            
                if(xmlReader.name() == "Name")
                {
                    setName(xmlReader.readElementText());
                }
                if(xmlReader.name() == "ImageID")
                {
                    QString valueText =  xmlReader.readElementText();
                    
                    for(Image<T>* allowed: m_allowed_images)
                    {
                        if (allowed->id() == valueText)
                        {
                            setImage(allowed);
                            success = true;
                        }
                    }
                }
                if(xmlReader.name() == "BandID")
                {
                    QString valueText =  xmlReader.readElementText();
                    setBandId(valueText.toInt());
                }
            }
            while(true)
            {
                if(!xmlReader.readNext())
                {
                    return false;
                }
                
                if(xmlReader.isEndElement() && xmlReader.name() == typeName())
                {
                    break;
                }
            }
            return success;
        }
        else
        {
            throw std::runtime_error("Did not find typeName() or id() in XML tree");
        }
        throw std::runtime_error("Did not find any start element in XML tree");
    }
    catch(std::runtime_error & e)
    {
        qCritical() << typeName() << "::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
}

template <class T>
void ImageBandParameter<T>::lock()
{	
    if(m_image)
    {
        m_lock = m_image->lock();
    }
}

template <class T>
void ImageBandParameter<T>::unlock()
{
    if(m_image)
    {
        m_image->unlock(m_lock);
    }
}

template <class T>
bool ImageBandParameter<T>::isValid() const
{
	return	(m_image != NULL) && (m_bandId < m_image->numBands());
}

template <class T>
std::vector<Model*> ImageBandParameter<T>::needsModels() const
{
    std::vector<Model*> modelList;
    
    if(isValid())
    {
        modelList.push_back(image());
    }
    return modelList;
}

template <class T>
void ImageBandParameter<T>::initConnections()
{
    ImageBandParameterBase::initConnections();
    
    m_cmbImage->clear();
            
    for(Image<T>* image : m_allowed_images)
    {
        m_cmbImage->addItem(image->shortName());
        m_cmbImage->setItemData(m_cmbImage->count()-1, image->description(), Qt::ToolTipRole);
    }
    
    if(m_allowed_images.size() != 0)
    {
        m_cmbImage->setCurrentIndex(0);
    }
}

template <class T>
void ImageBandParameter<T>::handleUpdateImage()
{
    if(m_delegate != NULL)
    {
        int idx = m_cmbImage->currentIndex();
        
        if(idx>=0 && idx<(int)m_allowed_images.size())
        {
            Image<T>* image = static_cast<Image<T>*>(m_allowed_images[m_cmbImage->currentIndex()]);
        
            if(image && image->numBands() != 0)
            {
                m_image = image;
                
                m_spbBand->setMinimum(0);
                m_spbBand->setMaximum(image->numBands()-1);
            
                if((unsigned int)m_spbBand->value() > image->numBands()-1)
                {
                    m_spbBand->setValue(0);
                }
                else
                {
                    m_spbBand->setValue(m_spbBand->value());
                }
            }
        }
    }
}

template <class T>
void ImageBandParameter<T>::handleUpdateBandId()
{
    if(m_image != NULL)
    {
        if((unsigned int)m_spbBand->value() > m_image->numBands()-1)
        {
            m_bandId = 0;
        }
        else
        {
            m_bandId = m_spbBand->value();
        }
    }
}

//Provide the three mainly needed band-types
template class ImageBandParameter<float>;
template class ImageBandParameter<int>;
template class ImageBandParameter<unsigned char>;

/**
 * @}
 */

} //end of namespace graipe
