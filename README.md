# GRAIPE

A modular interactive framework for image processing and computer vision

## Introduction

GRAIPE's aim is to give you an extendible and (more-or-less) easy to use interactive framework for image processing and computer vision. From the software side, GRAIPE is based on:

  - Qt5 for the GUI part,  
  - VIGRA for the image processing and computer vision part, and
  - GDAL for the import and export of various file formats.

As a build system, CMake is used. This greatly simplifies the build process under Windows, Mac OS, and Linux. GRAIPE is supposed to be compiled under all of these major platforms, but is mainly tested and maintained under Windows and Mac.


## Design concept

The design concept of GRAIPE is heavily based on the Model View Controller (MVC) paradigm. This paradigm forces a strict separation between data and data representation (Models), the visualization of models (Views) and the interactive manipulation of objects (via Controllers). However, since the interactive manipulation of models often refers to a particular view, we neglect the sharp separation between Views and Controllers and replace them by ViewControllers.

Moreover, there exist functional components, which are able to compute/generate models given parameter and other models. These functional components are called Algorithms and work in a non-interactive but concurrent manner. One example for such an algorithm might be an motion detection algorithm, which transfers two "image" Models and a set of further parameters into a vector field showing the estimated motion between the first and second image.

Unlike other software, where there is a fixed set of functionality and maybe a plugin interface for extension, GRAIPE follows the software design pattern of building blocks, to which we refer to as Modules. Only a very small part of the functionality is hold by the so-called core and gui modules, which are needed to build GRAIPE. A Module may encapsulate all the necessary functionality via a clearly defined API (see src/core/module.hxx). Thus any module might enrich GRAIPE by means of:

 - New Models (data container classes),
 - New ViewControllers (visualizations of Models), and
 - New Algorithms. 

Models should also provide a public api, so that their functionality can be used hierarchically for other dependent models. It is also recommended to keep models free from non-basic algorithms in order to not overload the necessary models.  This makes it easy to tailor GRAIPE using just a subset of Modules.


## Build

Since the CMake build system is used, it should be quite easy to build GRAIPE. Just go into the GRAIPE dir, create a build dir and let CMake create any type of Make or build/project files for you. 

GRAIPE relies on the following third-party open source libraries/tools, which need to be installed before the build process can be run without errors:

  - CMake (>= 3.1)
  - Qt5 (Widgets, Network, PrintSupport, Svg),
  - VIGRA (>= 1.11, only the header files are needed),
  - GDAL,
  - ZLIB, 
  - FFTW, and
  - optionally for the docs: Doxygen.

Under Mac OS X, we made good experiences with the MacPorts package manager to get those. Under Windows, you may install them manually or try to use Visual Studio and my vspkg project (also on GitHub) to install all dependencies.

The successful build creates the binaries in the bin dictionary and, if the docs shall be generated, the docs in the doc directory.


## Run/Install

To run GRAIPE, you need to enable the loading from shared objects either beneath the executable file or, under Mac OS X inside the Application container file. GRAIPE searches both while startup. Thus, under Linux, you may have to set the LD_LIBRARY path before startup, e.g. using a shell script.

The build of installation files is currently provided for Windows and Mac OS X only. See the deployment folder for the corresponding shell script for Mac OS X and the Nullsoft Installer script for Windows. For each release, binaries will be provided on GitHub, too.


 

## Extend

Since GRAIPE is designed based on the building blocks metaphor, it is really easy to write extension modules for GRAIPE. A good starting point for such a new module might be the graipe_imageprocessing module in the src/modules directory. It introduces some basic image processing algorithms into GRAIPE and thus adds new Algorithms to the framework. 

If you are interested in extending it by means of new data types/modules, you will preferably look into the features module to get an idea of how to implement them.

After adding the new module in the modules directory, just make sure that it gets compiled too by adding the subfolder to the CMake script inside the src/modules folder. If you are planning to build an installer, you need to edit the installer scripts in the deployment folder as well.


## To Dos

GRAIPE is still missing a Unit test suite. This is essential, especially for the models, which have been introduced.

The display performance of Qt5 reduces drastically for large data types like images. This needs to be further investigated and hopefully fixed.

