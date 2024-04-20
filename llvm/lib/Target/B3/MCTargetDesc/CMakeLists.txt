add_llvm_component_library(LLVMB3Desc
  B3MCTargetDesc.cpp
  B3MCAsmInfo.cpp
  B3InstPrinter.cpp
  B3TargetStreamer.cpp

  LINK_COMPONENTS
  CodeGenTypes
  MC
  B3Info
  Support
  TargetParser

  ADD_TO_COMPONENT
  B3
)
