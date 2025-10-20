cmake_minimum_required(VERSION 3.5...4.0)

project(Delphes)

add_compile_options(-fPIC -Wall -D_FILE_OFFSET_BITS=64 -DDROP_CGAL)
# tcl vendored...
set(CMAKE_C_FLAGS  "-Wno-old-style-definition -Wno-address -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -Wno-unused-value")
if(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-non-prototype")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-overflow ")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

# Set the runtime path of the libraries by default but allow to switch it off:
option(SET_RPATH "Set runtime path of the ${project} libraries?" ON)
IF(SET_RPATH)
  # Set up the RPATH so executables find the libraries even when installed in non-default location
  SET(CMAKE_MACOSX_RPATH 1)
  SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  # Add the automatically determined parts of the RPATH which point to directories outside
  # the build tree to the install RPATH
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  # the RPATH to be used when installing, but only if it's not a system directory
  LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  IF(${isSystemDir} EQUAL -1)
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  ENDIF(${isSystemDir} EQUAL -1)
ENDIF(SET_RPATH)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Declare ROOT dependency
find_package(ROOT REQUIRED COMPONENTS EG Eve Gui GuiHtml)
set(CMAKE_CXX_STANDARD ${ROOT_CXX_STANDARD})

# Declare Pythia8 dependancy
find_package(Pythia8)
if(PYTHIA8_FOUND)
  include_directories(${PYTHIA8_INCLUDE_DIRS})
endif()

# Fastjet
FIND_PACKAGE( ILCUTIL REQUIRED COMPONENTS ILCSOFT_CMAKE_MODULES )
find_package(FastJet 3 REQUIRED COMPONENTS fastjetplugins fastjetcontribfragile fastjettools)

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR "lib")
endif()

function(DELPHES_GENERATE_DICTIONARY dictionary)
  if(${ROOT_VERSION} VERSION_LESS "6.0.0")
    ROOT_GENERATE_DICTIONARY(${dictionary} ${ARGN})
  else()
    ROOT_GENERATE_DICTIONARY(${dictionary} MODULE ${dictionary} ${ARGN})
  endif()
endfunction()

# Define location of all other external dependencies
set(DelphesExternals_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external)

add_subdirectory(classes)
add_subdirectory(converters)
add_subdirectory(display)
add_subdirectory(examples)
add_subdirectory(external)
add_subdirectory(modules)
add_subdirectory(readers)
add_subdirectory(cards)

add_library(Delphes SHARED
  $<TARGET_OBJECTS:classes>
  $<TARGET_OBJECTS:modules>
  $<TARGET_OBJECTS:ExRootAnalysis>
  $<TARGET_OBJECTS:tcl>
  $<TARGET_OBJECTS:Hector>
  $<TARGET_OBJECTS:PUPPI>
  $<TARGET_OBJECTS:TrackCovariance>
)

target_link_libraries(Delphes PUBLIC ROOT::Core ROOT::Hist ROOT::Tree ROOT::MathCore ROOT::Physics ROOT::EG ${FastJet_LIBRARIES} ${FastJet_COMPONENT_LIBRARIES} gfortran)

if(PYTHIA8_FOUND)
  target_link_libraries(Delphes PUBLIC ${PYTHIA8_LIBRARIES} ${CMAKE_DL_LIBS})
endif()

add_library(DelphesDisplay SHARED
  $<TARGET_OBJECTS:display>
)

target_link_libraries(DelphesDisplay PUBLIC Delphes ROOT::Geom ROOT::Graf3d ROOT::RGL ROOT::Eve ROOT::GuiHtml)

install(TARGETS Delphes DelphesDisplay DESTINATION lib)
