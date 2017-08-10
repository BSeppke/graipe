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

#include "images/images.h"
#include "vectorfields/vectorfields.h"
#include "opticalflow.h"

namespace graipe {


/** 
 * Creates one instance of the original Horn & Schunck Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlowHSEstimator<OpticalFlowHSOriginalFunctor>.
 */
Algorithm* createOpticalFlowHSOriginalEstimator(Workspace* wsp)
{
	return new OpticalFlowHSEstimator<OpticalFlowHSOriginalFunctor>(wsp);
}

/** 
 * Creates one instance of the Gaussian version of the Horn & Schunck Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlowHSEstimator<OpticalFlowHSFunctor>.
 */
Algorithm* createOpticalFlowHSEstimator(Workspace* wsp)
{
	return new OpticalFlowHSEstimator<OpticalFlowHSFunctor>(wsp);
}

/** 
 * Creates one instance of the Nagel & Enkelmann Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlowHSEstimator<OpticalFlowNEFunctor>.
 */
Algorithm* createOpticalFlowNEEstimator(Workspace* wsp)
{
	return new OpticalFlowHSEstimator<OpticalFlowNEFunctor>(wsp);
}

/** 
 * Creates one instance of the Combined Local Global Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlowBruhnEstimator<OpticalFlowCLGFunctor>.
 */
Algorithm* createOpticalFlowCLGEstimator(Workspace* wsp)
{
	return new OpticalFlowBruhnEstimator<OpticalFlowCLGFunctor>(wsp);
}

/** 
 * Creates one instance of the Non-linear version of the Combined Local Global Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlowBruhnEstimator<OpticalFlowCLGNonlinearFunctor>.
 */
Algorithm* createOpticalFlowCLGNonlinearEstimator(Workspace* wsp)
{
	return new OpticalFlowBruhnEstimator<OpticalFlowCLGNonlinearFunctor>(wsp);
}

/** 
 * Creates one instance of the Lucas-Kanade Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlowLKEstimator.
 */
Algorithm* createOpticalFlowLKEstimator(Workspace* wsp)
{
	return new OpticalFlowLKEstimator(wsp);
}

/** 
 * Creates one instance of the Farnebaeck Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlowFBEstimator.
 */
Algorithm* createOpticalFlowFBEstimator(Workspace* wsp)
{
	return new OpticalFlowFBEstimator(wsp);
}

/** 
 * Creates one instance of the Structure Tensor  Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlowTensorEstimator<OpticalFlowSTFunctor>.
 */
Algorithm* createOpticalFlowSTEstimator(Workspace* wsp)
{
	return new OpticalFlowTensorEstimator<OpticalFlowSTFunctor>(wsp);
}
 
/** 
 * Creates one instance of the Verri's constant constrast Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlowCCEstimator.
 */
Algorithm* createOpticalFlowCCEstimator(Workspace* wsp)
{
	return new OpticalFlowCCEstimator(wsp);
}

/**
 * Experimental algorithms are not working right now.
 * 
 * TODO: If possible, fix them. If not, discard them.
 */
/*
Algorithm* createOpticalFlowGETEstimator()
{
	return new OpticalFlowExperimentalEstimator<OpticalFlowGETFunctor>;
}

Algorithm* createOpticalFlowHGEstimator()
{
	return new OpticalFlowExperimentalEstimator<OpticalFlowHGFunctor>;
}
*/



/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new algorithms.
 */
class OpticalFlowModule
:   public Module
{
	public:
        /**
         * Default constructor for the OpticalFlowModule.
         */
		OpticalFlowModule()
		{
        }
    
        /**
         * Returns the ModelFactory of the OpticalFlowModule.
         * Since this module does not introduce any new Models, it will
         * return an empty ModelFactory.
         *
         * \return An empty ModelFactory of the OpticalFlowModule.
         */
		ModelFactory modelFactory() const
		{
			return ModelFactory();
		}
    
        /**
         * Returns the ViewControllerFactory of the OpticalFlowModule.
         * Since this module does not introduce any new Models,  no views are
         * needed. Thus, it will return an empty ViewControllerFactory.
         *
         * \return An empty ViewControllerFactory of the OpticalFlowModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			return ViewControllerFactory();
		}
		
        /**
         * Returns the AlgorithmFactory of the OpticalFlowModule.
         * Here, we provide the algorithms defined above.
         *
         * \return An AlgorithmFactory containing the analysis algorithms defined herein.
         */
		AlgorithmFactory algorithmFactory() const
		{
			AlgorithmFactory alg_factory;
			
			AlgorithmFactoryItem alg_item;
            alg_item.topic_name = "Optical flow estimation";
			
			//1. original hs method
			alg_item.algorithm_name = "Horn-Schunk method (original)";
            alg_item.algorithm_type = "OpticalFlowHSOriginalEstimator";
			alg_item.algorithm_fptr = &createOpticalFlowHSOriginalEstimator;
			alg_factory.push_back(alg_item);
			
			//2. extended hs method
			alg_item.algorithm_name = "Horn-Schunk method (gaussian kernels)";
            alg_item.algorithm_type = "OpticalFlowHSEstimator";
			alg_item.algorithm_fptr = &createOpticalFlowHSEstimator;
			alg_factory.push_back(alg_item);
			
			//3. nagel's method
			alg_item.algorithm_name = "Nagel-Enkelmann method";	
            alg_item.algorithm_type = "OpticalFlowNEEstimator";
			alg_item.algorithm_fptr = &createOpticalFlowNEEstimator;
			alg_factory.push_back(alg_item);
			
			
			//4. lukas kanade method
			alg_item.algorithm_name = "Lucas kanade method";	
            alg_item.algorithm_type = "OpticalFlowLKEstimator";
			alg_item.algorithm_fptr = &createOpticalFlowLKEstimator;
			alg_factory.push_back(alg_item);
			
			//5. structure tensor method
			alg_item.algorithm_name = "Structure tensor method";	
            alg_item.algorithm_type = "OpticalFlowSTEstimator";
			alg_item.algorithm_fptr = &createOpticalFlowSTEstimator;
			alg_factory.push_back(alg_item);
			
			//6. Verri's constant contrast method
			alg_item.algorithm_name = "Verri's constant contrast method";
            alg_item.algorithm_type = "OpticalFlowCCEstimator";
			alg_item.algorithm_fptr = &createOpticalFlowCCEstimator;
			alg_factory.push_back(alg_item);
			
			//7. Farnebaeck's method
			alg_item.algorithm_name = "Farnebaeck method";
            alg_item.algorithm_type = "OpticalFlowFBEstimator";
			alg_item.algorithm_fptr = &createOpticalFlowFBEstimator;
			alg_factory.push_back(alg_item);
			
            
			//8. Combined Local Global method (linear)
			alg_item.algorithm_name = "Combined Local Global method (linear)";	
            alg_item.algorithm_type = "OpticalFlowCLGEstimator";
			alg_item.algorithm_fptr = &createOpticalFlowCLGEstimator;
			alg_factory.push_back(alg_item);
			
			//9. Combined Local Global method (nonlinear)
			alg_item.algorithm_name = "Combined Local Global method (nonlinear)";	
            alg_item.algorithm_type = "OpticalFlowCLGNonlinearEstimator";
			alg_item.algorithm_fptr = &createOpticalFlowCLGNonlinearEstimator;
			alg_factory.push_back(alg_item);
			
			/**
             * Experimental algorithms are not working right now.
             * 
             * TODO: If possible, fix them. If not, discard them.
             */
			 /*
             //9. Gradient energy tensor method
			 alg_item.algorithm_name = "Gradient energy tensor method";
			 alg_item.algorithm_fptr = &createOpticalFlowGETEstimator;
			 alg_factory.push_back(alg_item);
			 
			 //10. Hourglass filtered structure tensor method
			 alg_item.algorithm_name = "Hourglass filtered structure tensor method";
			 alg_item.algorithm_fptr = &createOpticalFlowHGEstimator;
			 alg_factory.push_back(alg_item);
			 */
			
			return alg_factory;
		}
		
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "OpticalFlowModule"
         */
        QString name() const
		{
			return "OpticalFlowModule";
		}		
};

} //end of namespace graipe

/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  OpticalFlowModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	\return The pointer to a new instance of this module.
 */
#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::OpticalFlowModule;
	}
}




