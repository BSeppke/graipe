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

#include "core/algorithm.hxx"

namespace graipe {

/**
 * Default Constructor of the Algorithm class.
 *
 * In a "real" algorithm implementation, please take care that
 * the results (m_results) are initialised here with the correct
 * Model classes using their empty/default constructors. This will
 * allow the further classes to derive the result types before the
 * algorithm's run.
 */
Algorithm::Algorithm()
    : m_parameters(new ParameterGroup)
{
}

/**
 * Destructor of the Algorithm class
 *
 * Since only the parameters of the algorithm have been created on 
 * the stack, we need to clean them up here
 */
Algorithm::~Algorithm() 
{
    delete m_parameters;
}

/**
 * Potentially non-const accessor of the algorithms parameters.
 */
ParameterGroup* Algorithm::parameters()
{
	return m_parameters;
}

/**
 * This functions is a convenience wrapper, which checks if all parameters are valid
 * If this is not the case, the algorithm must not be executed.
 *
 * \return true, if the parameters are valid.
 */
bool Algorithm::parametersValid() const
{
    unsigned int i=0;
    
    for(auto item : *m_parameters)
    {
        //pointer to parameter not existent or parameter not valid?
		if ( item.second == NULL)
        {
            qDebug() << "ERR for Alg: Null pointer for parameter " << i << "\n";
            return false;
        }
        if( !item.second->isValid() )
		{
            qDebug() << "ERR for Alg: parameter " << i << " Name: " << item.second->name() << " is not valid!\n";
        	return false;
        }
        i++;
    }
	return true;
}

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
void Algorithm::run()
{
	//Tell the caller about status updates
	emit statusMessage(0.0,   QString("Beginning processing"));
	
	if (!parametersValid())
	{
        //Parameters set incorrectly
		emit errorMessage(QString("Some parameters are not available"));
     }
     else
     {
        //Parameters set correctly set
		emit statusMessage(1.0,   QString("Parameters are all available"));
		
		//Calculate Result
		emit statusMessage(100.0, QString("Finished processing"));
	
		//Tell the upper instance (the separately started thread) to quit
		emit finished();
	}
}

/**
 * During the running of an algorithm, each parameter (especially the 
 * Model types) need to be locked, to prevent instable states during
 * the processes.
 *
 * This method locks each parameter.
 */
void Algorithm::lockModels()
{
    for(auto item : *m_parameters)
	{
        item.second->lock();
    }
}

/**
 * During the running of an algorithm, each parameter (especially the 
 * Model types) need to be locked, to prevent instable states during
 * the processes.
 *
 * This method unlocks each parameter.
 */
void Algorithm::unlockModels()
{
    for(auto item : *m_parameters)
    {
        item.second->unlock();
    }
}

/**
 * To enusure the running of non-thread-safe partial algorithms, like
 * the FFTW, it is possible to provide the algorithm a global Mutex.
 * This Mutex will then be used for locking if necessary.
 *
 * This is the setter for the Mutex.
 * \param mutex The global mutex to be set.
 */
void Algorithm::setGlobalAlgorithmMutex(QMutex * mutex)
{
    m_global_algorithm_mutex = mutex;
}

/**
 * To enusure the running of non-thread-safe partial algorithms, like
 * the FFTW, it is possible to provide the algorithm a global Mutex.
 * This Mutex will then be used for locking if necessary.
 *
 * This is the getter for the Mutex.
 * \return The global mutex of this algorithm.
 */
QMutex * Algorithm::globalAlgorithmMutex()
{
    return m_global_algorithm_mutex;
}

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
void Algorithm::status_update(float percent)
{
	//restrict to 99.9% because otherwise the processing of the algorithm 
	//could be interuppted unwanted
	float p_overall = 100.0*m_phase/std::max(m_phase_count,(unsigned int)1);
	p_overall += percent/std::max(m_phase_count,(unsigned int)1);
	
	emit statusMessage(std::min(p_overall, 99.9f), QString("processing"));	
}

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
std::vector<Model *>  Algorithm::results()
{
	return m_results;
}

}//end of namespace graipe
