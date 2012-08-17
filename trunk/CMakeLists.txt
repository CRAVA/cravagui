# CMake file for cravagui
# Author Alf B. Rustad abir@statoil.com

# Pointers to good documentation on CMake:
# http://www.qtcentre.org/wiki/index.php?title=Compiling_Qt4_apps_with_CMake
# http://techbase.kde.org/Development/Tutorials/CMake#CMake_and_Qt4
# http://www.cmake.org/Wiki/CMake#Tutorials

project(cravagui)
cmake_minimum_required(VERSION 2.8)

find_package(Qt4 REQUIRED) # find and setup QT4 for this project

# QtXml is not loaded by default
# http://developer.qt.nokia.com/quarterly/view/using_cmake_to_build_qt_projects
set(QT_USE_QTXML true)
include(${QT_USE_FILE})


#--------------------------------------------
# Workaround for Qt install on /project/res
# not needed for standard Qt install
#find_package(GTK2 COMPONENTS gtk)
#include_directories (${GTK2_INCLUDE_DIRS})
#link_libraries(${cravagui} ${GTK2_LIBRARIES} )
#--------------------------------------------

# With this line, all intermediary files will be available in the include paths
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# The variable cravagui_sources contain all .cpp files for the project
set(cravagui_sources
  Main_crava.cpp
  Main.cpp
  VariogramDialog.cpp
  ModeDialog.cpp
  SettingsDialog.cpp
  StandardStrings.cpp
)

# The variable cravagui_headers contain all header files
set(cravagui_headers
  Main_crava.h
  VariogramDialog.h
  ModeDialog.h
  SettingsDialog.h
  StandardStrings.h
)

# The variable cravagui_forms contain all form files
set(cravagui_forms
  Main_crava.ui
  VariogramDialog.ui
  ModeDialog.ui
  SettingsDialog.ui
)

# The variable cravagui_resources contain resource files (typically paths to images)
set(cravagui_resources cravaguiresources.qrc)

# Some QT magic to wrap it all together with moc, headers and resources
QT4_WRAP_CPP(cravagui_headers_moc ${cravagui_headers})
QT4_WRAP_UI(cravagui_forms_headers ${cravagui_forms})
QT4_ADD_RESOURCES(cravagui_resources_resources ${cravagui_resources})

add_executable (cravagui
  ${cravagui_sources}
  ${cravagui_headers}
  ${cravagui_headers_moc}
  ${cravagui_forms_headers}
  ${cravagui_resources_resources}
)

# Link the cravagui executable to the Qt libraries
#target_link_libraries(cravagui ${QT_LIBRARIES})
# Work around needed for custom Qt install at /project/res
# Line above works fine on standard install Qt system 
target_link_libraries(cravagui ${QT_LIBRARIES} ${GTK2_LIBRARIES})
