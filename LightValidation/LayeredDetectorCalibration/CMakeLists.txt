#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(GS20Cal)

#----------------------------------------------------------------------------
# check MPI package...
find_package(MPI REQUIRED)
# modify these var. if needed
set(CMAKE_CXX_COMPILER mpicxx)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Add G4mpi library
#
SET(Geant4_LIBRARIES ${Geant4_LIBRARIES} G4mpi)
ADD_LIBRARY(G4mpi SHARED IMPORTED)
SET_PROPERTY(TARGET G4mpi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
SET_TARGET_PROPERTIES(G4mpi PROPERTIES
  IMPORTED_LOCATION_RELEASE "${Geant4_DIR}/../libG4mpi.so"
  IMPORTED_SONAME_RELEASE "libG4mpi.so"
)


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(GS20LightYieldCal GS20LightYieldCal.cc ${sources} ${headers})
target_link_libraries(GS20LightYieldCal ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build GS20Cal. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(GS20CAL_SCRIPTS
vis.mac
macros/co60Source.mac
macros/gRun.mac
macros/nRun.mac
macros/neutronSource.mac
)

foreach(_script ${GS20CAL_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(GS20Cal DEPENDS GS20LightYieldCal)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS GS20LightYieldCal DESTINATION bin)

