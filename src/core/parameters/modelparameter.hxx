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

#ifndef GRAIPE_CORE_PARAMETERS_MODELPARAMETER_HXX
#define GRAIPE_CORE_PARAMETERS_MODELPARAMETER_HXX

#include "core/parameters/parameter.hxx"
#include "core/model.hxx"

#include <QComboBox>


/**
 * This file defines the ModelParameter class.
 * It inherits from the Parameter base class to:
 * - hold a Model pointer, and
 * - provide selection facilities by means of a QComboBox.
 * It also provides easy filtering of given Models by their typeName().
 */


namespace graipe {

class GRAIPE_CORE_EXPORT ModelParameter
:   public Parameter
{	
    Q_OBJECT
    
    public:
        /**
         * Default constructor of the ModelParameter class with a setting of the
         * most important values directly.
         *
         * \param name           The name (label) of this parameter.
         * \param allowed_models A vector containing all currently available models.
         * \param type_filter    A QString to restrict the model list to certain model types.
         * \param value          The initial value of this parameter.
         * \param parent         If given (!= NULL), this parameter has a parent and will
         *                       be enabled/disabled, if the parent is a BoolParameter.
         * \param invert_parent  If true, the enables/disabled dependency to the parent will be swapped.
         */
        ModelParameter(const QString& name, const std::vector<Model*> * allowed_models, QString type_filter="", Model* value=NULL, Parameter* parent=NULL, bool invert_parent=false);
    
        /**
         * The destructor of the ModelParameter class
         */
        ~ModelParameter();
    
        /**
         * The (immutable) type name of this parameter class.
         *
         * \return "ModelParameter".
         */
        QString typeName() const;
        
        /** 
         * The current value of this parameter in the correct, most special type.
         *
         * \return The value of this parameter.
         */
        Model* value() const;
    
        /**
         * Writing accessor of the current value of this parameter.
         *
         * \param value The new value of this parameter.
         */
        void setValue(Model* value);
            
        /**
         * The value converted to a QString. Please note, that this can vary from the 
         * serialize() result, which also returns a QString. This is due to the fact,
         * that serialize also may perform encoding of QStrings to avoid special chars
         * inside the QString.
         *
         * \return The value of the parameter converted to an QString.
         */
        QString valueText() const;
    
        /**
         * This method is called after each (re-)assignment of the model list
         * e.g. after a call of the setModelList() function. 
         * It synchronizes the list of available models with the widget's list.
         */
        void refresh();
        
        /**
         * Serialization of the parameter's state to a QString. Please note, that this can 
         * vary from the valueText() result, which also returns a QString. This is due to the fact,
         * that serialize also may perform encoding of QStrings to avoid special chars.
         *
         * \return The serialization of the parameter's state.
         */
        void serialize(QIODevice& out) const;
    
        /**
         * Deserialization of a parameter's state from a QString.
         *
         * \param str The serialization of this parameter's state.
         * \return True, if the deserialization was successful, else false.
         */
        bool deserialize(QIODevice& out);
    
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
         * The delegate widget of this parameter. 
         * Each parameter generates such a widget on demand, which refers to the
         * first call of this function. This is needed due to the executability of
         * classes using parameters (like the Algorithm class) in different threads.
         *
         * \return The delegate widget to control the values of this parameter.
         */
        QWidget * delegate();
        
    protected slots:
        /**
         * This slot is called everytime, the delegate has changed. It has to synchronize
         * the internal value of the parameter with the current delegate's value
         */
        void updateValue();
  
    protected:
        /**
         * Initializes the connections (signal<->slot) between the parameter class and
         * the delegate widget. This will be done after the first call of the delegate()
         * function, since the delegate is NULL until then.
         */
        void initConnections();
    
        //The model's delegate widget
        QComboBox* m_cmbDelegate;
    
        //The allowed model pointers
        std::vector<Model*>	m_allowed_values;
    
        //A type filter to show only a subset of models (filtered by their typeName())
        QString m_type_filter;
    
        //The currently selected model
        Model* m_value;
    
        //The lock (if locked)
        unsigned int m_lock;
};

} //end of namespace graipe

#endif //GRAIPE_CORE_PARAMETERS_MODELPARAMETER_HXX
