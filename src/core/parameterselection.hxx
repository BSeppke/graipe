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

#ifndef GRAIPE_CORE_PARAMETERSELECTION_HXX
#define GRAIPE_CORE_PARAMETERSELECTION_HXX

#include "core/config.hxx"
#include "core/parameters/parameter.hxx"
#include "core/algorithm.hxx"

#include <QPointer>
#include <QDialog>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <QScrollArea>

/**
 * @file
 * @brief This file defines the ParameterSelection classes, which are QDialog
 * wrappers for the most commonly used class.
 *
 * This is e.g. used to determine the actual parameters' values for each
 * algorithm run, for a new model creation or for single parameters.
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Class to select a value for a single parameter, which may also be a
 * parameter group of course!
 */
class GRAIPE_CORE_EXPORT ParameterSelection
 : public QDialog
{    
public:
    /**
     * Constructor of the parameter selection. This constructor
     * may be used to create a Dialog for a single or multiple Parameter
     * choices.
     * 
     * \param parent The parent widget, to make this selection modal.
     * \param param  The parameter, for which the selection shall be generated.
     */
    ParameterSelection(QWidget *parent, Parameter* param);
    
    /**
     * Destructor of the parameter selection. This destructor returns the
     * ownership of the parameter's widget back to the caller. 
     * Thus, it's widget(s) is(are) not destroyed here!
     *
     */
    ~ParameterSelection();
    
protected:
    /** The  delegate of the parameter (temporary owned by the layout of this selection) **/
    QPointer<QWidget> m_widget;
    /** The main layout of the parameter selection **/
    QVBoxLayout* m_verticalLayout;

};


/**
 * Class to select values for the parameters of a Model. It also adds the possibility to
 * Copy the metadata and data from an existing model if needed. This makes it very useful
 * for the task of new Model instantiation/creation.
 */
class GRAIPE_CORE_EXPORT ModelParameterSelection
 : public QDialog
{    
public:
    /**
     * Third constructor of the parameter selection. This constructor
     * may be used to create a Dialog for all parameter choices of a Model.
     * Moreover, it is possible to generate a new (empty) model based on the
     * properties of another existing model.
     * 
     * \param parent      The parent widget, to make this selection modal.
     * \param model       The model, for which the selection shall be generated.
     * \param modelList The modelList of all models to copy the parameters from.
     */
    ModelParameterSelection(QWidget *parent, Model* model, const std::vector<Model*> * modelList);

    /**
     * Destructor of the parameter selection. This destructor returns the
     * ownership of the parameter's widget back to the caller. 
     * Thus, it's widget(s) is(are) not destroyed here!
     *
     */
    ~ModelParameterSelection();
    
    /**
     * Indicates, whether an existing Model* should be used to copy the 
     * metadata from.
     *
     * \return NULL, if no metadata copying shall occur, else valid pointer to the other model.
     */
    Model* useOtherModel();
    
    /**
     * Indicated, whether the complete Model shall be cloned into the new one. Only will
     * return true, if useOtherModel() == true.
     *
     * \return If true, the new model will be an exact copy of the given one.
     */
    bool cloneOtherModel() const;
    
protected:
    /** RadioButton to find out if the metadata shall be newly set **/
    QPointer<QRadioButton>   m_radNewParameters;
    /** RadioButton to find out if the metadata shall be copied from another Model **/
    QPointer<QRadioButton>   m_radCopyParameters;
    /** CheckBox to indicate if the complete metadata shall be copied, too. **/
    QPointer<QCheckBox>      m_chkCloneOtherModel;
    /** The scrollarea, where the parameter selection will be shown. **/
    QPointer<QScrollArea> m_scrParameters;
    /** The (not-owned) pointer to the Model to copy the metadata (and maybe data) from. **/
    ModelParameter* m_otherModel;
};


/**
 * Class to select values for the parameters of an Algorithm. It also adds the possibility to
 * see the type of the results of the algorithm, if they are inserted into the m_results member
 * variable *before* the run of the algorithm. Since the results of an algorithm may depend on
 * the "input" parameter's settings, it is neccessary to update them if any parameter has changed.
 * This is done by means of the updateResults() slot.
 */
class GRAIPE_CORE_EXPORT AlgorithmParameterSelection
 : public QDialog
{
    Q_OBJECT
    
public:
    /**
     * Specialised constructor of the parameter selection. This constructor
     * may be used to create a Dialog for all parameter choices of an Algorithm.
     * It also displays the return type of the algorithm, if it is given as a 
     * prototypic result in alg->results().
     * 
     * \param parent The parent widget, to make this selection modal.
     * \param alg    The algorithm, for which the selection shall be generated.
     */
    AlgorithmParameterSelection(QWidget *parent, Algorithm* alg);

    /**
     * Destructor of the parameter selection. This destructor returns the
     * ownership of the parameter's widget back to the caller. 
     * Thus, it's widget(s) is(are) not destroyed here!
     *
     */
    ~AlgorithmParameterSelection();
    
public slots:
    /**
     * Since the results of an algorithm may depend on the "input" parameter's
     * settings, it is neccessary to update them if any parameter has changed.
     * This slot is connected to the parameter's valueChanged()-signal and will
     * be called in this case to update the results' QLabel
     */
    void updateResults();
    
protected:
    /** The label, where the result type are presented **/
    QPointer<QLabel> m_lblResults;
    /** Pointer to the algorithm (not owned) **/
    Algorithm * m_algorithm;
};

}//end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_PARAMETERSELECTION_HXX
