cmake_minimum_required(VERSION 3.1)

project(graipe)

include(../../config/common.cmake)

#First the model/types
add_subdirectory(features)
add_subdirectory(images)
add_subdirectory(vectorfields)

#Then everything else
add_subdirectory(analysis)
add_subdirectory(featuredetection)
add_subdirectory(featurematching)
add_subdirectory(imagefilter)
add_subdirectory(imageprocessing)
add_subdirectory(multispectral)
add_subdirectory(opticalflow)
add_subdirectory(vectorfieldprocessing)
add_subdirectory(racerclient)
add_subdirectory(registration)
add_subdirectory(winddetection)