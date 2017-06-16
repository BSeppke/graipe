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

#ifndef GRAIPE_CORE_PARAMETERS_MULTIMODELPARAMETER_HXX
#define GRAIPE_CORE_PARAMETERS_MULTIMODELPARAMETER_HXX

#include "core/parameters/parameter.hxx"
#include "core/model.hxx"

#include <QPointer>
#include <QListWidget>

/**
 * @file
 * @brief Header file for the MultiModelParameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * This is the MultiModelParameter class.
 * It inherits from the Parameter base class to:
 * - hold an std::vector of Model pointers, and
 * - provide selection facilities by means of a QListWidget.
 * It also provides easy filtering of given Models by their typeName().
 */
class GRAIPE_CORE_EXPORT MultiModelParameter
:   public Parameter
{
    Q_OBJECT
    
    public:
        /**
         * Default constructor of the MultiModelParameter class with a setting of the
         * most important values directly.
         *
         * \param name           The name (label) of this parameter.
         * \param allowed_models A vector containing all currently available models.
         * \param type_filter    A QString to restrict the model list to certain model types.
         * \param parent         If given (!= NULL), this parameter has a parent and will
         *                       be enabled/disabled, if the parent is a BoolParameter.
         * \param invert_parent  If true, the enables/disabled dependency to the parent will be swapped.
         */
        MultiModelParameter(const QString& name, QString type_filter, Parameter* parent, bool invert_parent, Environment* env);
    
        /**
         * Destructor of the MultiModel class
         */
        ~MultiModelParameter();
    
        /**
         * The (immutable) type name of this parameter class.
         *
         * \return "MultiModelParameter".
         */
        QString typeName() const
        {
            return "MultiModelParameter";
        }
        
        /** 
         * The current value of this parameter in the correct, most special type.
         *
         * \return The value of this parameter.
         */
        std::vector<Model*> value() const;
    
        /**
         * Writing accessor of the current value of this parameter.
         *
         * \param value The new value of this parameter.
         */
        void setValue(const std::vector<Model*>& value);
            
        /**
         * The value converted to a QString. Please note, that this can vary from the 
         * serialize() result, which also returns a QString. This is due to the fact,
         * that serialize also may perform encoding of QStrings to avoid special chars
         * inside the QString.
         *
         * \return The value of the parameter converted to an QString.
         */
        QString toString() const;
            
        /**
         * Serialization of the parameter's state to a xml stream.
         * Writes the following XML code by default:
         * 
         * <MultiModelParameter>
         *     <Name>NAME</Name>
         *     <Values>N</Value>
         *     <Value ID="0">VALUE_0_ID</Value>
         *     ...
         *     <Value ID="N-1">VALUE_N-1_ID</Value>
         * </MultiModelParameter>
         *
         * with     NAME = name(),
         *             N = QString::number(value().size()), and
         *    VALUE_0_ID = values()[0]->id().
         *
         * \param xmlWriter The QXMLStreamWriter, which we use serialize the 
         *                  parameter's type, name and value.
         */
        void serialize(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization of a parameter's state from an xml file.
         *
         * \param xmlReader The QXmlStreamReader, where we read from.
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
        void updateValue();
        
    protected:
        /** The storage for the value of this parameter (list of idxs in m_allowed_models) **/
        std::vector<int> m_model_idxs;
    
        /** The delegate list widget **/
        QPointer<QListWidget> m_delegate;
    
        /** A vector of all allowed models (listed) **/
        std::vector<Model*>	m_allowed_values;
    
        /** The currently used type-filter **/
        QString m_type_filter;
    
        /** The currently active locks on the selected models **/
        std::vector<unsigned int> m_locks;
};

} //end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_PARAMETERS_MULTIMODELPARAMETER_HXX
