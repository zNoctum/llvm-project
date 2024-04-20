add_llvm_component_library(LLVMB3Info
  B3TargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  B3
  )