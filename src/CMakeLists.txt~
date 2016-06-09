cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project      (pcl_visualizer)
find_package (Qt4 REQUIRED)
find_package (VTK REQUIRED)
find_package (PCL 1.8 REQUIRED )

include_directories (${PCL_INCLUDE_DIRS})
link_directories    (${PCL_LIBRARY_DIRS})
add_definitions     (${PCL_DEFINITIONS})



include_directories(${PCL_INCLUDE_DIRS}) 
include_directories(${PROJECT_SOURCE_DIR}) 
set  (project_SOURCES main.cpp mainwindow.cpp rscloudgrabber.cpp rscloud.cpp )
set  (project_HEADERS mainwindow.h rscloudgrabber.h rscloud.h )
set  (project_FORMS   mainwindow.ui)
set  (VTK_LIBRARIES   vtkRendering vtkGraphics vtkHybrid QVTK)


#################################### Locate realsense #####################################

include_directories("/home/miky/appz/librealsense/include")
link_directories("/home/miky/appz/librealsense/lib")
#include_directories("/usr/local/include")
#link_directories("/usr/local/lib")
set(LIBS ${LIBS} realsense)
link_directories( ${realsense_LIBRARY})
include_directories( ${realsense_INCLUDE_DIR})

#################################### Locate OpenCV ############################################

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )
message(${OpenCV_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OpenCV_LIBS})

##########################################################################################

QT4_WRAP_CPP (project_HEADERS_MOC   ${project_HEADERS})
QT4_WRAP_UI  (project_FORMS_HEADERS ${project_FORMS})

INCLUDE         (${QT_USE_FILE})
ADD_DEFINITIONS (${QT_DEFINITIONS})

ADD_EXECUTABLE  (pcl_visualizer ${project_SOURCES}
                                ${project_FORMS_HEADERS}
                                ${project_HEADERS_MOC})

TARGET_LINK_LIBRARIES (pcl_visualizer ${PCL_LIBRARIES} ${QT_LIBRARIES} ${VTK_LIBRARIES} ${LIBS})

