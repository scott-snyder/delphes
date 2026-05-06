include_directories(
  ${CMAKE_SOURCE_DIR}
  ${ROOT_INCLUDE_DIRS}
  ${DelphesExternals_INCLUDE_DIR}
)

file(GLOB sources *.cc)
file(GLOB headers *.h)
list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/ClassesLinkDef.h)

# the macro invocation for ROOT6 ensures that the headers are relocatable
if (NOT ${ROOT_VERSION} VERSION_LESS "6.0.0")
  DELPHES_GENERATE_DICTIONARY(ClassesDict
    classes/DelphesModule.h
    classes/DelphesFactory.h
    classes/SortableObject.h
    classes/DelphesClasses.h
    LINKDEF ClassesLinkDef.h
  )
else()
  # for ROOT5 the above fails, keep the following as workaround
  DELPHES_GENERATE_DICTIONARY(ClassesDict
  ${CMAKE_CURRENT_SOURCE_DIR}/DelphesModule.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DelphesFactory.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SortableObject.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DelphesClasses.h
    LINKDEF ClassesLinkDef.h
  )
endif()

add_library(classes OBJECT ${sources} ClassesDict.cxx)

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
endif()
