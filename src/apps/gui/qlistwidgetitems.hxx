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

#ifndef GRAIPE_GUI_QLISTWIDGETITEMS_HXX
#define GRAIPE_GUI_QLISTWIDGETITEMS_HXX

#include "core/model.hxx"
#include "core/viewcontroller.hxx"
#include "core/algorithm.hxx"

#include <QListWidgetItem>




/**
 * This file contains some specialized classes of QListWidget. They
 * are used inside the GUI to strore the corresponding Models, Views
 * Properties, Controllers and Algorithms inside simple QListWidgets as
 * items. 
 * Using casts and specialized accessors, the GUI is able to get from each
 * list entry to a GRAIPE datatype. 
 */




namespace graipe {
    
/**
 * This class extends the basic QListWidgetItem class to have another member
 * variable: an assigned Model pointer. This can be used by the
 * GUI to store the models inside a QListWidget.
 */
class QListWidgetModelItem
    : public QListWidgetItem
{
public:
    /**
     * Default constructor of the QListWidgetModelItem class.
     *
     * \param text  A QString determining the text of this item. Dafaults to "".
     * \param model A Model pointer to the Model attached to this item.
     *              Note, that the ownership changes here - the Model
     *              will belong to the item after creation.
     */
    QListWidgetModelItem(QString text="", Model* model=NULL);
    
    /**
     * Destructor of the QListWidgetModelItem class.
     * Since the item owns the Model pointer, it will be deleted if the Model is
     * not locked. If the Model is locked, it will be kept alive but a debug
     * message will be printed out.
     */
    ~QListWidgetModelItem();
    
    /**
     * Const accessor to the Model pointer. You may edit the Model, but not
     * the pointer itself.
     *
     * \return the assigned Model pointer.
     */
    Model* model() const;
    
protected:
    //The Model pointer
    Model* m_model;
};

    
    
/**
 * This class extends the basic QListWidgetItem class to have another member
 * variable: an assigned ViewController pointer. This can be used by the
 * GUI to store the ViewControllers inside a QListWidget.
 */
class QListWidgetViewControllerItem
	: public QListWidgetItem
{
public:
    /**
     * Default constructor of the QListWidgetViewItem class.
     *
     * \param text A QString determining the text of this item. Dafaults to "".
     * \param view A View pointer to the View attached to this item.
     *             Note, that the ownership changes here - the View
     *             will belong to the item after creation.
     */
    QListWidgetViewControllerItem(QString text="", ViewController* viewController=NULL);
    
    /**
     * Destructor of the QListWidgetViewItem class.
     * Since the item owns the View pointer, it will be deleted at destruction.
     */
    ~QListWidgetViewControllerItem();
    
    /**
     * Const accessor to the View pointer. You may edit the View, but not
     * the pointer itself.
     *
     * \return the assigned View pointer.
     */
    ViewController* viewController() const;
    
protected:
    //The View pointer
    ViewController* m_viewController;
};


    
/**
 * This class extends the basic QListWidgetItem class to have another member
 * variable: an assigned Algorithm pointer. This can be used by the
 * GUI to store the Algorithm statuses in the model list while the running.
 */
class QListWidgetAlgorithmItem
    : public QListWidgetItem
{
public:
    /**
     * Default constructor of the QListWidgetAlgorithmItem class.
     *
     * \param text A QString determining the text of this item. Dafaults to "".
     * \param alg  A Algorithm pointer to the Algorithm attached to this item.
     *             Note, that the ownership does not change for the algorithm.
     */
    QListWidgetAlgorithmItem(QString text="", Algorithm* alg=NULL);
    
    /**
     * Const accessor to the Algorithm pointer. You may edit the Algorithm, but not
     * the pointer itself.
     *
     * \return the assigned Algorithm pointer.
     */
    Algorithm* algorithm() const;
	
protected:
    //The algorithm pointer
	Algorithm* m_algorithm;
};
    
}//end of namespace graipe

#endif //GRAIPE_GUI_QLISTWIDGETITEMS_HXX
