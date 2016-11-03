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

#include "core/parameterselection.hxx"

#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>

namespace graipe {

    
/**
 * Constructor of the parameter selection. This constructor
 * may be used to create a Dialog for a single or multiple parameter
 * choices.
 * 
 * \param parent The parent widget, to make this selection modal.
 * \param param  The parameter, for which the selection shall be generated.
 */
ParameterSelection::ParameterSelection(QWidget *parent, Parameter* param)
:	QDialog(parent),
    m_widget(param->delegate())
{
    this->setWindowTitle(QString("Selection for: ") + param->name());
    
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(m_widget);
    
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    QSpacerItem* horizontalSpacer = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    QPushButton* btnCancel = new QPushButton(this);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setText(QString::fromUtf8("Cancel"));
    
    QPushButton* btnOk = new QPushButton(this);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setText(QString::fromUtf8("Ok"));
    btnOk->setFocus();
    
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(btnCancel);
    horizontalLayout->addWidget(btnOk);
    
    verticalLayout->addLayout(horizontalLayout);
    
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

/**
 * Destructor of the parameter selection. This destructor returns the
 * ownership of the parameter's widget back to the caller. 
 * Thus, it's widget(s) is(are) not destroyed here!
 *
 */
ParameterSelection::~ParameterSelection()
{
    m_widget->setParent(NULL);
}

/**
 * Constructor of the parameter selection. This constructor
 * may be used to create a Dialog for all parameter choices of a Model.
 * Moreover, it is possible to generate a new (empty) model based on the
 * properties of another existing model.
 * 
 * \param parent      The parent widget, to make this selection modal.
 * \param model       The model, for which the selection shall be generated.
 * \param modelList The modelList of all models to copy the parameters from.
 */
ModelParameterSelection::ModelParameterSelection(QWidget *parent, Model* model, const std::vector<Model*> * modelList)
:	QDialog(parent),
    m_radCopyParameters(NULL),
    m_chkCloneOtherModel(NULL),
    m_otherModel(NULL)
{
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    
    QRadioButton * radNewParameters = new QRadioButton("Select parameters by hand:", this);
    radNewParameters->setChecked(true);
    verticalLayout->addWidget(radNewParameters);
    
    QScrollArea * scrParameters = new QScrollArea(this);
    scrParameters->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrParameters->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrParameters->setWidget(model->parameters()->delegate());
    scrParameters->setWidgetResizable(true);
    
    verticalLayout->addWidget(scrParameters);
    
    m_radCopyParameters = new QRadioButton("Copy parameters from other model:", this);
    verticalLayout->addWidget(m_radCopyParameters);
    
    connect(radNewParameters, SIGNAL(toggled(bool)), scrParameters, SLOT(setEnabled(bool)));
    connect(m_radCopyParameters, SIGNAL(toggled(bool)), scrParameters, SLOT(setDisabled(bool)));
    
    m_otherModel = new ModelParameter("Model:", modelList, model->typeName());
    
    QHBoxLayout * layoutOtherModel = new QHBoxLayout(this);
    layoutOtherModel->addWidget(new QLabel(m_otherModel->name()));
    layoutOtherModel->addWidget(m_otherModel->delegate());
    verticalLayout->addLayout(layoutOtherModel);
    connect(m_radCopyParameters, SIGNAL(toggled(bool)), m_otherModel->delegate(), SLOT(setEnabled(bool)));
    
    m_chkCloneOtherModel = new QCheckBox("Completely clone other model?", this);
    verticalLayout->addWidget(m_chkCloneOtherModel);
    connect(m_radCopyParameters, SIGNAL(toggled(bool)), m_chkCloneOtherModel, SLOT(setEnabled(bool)));
    
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    QSpacerItem* horizontalSpacer = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    QPushButton* btnCancel = new QPushButton(this);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setText(QString::fromUtf8("Cancel"));
    
    QPushButton* btnOk = new QPushButton(this);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setText(QString::fromUtf8("Ok"));
    btnOk->setFocus();
    
    horizontalLayout->addItem(horizontalSpacer);
    
    horizontalLayout->addWidget(btnCancel);
    horizontalLayout->addWidget(btnOk);
    
    verticalLayout->addLayout(horizontalLayout);
    
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

/**
 * Indicates, whether an existing Model* should be used to copy the 
 * metadata from.
 *
 * \return NULL, if no metadata copying shall occur, else valid pointer to the other model.
 */
Model* ModelParameterSelection::useOtherModel()
{
    if (m_radCopyParameters->isChecked())
    {
        return m_otherModel->value();
    }
    else
    {
        return NULL;
    }
}

/**
 * Indicated, whether the complete Model shall be cloned into the new one. Only will
 * return true, if useOtherModel() == true.
 *
 * \return If true, the new model will be an exact copy of the given one.
 */
bool ModelParameterSelection::cloneOtherModel() const
{
    return m_radCopyParameters->isChecked() && m_chkCloneOtherModel->isChecked();
}

/**
 * Constructor of the parameter selection. This constructor
 * may be used to create a Dialog for all parameter choices of a algorithm.
 * It also displays the return type of the algorithm, if it is given as a 
 * prototypic result in alg->results().
 * 
 * \param parent The parent widget, to make this selection modal.
 * \param alg    The algorithm, for which the selection shall be generated.
 */
AlgorithmParameterSelection::AlgorithmParameterSelection(QWidget *parent, Algorithm* alg)
:   QDialog(parent),
    m_lblResults(NULL),
    m_algorithm(alg)
{
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(alg->parameters()->delegate());
    
    //Add resulting types of the algorithm
    m_lblResults = new QLabel(this);
    verticalLayout->addWidget(m_lblResults);
    
    connect(alg->parameters(), SIGNAL(valueChanged()), this, SLOT(updateResults()));
    updateResults();
    
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    QSpacerItem* horizontalSpacer = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    QPushButton* btnCancel = new QPushButton(this);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setText(QString::fromUtf8("Cancel"));
    
    QPushButton* btnOk = new QPushButton(this);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setText(QString::fromUtf8("Ok"));
    btnOk->setFocus();
    
    horizontalLayout->addItem(horizontalSpacer);
    
    horizontalLayout->addWidget(btnCancel);
    horizontalLayout->addWidget(btnOk);
    
    verticalLayout->addLayout(horizontalLayout);
    
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    
}

/**
 * Since the results of an algorithm may depend on the "input" parameter's
 * settings, it is neccessary to update them if any parameter has changed.
 * This slot is connected to the parameter's valueChanged()-signal and will
 * be called in this case to update the results' QLabel
 */
void AlgorithmParameterSelection::updateResults()
{
    if(m_algorithm->results().size() != 0)
    {
        QString res_QString("<b>Algorithm results:</b>\n<ol>");
        
        for(Model* res_model : m_algorithm->results())
        {
            res_QString += "  <li>" + res_model->typeName() + "</li>\n";
        }
        
        res_QString +="</ol>";
        m_lblResults->setText(res_QString);
        m_lblResults->setVisible(true);
    }
    else
    {
        m_lblResults->setText("");
        m_lblResults->setVisible(true);
    }
}

} //end of namespace graipe
