include_directories(
  ${CMAKE_SOURCE_DIR}
  ${ROOT_INCLUDE_DIRS}
  ${DelphesExternals_INCLUDE_DIR}
)

file(GLOB sources *.cc)
file(GLOB headers *.h)
list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/ClassesLinkDef.h)
list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/ClassesPythia8LinkDef.h)

DELPHES_GENERATE_DICTIONARY(ClassesDict ${headers} LINKDEF ClassesLinkDef.h)

if(PYTHIA8_FOUND)
  DELPHES_GENERATE_DICTIONARY(ClassesPythia8Dict ${headers} LINKDEF ClassesPythia8LinkDef.h)
  add_library(classes OBJECT ${sources} ClassesDict.cxx ClassesPythia8Dict.cxx)
else()
  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/DelphesPythia8Reader.cc)
  add_library(classes OBJECT ${sources} ClassesDict.cxx)
endif()


# Needed when building in ABS_SYMLINK mode.
# Cling tries to load the PCM file for a library in the same directory
# as the library, after resolving symlinks.
# But the libraries are built in the build/PACK/lib directory and the PCM
# files in build/PACK/rootdict.  They both get symlinked in inst/lib,
# but that doesn't help.  So put a link to the PCM in build/PACK/lib.
add_custom_target( pcmlinkClasses
                   COMMAND  ln -sf ${PROJECT_BINARY_DIR}/classes/libClassesDict_rdict.pcm ${PROJECT_BINARY_DIR} )
add_dependencies( classes pcmlinkClasses )

# install public headers
install(FILES ${headers} DESTINATION include/classes)

# install pcms if they are created
if (NOT ${ROOT_VERSION} VERSION_LESS "6.0.0")
  install(FILES
      ${PROJECT_BINARY_DIR}/classes/libClassesDict_rdict.pcm
      DESTINATION lib)
  if(PYTHIA8_FOUND)
    install(FILES
      ${PROJECT_BINARY_DIR}/classes/libClassesPythia8Dict_rdict.pcm
      DESTINATION lib)
  endif()
endif()
