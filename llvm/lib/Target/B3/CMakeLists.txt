add_llvm_component_group(B3)

set(LLVM_TARGET_DEFINITIONS B3.td)

tablegen(LLVM B3GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM B3GenGlobalISel.inc -gen-global-isel)
tablegen(LLVM B3GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM B3GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM B3GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM B3GenRegisterBank.inc -gen-register-bank)

add_public_tablegen_target(B3CommonTableGen)

add_llvm_target(B3CodeGen
        B3TargetMachine.cpp
        B3RegisterInfo.cpp
        B3InstrInfo.cpp
        B3CallLowering.cpp
        B3AsmPrinter.cpp
        B3Subtarget.cpp
        B3RegisterBankInfo.cpp
        B3InstructionSelector.cpp
        B3LegalizerInfo.cpp
        B3PhiLegalizer.cpp
        B3ISelLowering.cpp

        LINK_COMPONENTS
        AsmPrinter
        CodeGen
        CodeGenTypes
        Core
        GlobalISel
        B3Desc
        B3Info
        Support
        Target

        ADD_TO_COMPONENT
        B3
)

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)