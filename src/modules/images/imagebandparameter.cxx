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

#include "images/imagebandparameter.hxx"

namespace graipe {

/**
 * Default constructor of the ImageBandParameter class:
 *
 * \param name           The name (label) of this parameter.
 * \param parent         If given (!= NULL), this parameter has a parent and will
 *                       be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent  If true, the enables/disabled dependency to the parent will be swapped.
 */
ImageBandParameterBase::ImageBandParameterBase(QString name, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_delegate(NULL),
	m_cmbImage(NULL),
	m_spbBand(NULL)
{
}

/**
 * The destructor of the ImageBandParameterBase class
 */
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

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "ImageBandParameterBase".
 */
QString ImageBandParameterBase::typeName() const
{
	return "ImageBandParameterBase";
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
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
        
		refresh();
        initConnections();
    }
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void ImageBandParameterBase::updateImage()
{
    handleUpdateImage();
}
    
/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void ImageBandParameterBase::updateBandId()
{
    handleUpdateBandId();
}

/**
 * Initializes the connections (signal<->slot) between the parameter class and
 * the delegate widget. This will be done after the first call of the delegate()
 * function, since the delegate is NULL until then.
 */
void ImageBandParameterBase::initConnections()
{
    connect(m_cmbImage, SIGNAL(currentIndexChanged(int)), this, SLOT(updateImage()));
    connect(m_spbBand,  SIGNAL(valueChanged(int)),        this, SLOT(updateBandId()));
    
    Parameter::initConnections();
}




/**
 * Default constructor of the ImageBandParameter class:
 *
 * \param name           The name (label) of this parameter.
 * \param parent         If given (!= NULL), this parameter has a parent and will
 *                       be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent  If true, the enables/disabled dependency to the parent will be swapped.
 */
template <class T>
ImageBandParameter<T>::ImageBandParameter(QString name, Parameter* parent, bool invert_parent)
:	ImageBandParameterBase(name, parent, invert_parent),
    m_image(NULL),
    m_bandId(0)
{
}

/**
 * The destructor of the ImageBandParameter class
 */
template <class T>
ImageBandParameter<T>::~ImageBandParameter()
{
    //Widgets will be destroyed in parent (base) class
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return Image<T>::typeName() + "BandParameter".
 */
template <class T>
QString ImageBandParameter<T>::typeName() const
{
    Image<T> temp;
	return temp.typeName() + "BandParameter";
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
template <class T>
Image<T>* ImageBandParameter<T>::image() const
{
    return m_image;
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
template <class T>
unsigned int ImageBandParameter<T>::bandId() const
{
    return m_bandId;
}
    
/**
 * The current const value of this parameter in the correct, most special type.
 *
 * \return The const value of this parameter.
 */
template <class T>
const vigra::MultiArrayView<2,T>& ImageBandParameter<T>::value() const
{
    if(m_image != NULL && m_bandId < m_image->numBands())
    {
        return m_image->band(m_bandId);
    }
    return m_empty_image;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
template <class T>
void ImageBandParameter<T>::setImage(Image<T> * image)
{
    for (int i=0; i<m_allowed_images.size(); ++i)
    {
        if (m_allowed_images[i] == image)
        {
            m_image = image;
            m_cmbImage->setCurrentIndex(i);
        }
    }
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
template <class T>
void ImageBandParameter<T>::setBandId(unsigned int bandid)
{
    if (m_image != NULL && bandid < m_image->numBands())
    {
        m_bandId = bandid;
    }
}


/**
 * The value converted to a string. Please note, that this can vary from the 
 * serialize() result, which also returns a string. This is due to the fact,
 * that serialize also may perform encoding of strings to avoid special chars
 * inside the string.
 *
 * \return The value of the parameter converted to an QString.
 */
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

    
/**
 * This method is called after each (re-)assignment of the model list
 * e.g. after a call of the setModelList() function. 
 * It synchronizes the list of available models with the widget's list.
 */
template <class T>
void ImageBandParameter<T>::refresh()
{
    if(m_modelList && m_cmbImage)
	{
		m_allowed_images.clear();
		m_cmbImage->clear();
		
        Image<T> temp;
		
        for(Model* model : *m_modelList)
		{
        	if(model->typeName() == temp.typeName())
			{
				m_cmbImage->addItem(model->shortName());
				m_cmbImage->setItemData(m_cmbImage->count()-1, model->description(), Qt::ToolTipRole);
				m_allowed_images.push_back(static_cast<Image<T>*>(model));
			}
		}
        
        if(m_allowed_images.size())
        {
			m_cmbImage->setCurrentIndex(0);
            updateImage();
        }
	}
}

/**
 * Serialization of the parameter's state to a string. Please note, that this can 
 * vary from the toString() result, which also returns a string. This is due to the fact,
 * that serialize also may perform encoding of strings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
template <class T>
void ImageBandParameter<T>::serialize(QXmlStreamWriter& xmlWriter) const
{    
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(magicID());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("Filename", m_image->filename());
    xmlWriter.writeTextElement("BandID", QString::number(m_bandId));
    xmlWriter.writeEndElement();
}

/**
 * Deserialization of a parameter's state from a string.
 *
 * \param str The serialization of this parameter's state.
 * \return True, if the deserialization was successful, else false.
 */
template <class T>
bool ImageBandParameter<T>::deserialize(QXmlStreamReader& xmlReader)
{
//TODO!!!
/**
    if (!Parameter::deserialize(in))
    {
        return false;
    }
    
    bool found = false;
    unsigned int i=0;
    
    QString content(in.readLine());
    
    QStringList fname_bandId = split_string_once(content, ", ");
    
    if(fname_bandId.size() ==2)
    {
        QString filename = decode_string(fname_bandId[0]);
        unsigned int bandId = fname_bandId[1].toUInt();
    
        for(Image<T>* image: m_allowed_images)
        {
            if(image->filename() == filename && image->numBands() > bandId)
            {
                m_image = image;
                m_bandId = bandId;
                
                if(m_cmbImage != NULL)
                {
                    m_cmbImage->setCurrentIndex(i);
                    m_spbBand->setValue(bandId);
                }
                found = true;
            }
            i++;
        }
        
        if (!found)
        {
            qDebug() << "ImageBandParameter deserialize: filename does not match any given or bandId was erroneous! Was:'" << content << "'";
        }
        
        return found;
    }
    */
    return false;
}

/**
 * This function locks the parameters value. 
 * This means, that after a lock() call, only const acess to the parameter is 
 * possible until someone unlocks it. 
 * To work properly, the inner parameter class has to be designed accordingly.
 * As an example, you may look at the Model class, which supports locking and
 * unlocking - so do the parameter classes based on models!
 */
template <class T>
void ImageBandParameter<T>::lock()
{	
    if(m_image)
    {
        m_lock = m_image->lock();
    }
}

/**
 * This function unlocks the parameters value.
 * This means, that after a lock() call, only const acess to the parameter is 
 * possible until someone unlocks it. 
 * To work properly, the inner parameter class has to be designed accordingly.
 * As an example, you may look at the Model class, which supports locking and
 * unlocking - so do the parameter classes based on models!
 */
template <class T>
void ImageBandParameter<T>::unlock()
{
    if(m_image)
    {
        m_image->unlock(m_lock);
    }
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
template <class T>
bool ImageBandParameter<T>::isValid() const
{
	return	(m_image != NULL) && (m_bandId < m_image->numBands());
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
template <class T>
void ImageBandParameter<T>::handleUpdateImage()
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

/**
 * This slot is called everytime, the bandID delegate has changed. 
 * It has to synchronize the internal value of the parameter with the current delegate's value
 */
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

} //end of namespace graipe
