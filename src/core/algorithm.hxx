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

#ifndef GRAIPE_CORE_ALGORITHM_HXX
#define GRAIPE_CORE_ALGORITHM_HXX

#include "core/config.hxx"
#include "core/model.hxx"
#include "core/parameters.hxx"

#include <vector>

#include <QMutex>

/**
 * @file
 * @brief Header file for the Algorithm class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * This class defines the concept of an algorithm within the GRAIPE
 * framework. An algorithm can best be described by a function,
 * which takes a number of arguments (given as a ParameterGroup)
 * and results in a set of Models.
 *
 * Defining the algorithm class as a QObjects allows a concurrent
 * execution of each algorothm call in another thread as proposed by
 * Hughes in the following article:
 * http://labs.qt.nokia.com/2010/06/17/youre-doing-it-wrong/
 *
 * If the called algorithm is (somewhere) not thread-safe, it can lock
 * the given global Mutex, which is generated and passed by the GUI
 * to each algorithm before the execution.
 *
 * To further keep consistency among the involved models, which are 
 * needed by the algorithm to run, it has a lockModels() method
 * method and a unlockModels() method to make sure the models 
 * cannot be (re-)edited during the running of an algorithm.
 *
 * During each run, the algorithm uses signals to report about the
 * current progress, errors and finshed state.
 */
class GRAIPE_CORE_EXPORT Algorithm
:	public QObject
{
	Q_OBJECT
	
	public:
        /**
         * Default Constructor of the Algorithm class.
         *
         * In a "real" algorithm implementation, please take care that
         * the results (m_results) are initialised here with the correct
         * Model classes using their empty/default constructors. This will
         * allow the further classes to derive the result types before the
         * algorithm's run.
         */
		Algorithm();
    
        /**
         * Destructor of the Algorithm class
         *
         * Since only the parameters of the algorithm have been created on 
         * the stack, we need to clean them up here
         */
		virtual ~Algorithm();
    
        /**
         * Potentially non-const accessor of the algorithms parameters.
         */
		virtual ParameterGroup* parameters();

        /**
         * This functions is a convenience wrapper, which checks if all parameters are valid
         * If this is not the case, the algorithm must not be executed.
         *
         * \return true, if the parameters are valid.
         */
		virtual bool parametersValid() const;
    
        /**
         * During the running of an algorithm, each parameter (especially the 
         * Model types) need to be locked, to prevent instable states during
         * the processes.
         *
         * This method locks each parameter.
         */
        void lockModels();
    
        /**
         * During the running of an algorithm, each parameter (especially the 
         * Model types) need to be locked, to prevent instable states during
         * the processes.
         *
         * This method unlocks each parameter.
         */
     	void unlockModels();

        /**
         * To enusure the running of non-thread-safe partial algorithms, like
         * the FFTW, it is possible to provide the algorithm a global Mutex.
         *This Mutex will then be used for locking if necessary.
         *
         * This is the setter for the Mutex.
         * \param mutex The global mutex to be set.
         */
        void setGlobalAlgorithmMutex(QMutex * mutex);

        /**
         * To enusure the running of non-thread-safe partial algorithms, like
         * the FFTW, it is possible to provide the algorithm a global Mutex.
         *This Mutex will then be used for locking if necessary.
         *
         * This is the getter for the Mutex.
         * \return The global mutex of this algorithm.
         */
        QMutex * globalAlgorithmMutex();

        /**
         * For monitoring purpose, algorithms may send progress signals by means of
         * status messages. This method provids a slot, which creates and sends the
         * appropriate processing signal 0...99.9 (%) as a status message.
         * This function also takes into account the different phases of an algorithm,
         * which are defined by the total number of phases: m_pase_count and
         * the current phase: m_phase.
         *
         * \param percent The current progress of the algorithm in percent (0...99.9)
         */
		virtual void status_update(float percent);

        /**
         * This method returns the result of the algorithm. There are two use cases for this 
         * function:
         * 1. Before the running of the algorithm:
         *    If properly initialized, the results contain empty prototypes of the (not yet
         *    exisiting) results. These can be used to the derive the expected result types.
         *
         * 2. After the algorithm sent the finished() signal
         *    Then, the "real" results can be obtained using this function.
         *
         * \return The results of the algorithm (if finished).
         */
		virtual std::vector<Model*> results();
	
    
    public slots:
        /**
         * Running an algorithm is held inside this method
         *
         * During each run, different signals may be emitted by this class:
         * - statusMessage(percent, "some text")
         * - errorMessage("something went wrong")
         * - finished()
         *
         * After the receiver got the finished() signal, it may access the results
         * of each algorithm and destroy the worker thread of the algorithm. The
         * results will than be "grabbed" by the caller of the algorithms, which
         * changed the ownership from algorithm to the caller.
         */
        virtual void run();

	signals:
        /** Neutral status message **/
		void statusMessage(float p, QString message);
        /** Message for the error case of an algorithm **/
		void errorMessage(QString message);
        /** Finished (correctly) **/
		void finished();

	protected:
        /** The current phase of the algorithm **/
		unsigned int m_phase;
        /** The total number phases of the algorithm **/
        unsigned int m_phase_count;
	
        /** The parameters **/
		ParameterGroup * m_parameters;
		/** The results **/
        std::vector<Model*> m_results;
    
    private:
        /** The global algorithm mutex **/
        QMutex * m_global_algorithm_mutex;
};

}//end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_ALGORITHM_HXX
