include_directories(
  ${FastJet_INCLUDE_DIRS}
  ${DelphesExternals_INCLUDE_DIR}
  ${ROOT_INCLUDE_DIRS}
)

file(GLOB sources *.cc)

add_library(PUPPI OBJECT ${sources})
