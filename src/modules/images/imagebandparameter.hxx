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

#ifndef GRAIPE_IMAGES_IMAGEPARAMETER_HXX
#define GRAIPE_IMAGES_IMAGEPARAMETER_HXX

#include "images/config.hxx"
#include "images/image.hxx"
#include "core/parameters.hxx"

namespace graipe {

/**
 * This class defines the ImageBandParameterBase class.
 * It inherits from the Parameter base class to:
 * - provide selection facilities by means of a QComboBox for the image and 
 * - a spinBox for the band selection of the image.
 */
class GRAIPE_IMAGES_EXPORT ImageBandParameterBase
    : public Parameter
{
    Q_OBJECT
    
    public:
        /**
         * Default constructor of the ImageBandParameter class:
         *
         * \param name           The name (label) of this parameter.
         * \param parent         If given (!= NULL), this parameter has a parent and will
         *                       be enabled/disabled, if the parent is a BoolParameter.
         * \param invert_parent  If true, the enables/disabled dependency to the parent will be swapped.
         */
        ImageBandParameterBase(QString name, Parameter* parent, bool invert_parent, Environment* env);
            
        /**
         * The destructor of the ImageBandParameter class
         */
        ~ImageBandParameterBase();
    
        
        /**
         * The (immutable) type name of this parameter class.
         *
         * \return "ImageBandParameterBase".
         */
        QString typeName() const
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
        QWidget * delegate();

    public slots:
        /**
         * This slot is called everytime, the delegate has changed. It has to synchronize
         * the internal value of the parameter with the current delegate's value
         */
        void updateImage();
        /**
         * This slot is called everytime, the delegate has changed. It has to synchronize
         * the internal value of the parameter with the current delegate's value
         */
        void updateBandId();
	
    protected:
        /**
         * Initializes the connections (signal<->slot) between the parameter class and
         * the delegate widget. This will be done after the first call of the delegate()
         * function, since the delegate is NULL until then.
         */
        void initConnections();
    
        /**
         * Update handler, to support templated classes for each image band type, see below.
         */
        virtual void handleUpdateImage() = 0;
        /**
         * Update handler, to support templated classes for each image band type, see below.
         */
        virtual void handleUpdateBandId() = 0;
    
        //The parent delegate
        QWidget* m_delegate;
    
        //The delegate to select the image
        QComboBox* m_cmbImage;
    
        //The delegate to the select the image's band
        QSpinBox* m_spbBand;
};




/**
 * This class defines the ImageBandParameter class.
 * It inherits from the ImageBandParameterBase class  and templates it
 * to a pixel type T to hold an ImageBand<T> pointer.
 */
template <class T>
class GRAIPE_IMAGES_EXPORT ImageBandParameter
    : public ImageBandParameterBase
{
    public:
        /**
         * Default constructor of the ImageBandParameter class:
         *
         * \param name           The name (label) of this parameter.
         * \param parent         If given (!= NULL), this parameter has a parent and will
         *                       be enabled/disabled, if the parent is a BoolParameter.
         * \param invert_parent  If true, the enables/disabled dependency to the parent will be swapped.
         */
        ImageBandParameter(QString name, Parameter* parent, bool invert_parent, Environment* env);
            
        /**
         * The destructor of the ImageBandParameter class
         */
        ~ImageBandParameter();
    
        
        /**
         * The (immutable) type name of this parameter class.
         *
         * \return "ImageBandParameter".
         */
        QString typeName() const
        {
            return "ImageBandParameter";
        }
    
        /**
         * The current value of this parameter in the correct, most special type.
         *
         * \return The value of this parameter.
         */
		Image<T>* image() const;
    
        /**
         * The current value of this parameter in the correct, most special type.
         *
         * \return The value of this parameter.
         */
		unsigned int bandId() const;
    
        /**
         * The current const value of this parameter in the correct, most special type.
         *
         * \return The const value of this parameter.
         */
		const vigra::MultiArrayView<2,T>& value() const;
    
        /**
         * Writing accessor of the current value of this parameter.
         *
         * \param value The new value of this parameter.
         */
        void setImage(Image<T> * image);
    
        /**
         * Writing accessor of the current value of this parameter.
         *
         * \param value The new value of this parameter.
         */
        void setBandId(unsigned int bandid);
    
        /**
         * The value converted to a string. Please note, that this can vary from the 
         * serialize() result, which also returns a string. This is due to the fact,
         * that serialize also may perform encoding of strings to avoid special chars
         * inside the string.
         *
         * \return The value of the parameter converted to an QString.
         */
        QString toString() const;
    
	    /**
         * Serialization of the parameter's state to a string. Please note, that this can 
         * vary from the toString() result, which also returns a string. This is due to the fact,
         * that serialize also may perform encoding of strings to avoid special chars.
         *
         * \return The serialization of the parameter's state.
         */
        void serialize(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization of a parameter's state from a string.
         *
         * \param str The serialization of this parameter's state.
         * \return True, if the deserialization was successful, else false.
         */
        bool deserialize(QXmlStreamReader& xmlReader);
    
        /**
         * This function locks the parameters value. 
         * This means, that after a lock() call, only const acess to the parameter is 
         * possible until someone unlocks it. 
         * To work properly, the inner parameter class has to be designed accordingly.
         * As an example, you may look at the Model class, which supports locking and
         * unlocking - so do the parameter classes based on models!
         */
        virtual void lock();
    
        /**
         * This function unlocks the parameters value.
         * This means, that after a lock() call, only const acess to the parameter is 
         * possible until someone unlocks it. 
         * To work properly, the inner parameter class has to be designed accordingly.
         * As an example, you may look at the Model class, which supports locking and
         * unlocking - so do the parameter classes based on models!
         */
        virtual void unlock();
    
        /**
         * This function indicates whether the value of a parameter is valid or not.
         *
         * \return True, if the parameter's value is valid.
         */
        virtual bool isValid() const;
    
        /**
         * This function indicates whether the value of a parameter is a Model* or 
         * many of them or needs one at least. These parameters need to access the
         * global 'models' variable, too!
         *
         * \return A filled vector, if the parameter's value is related to a Model*.
         *         An empty vector by default.
         */
        std::vector<Model*> needsModels() const;
        
    protected:
        /**
         * Initializes the connections (signal<->slot) between the parameter class and
         * the delegate widget. This will be done after the first call of the delegate()
         * function, since the delegate is NULL until then.
         */
        void initConnections();
        
        /**
         * This slot is called everytime, the delegate has changed. It has to synchronize
         * the internal value of the parameter with the current delegate's value
         */
        void handleUpdateImage();
    
        /**
         * This slot is called everytime, the bandID delegate has changed.
         * It has to synchronize the internal value of the parameter with the current delegate's value
         */
        void handleUpdateBandId();
    
        //The list of all available images (currently)
        std::vector<Image<T>*>	m_allowed_images;
    
        Image<T> * m_image;
        unsigned int m_bandId;
	
        //The lock (if locked)
        unsigned int m_lock;
        
        vigra::MultiArray<2,T> m_empty_image;
};

} //end of namespace graipe
#endif
