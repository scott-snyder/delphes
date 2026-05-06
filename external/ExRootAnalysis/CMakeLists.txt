include_directories(
  ${DelphesExternals_INCLUDE_DIR}
  ${ROOT_INCLUDE_DIRS}
)

file(GLOB sources *.cc)
file(GLOB headers *.h)

list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/ExRootAnalysisLinkDef.h)

# the macro invocation for ROOT6 ensures that the headers are relocatable
if (NOT ${ROOT_VERSION} VERSION_LESS "6.0.0")
  DELPHES_GENERATE_DICTIONARY(ExRootAnalysisDict
    ExRootAnalysis/ExRootClassifier.h
    ExRootAnalysis/ExRootConfReader.h
    ExRootAnalysis/ExRootFilter.h
    ExRootAnalysis/ExRootProgressBar.h
    ExRootAnalysis/ExRootResult.h
    ExRootAnalysis/ExRootTreeBranch.h
    ExRootAnalysis/ExRootTreeReader.h
    ExRootAnalysis/ExRootTreeWriter.h
    ExRootAnalysis/ExRootUtilities.h
    LINKDEF ExRootAnalysisLinkDef.h)
else()
  # for ROOT5 the above fails, keep the following as workaround
  DELPHES_GENERATE_DICTIONARY(ExRootAnalysisDict ${headers} LINKDEF ExRootAnalysisLinkDef.h)
endif()

add_library(ExRootAnalysis OBJECT ${sources} ExRootAnalysisDict.cxx)

# Needed when building in ABS_SYMLINK mode.
# Cling tries to load the PCM file for a library in the same directory
# as the library, after resolving symlinks.
# But the libraries are built in the build/PACK/lib directory and the PCM
# files in build/PACK/rootdict.  They both get symlinked in inst/lib,
# but that doesn't help.  So put a link to the PCM in build/PACK/lib.
add_custom_target( pcmlinkRootAna
                   COMMAND  ln -sf ${PROJECT_BINARY_DIR}/external/ExRootAnalysis/libExRootAnalysisDict_rdict.pcm ${PROJECT_BINARY_DIR} )
add_dependencies( ExRootAnalysis pcmlinkRootAna )

# install headers
install(FILES ${headers} DESTINATION include/ExRootAnalysis)

# install pcms if they are created
if (NOT ${ROOT_VERSION} VERSION_LESS "6.0.0")
  install(FILES
      ${PROJECT_BINARY_DIR}/external/ExRootAnalysis/libExRootAnalysisDict_rdict.pcm
      DESTINATION lib)
endif()
