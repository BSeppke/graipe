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

#include "vectorfieldprocessing/vectorfieldprocessing.h"
#include "core/core.h"

namespace graipe {
    



/**
 * This class implements the vectorfield smoothing approach by means of an graipe::Algorithm.
 */
class VectorfieldSmoother
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        VectorfieldSmoother()
        {
            m_parameters->addParameter("vf",               new ModelParameter("Vectorfield", "SparseWeightedMultiVectorfield2D"));
            m_parameters->addParameter("radius",           new FloatParameter("radius for smoothing", 0.0, 9999, 10));
            m_parameters->addParameter("weightT",          new FloatParameter("weight threshold", 0.0, 9999, 0.0));
            m_parameters->addParameter("iterations",       new IntParameter("iterations", 1, 9999, 10));
            m_parameters->addParameter("use_all_weights?", new BoolParameter("use all alternative weights for smoothness comparison", true));
        }
        QString typeName() const
        {
            return "VectorfieldSmoother";
        }
        
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    emit statusMessage(0.0, QString("started"));
                        
                    ModelParameter	* param_vf = static_cast<ModelParameter*> ((*m_parameters)["vf"]);
                    FloatParameter	* param_radius = static_cast<FloatParameter*>((*m_parameters)["radius"]);
                    FloatParameter	* param_threshold = static_cast<FloatParameter*>((*m_parameters)["weightT"]);
                    IntParameter		* param_iterations = static_cast<IntParameter*>((*m_parameters)["iterations"]);
                    BoolParameter		* param_use_all = static_cast<BoolParameter*>((*m_parameters)["use_all_weights?"]);
                    
                    SparseWeightedMultiVectorfield2D* current_vf = static_cast<SparseWeightedMultiVectorfield2D* >(  param_vf->value() );	
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    SparseWeightedVectorfield2D* new_vf = smoothVectorfield(current_vf,
                                                                              param_iterations->value(), 
                                                                              param_radius->value(), 
                                                                              param_threshold->value(), 
                                                                              param_use_all->value());
                    
                    new_vf->setName(QString("Smoothed ") + current_vf->name());
                    
                    QString descr("The following parameters were used for smoothing:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_vf->setDescription(descr);
                    
                    current_vf->copyGeometry(*new_vf);
                    new_vf->setScale(current_vf->scale());
                    
                    m_results.push_back(new_vf);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/** 
 * Creates one instance of the vectorfield smoothing
 * algorithm defined above.
 *
 * \return A new instance of the VectorfieldSmoother.
 */
Algorithm* createVectorfieldSmoother()
{
	return new VectorfieldSmoother;
}




/**
 * This class implements the vectorfield relaxation approach by means of an graipe::Algorithm.
 */
class VectorfieldRelaxer : public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        VectorfieldRelaxer()
        {
            m_parameters->addParameter("vf",               new ModelParameter("Vectorfield", "SparseWeightedMultiVectorfield2D"));
            m_parameters->addParameter("radius",           new FloatParameter("radius for smoothing", 0.0, 9999, 10));
            m_parameters->addParameter("weightT",          new FloatParameter("weight threshold", 0.0, 9999, 0.0));
            m_parameters->addParameter("iterations",       new IntParameter("iterations", 1, 9999, 10));
            m_parameters->addParameter("use_all_weights?", new BoolParameter("use all alternative weights for smoothness comparison", true));
        }
        QString typeName() const
        {
            return "VectorfieldRelaxer";
        }
        
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try
                {
                    emit statusMessage(0.0, QString("started"));
                    
                    ModelParameter	* param_vf = static_cast<ModelParameter*> ((*m_parameters)["vf"]);
                    FloatParameter	* param_radius = static_cast<FloatParameter*>((*m_parameters)["radius"]);
                    FloatParameter	* param_threshold = static_cast<FloatParameter*>((*m_parameters)["weightT"]);
                    IntParameter		* param_iterations = static_cast<IntParameter*>((*m_parameters)["iterations"]);
                    BoolParameter		* param_use_all = static_cast<BoolParameter*>((*m_parameters)["use_all_weights?"]);
                    
                    SparseWeightedMultiVectorfield2D* current_vf = static_cast<SparseWeightedMultiVectorfield2D* >(  param_vf->value() );	
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    //create new vf and do the transform
                    SparseWeightedVectorfield2D* new_vf;
                    
                    new_vf = relaxVectorfield(current_vf,	param_iterations->value(), 
                                              param_radius->value(), param_threshold->value(), param_use_all->value());
                    
                    new_vf->setName(QString("Relaxed ") + current_vf->name());
                    
                    QString descr("The following parameters were used for relaxation:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_vf->setDescription(descr);
                    
                    current_vf->copyGeometry(*new_vf);
                    new_vf->setScale(current_vf->scale());
                    
                    m_results.push_back(new_vf);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/** 
 * Creates one instance of the vectorfield relaxation
 * algorithm defined above.
 *
 * \return A new instance of the VectorfieldRelaxer.
 */
Algorithm* createVectorfieldRelaxer()
{
	return new VectorfieldRelaxer;
}




/**
 * This class implements the greedy clustering approach by means of an graipe::Algorithm.
 */
class VectorfieldClustererGreedy : public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        VectorfieldClustererGreedy()
        {
            m_parameters->addParameter("vf",  new ModelParameter("Vectorfield", "SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D"));
            m_parameters->addParameter("weight-dir", new FloatParameter("weight direction for clustering", 0.0, 9999, 1.0));
            m_parameters->addParameter("radius", new FloatParameter("radius for clustering sizes", 0.0, 9999, 10));
            m_parameters->addParameter("weightT", new FloatParameter("weight threshold", 0.0, 9999, 0.0));
            m_parameters->addParameter("use_local_vectors?", new BoolParameter("use local vectors", false));
        }
        QString typeName() const
        {
            return "VectorfieldClustererGreedy";
        }
        
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    emit statusMessage(0.0, QString("started"));
                        
                    ModelParameter	* param_vf = static_cast<ModelParameter*> ((*m_parameters)["vf"]);
                    FloatParameter	* param_direction_weight = static_cast<FloatParameter*>((*m_parameters)["weight-dir"]);
                    FloatParameter	* param_radius = static_cast<FloatParameter*>((*m_parameters)["radius"]);
                    FloatParameter	* param_threshold = static_cast<FloatParameter*>((*m_parameters)["weightT"]);
                    BoolParameter	    * param_use_local = static_cast<BoolParameter*>((*m_parameters)["use_local_vectors?"]);
                    
                    SparseVectorfield2D* current_vf = static_cast<SparseVectorfield2D* >(  param_vf->value() );	
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    std::vector<Model*> results; 
                    
                    if(current_vf->typeName() =="SparseWeightedVectorfield2D")
                    {
                        results = clusterVectorfieldGreedy(static_cast<SparseWeightedVectorfield2D*>(current_vf), param_radius->value(), param_threshold->value(), param_direction_weight->value(), param_use_local->value());
                    }
                    else {
                        results = clusterVectorfieldGreedy(static_cast<SparseWeightedMultiVectorfield2D*>(current_vf), param_radius->value(), param_threshold->value(), param_direction_weight->value(), param_use_local->value());
                    }

                    results[0]->setName(QString("Labeled cluster vectors ") + current_vf->name());
                    static_cast<SparseWeightedVectorfield2D*>(results[0])->setScale(current_vf->scale());
                    
                    results[1]->setName(QString("Cluster center vectors ") + current_vf->name());
                    static_cast<SparseWeightedVectorfield2D*>(results[1])->setScale(current_vf->scale());
                    
                    results[2]->setName(QString("Cluster boarders ") + current_vf->name());
                    
                    QString descr("The following parameters were used for clustering:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    
                    for(unsigned int i=0; i<results.size(); ++i)
                    {
                        results[i]->setDescription(descr);
                        
                        current_vf->copyGeometry(*results[i]);
                        
                        m_results.push_back(results[i]);
                    }
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/** 
 * Creates one instance of the greedy vectorfield clustering
 * algorithm defined above.
 *
 * \return A new instance of the VectorfieldClustererGreedy.
 */
Algorithm* createVectorfieldClustererSimple()
{
	return new VectorfieldClustererGreedy;
}




/** 
 * This class implements the k-Means clustering approach by means of an graipe::Algorithm.
 */
class VectorfieldClustererKMeans
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        VectorfieldClustererKMeans()
        {
            m_parameters->addParameter("vf", new ModelParameter("Vectorfield", "SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D"));
            m_parameters->addParameter("weight-dir", new FloatParameter("weight direction for clustering", 0.0, 9999, 1.0));
            m_parameters->addParameter("k", new IntParameter("k (count of clusters)", 0.0, 9999, 10));
            m_parameters->addParameter("weightT",  new FloatParameter("weight threshold", 0.0, 9999, 0.0));
            m_parameters->addParameter("use_local_vectors?", new BoolParameter("use local vectors", false));
        }
        QString typeName() const
        {
            return "VectorfieldClustererKMeans";
        }
        
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    emit statusMessage(0.0, QString("started"));
                    
                    ModelParameter	* param_vf = static_cast<ModelParameter*> ((*m_parameters)["vf"]);
                    FloatParameter	* param_direction_weight = static_cast<FloatParameter*>((*m_parameters)["weight-dir"]);
                    IntParameter		* param_k = static_cast<IntParameter*>((*m_parameters)["k"]);
                    FloatParameter	* param_threshold = static_cast<FloatParameter*>((*m_parameters)["weightT"]);
                    BoolParameter	    * param_use_local = static_cast<BoolParameter*>((*m_parameters)["use_local_vectors?"]);
                    
                    SparseVectorfield2D* current_vf = static_cast<SparseVectorfield2D* >(  param_vf->value() );	
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    std::vector<Model*> results; 
                    
                    if(current_vf->typeName() =="SparseWeightedVectorfield2D")
                    {
                        results = clusterVectorfieldKMeans(static_cast<SparseWeightedVectorfield2D*>(current_vf), param_k->value(), param_threshold->value(), param_direction_weight->value(), param_use_local->value());
                    }
                    else {
                        results = clusterVectorfieldKMeans(static_cast<SparseWeightedMultiVectorfield2D*>(current_vf), param_k->value(), param_threshold->value(), param_direction_weight->value(), param_use_local->value());
                    }
                    
                    results[0]->setName(QString("K-Means Labeled cluster vectors ") + current_vf->name());
                    static_cast<SparseWeightedVectorfield2D*>(results[0])->setScale(current_vf->scale());
                    
                    results[1]->setName(QString("K-Means Cluster center vectors ") + current_vf->name());
                    static_cast<SparseWeightedVectorfield2D*>(results[1])->setScale(current_vf->scale());
                    
                    results[2]->setName(QString("K-Means Cluster boarders ") + current_vf->name());
                
                    
                    QString descr("The following parameters were used for k-means clustering:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    
                    for(unsigned int i=0; i<results.size(); ++i)
                    {
                        results[i]->setDescription(descr);
                        
                        current_vf->copyGeometry(*results[i]);
                        
                        m_results.push_back(results[i]);
                    }
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/** 
 * Creates one instance of the k-means vectorfield clustering
 * algorithm defined above.
 *
 * \return A new instance of the VectorfieldClustererKMeans.
 */
Algorithm* createVectorfieldClustererKMeans()
{
	return new VectorfieldClustererKMeans;
}




/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new algorithms.
 */
class VectorfieldProcessingModule
:   public Module
{
	public:
        /**
         * Default constructor for the VectorfieldProcessingModule.
         */
		VectorfieldProcessingModule()
		{
        }
    
        /**
         * Returns the ModelFactory of the VectorfieldProcessingModule.
         * Since this module does not introduce any new Models, it will
         * return an empty ModelFactory.
         *
         * \return An empty ModelFactory of the VectorfieldProcessingModule.
         */
        ModelFactory modelFactory() const
        {
            return ModelFactory();
        }
    
        /**
         * Returns the ViewControllerFactory of the VectorfieldProcessingModule.
         * Since this module does not introduce any new Models,  no views are
         * needed. Thus, it will return an empty ViewControllerFactory.
         *
         * \return An empty ViewControllerFactory of the VectorfieldProcessingModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			return ViewControllerFactory();
		}
        		
        /**
         * Returns the AlgorithmFactory of the VectorfieldProcessingModule.
         * Here, we provide the tree algorithms defined above.
         *
         * \return An AlgorithmFactory containing the processing algorithms defined herein.
         */
		AlgorithmFactory algorithmFactory() const
		{
			AlgorithmFactory alg_factory;
		
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "Vectorfield processing";
		
			//1. Smoothing
			alg_item.algorithm_name = "Vectorfield smoothing";	
			alg_item.algorithm_fptr = &createVectorfieldSmoother;
			alg_factory.push_back(alg_item);
			
			//2. Relaxation
			alg_item.algorithm_name = "Vectorfield relaxation (variance minimization)";		
			alg_item.algorithm_fptr = &createVectorfieldRelaxer;
			alg_factory.push_back(alg_item);
			
			//3. Simple Clustering
			alg_item.algorithm_name = "Basic Vectorfield clustering";	
			alg_item.algorithm_fptr = &createVectorfieldClustererSimple;
			alg_factory.push_back(alg_item);
			
			//4. KMeans Clustering
			alg_item.algorithm_name = "k-means Vectorfield clustering";	
			alg_item.algorithm_fptr = &createVectorfieldClustererKMeans;
			alg_factory.push_back(alg_item);
			
			
			return alg_factory;
		}
		
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "VectorfieldprocessingModule"
         */
		QString name() const
        {
            return "VectorfieldprocessingModule";
        }
};

} //end of namespace graipe

/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  VectorfieldProcessingModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	\return The pointer to a new instance of this module.
 */
#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::VectorfieldProcessingModule;
	}	
}




