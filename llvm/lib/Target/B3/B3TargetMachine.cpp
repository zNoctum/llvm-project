//===- B3TargetMachine.cpp - Define TargetMachine for B3 -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about B3 target spec.
//
//===----------------------------------------------------------------------===//

#include "B3.h"
#include "B3TargetMachine.h"
#include "TargetInfo/B3TargetInfo.h"
#include "llvm/CodeGen/GlobalISel/IRTranslator.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelect.h"
#include "llvm/CodeGen/GlobalISel/Legalizer.h"
#include "llvm/CodeGen/GlobalISel/RegBankSelect.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Transforms/Utils.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeB3Target() {
  RegisterTargetMachine<B3TargetMachine> X(getTheB3Target());

  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeGlobalISel(PR);
}

static std::string computeDataLayout(const Triple &TT) {
  return "e-m:e-p:64:64";
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  if (!RM)
    return Reloc::PIC_;
  return *RM;
}

B3TargetMachine::B3TargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this) {
  initAsmInfo();
  setGlobalISel(true);
  setFastISel(false);
  setO0WantsFastISel(false);
  setRequiresStructuredCFG(false);
}

namespace {
// B3 Code Generator Pass Configuration Options.
class B3PassConfig : public TargetPassConfig {
public:
  B3PassConfig(B3TargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  B3TargetMachine &getB3TargetMachine() const {
    return getTM<B3TargetMachine>();
  }

  void addIRPasses() override;
  void addISelPrepare() override;

  bool addIRTranslator() override;
  void addPreLegalizeMachineIR() override;
  bool addLegalizeMachineIR() override;
  bool addGlobalInstructionSelect() override;

  FunctionPass *createTargetRegisterAllocator(bool) override;
  void addFastRegAlloc() override {}
  void addOptimizedRegAlloc() override {}
  bool addRegBankSelect() override;

  void addPostRegAlloc() override;
};
} // namespace

TargetPassConfig *B3TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new B3PassConfig(*this, PM);
}

// We do not use physical registers, and maintain virtual registers throughout
// the entire pipeline, so return nullptr to disable register allocation.
FunctionPass *B3PassConfig::createTargetRegisterAllocator(bool) {
  return nullptr;
}

// Disable passes that break from assuming no virtual registers exist.
void B3PassConfig::addPostRegAlloc() {
  // Do not work with vregs instead of physical regs.
  disablePass(&MachineCopyPropagationID);
  disablePass(&PostRAMachineSinkingID);
  disablePass(&PostRASchedulerID);
  disablePass(&FuncletLayoutID);
  disablePass(&StackMapLivenessID);
  disablePass(&PatchableFunctionID);
  disablePass(&ShrinkWrapID);
  disablePass(&LiveDebugValuesID);
  disablePass(&MachineLateInstrsCleanupID);

  // Do not work with OpPhi.
  disablePass(&BranchFolderPassID);
  disablePass(&MachineBlockPlacementID);

  disablePass(&PrologEpilogCodeInserterID);

  TargetPassConfig::addPostRegAlloc();
}

void B3PassConfig::addIRPasses() {
  TargetPassConfig::addIRPasses();
}

void B3PassConfig::addISelPrepare() {
  TargetPassConfig::addISelPrepare();
}

bool B3PassConfig::addIRTranslator() {
  addPass(new IRTranslator(getOptLevel()));
  return false;
}

void B3PassConfig::addPreLegalizeMachineIR() {
  // addPass(createB3PreLegalizerPass());
}

// Use the default legalizer.
bool B3PassConfig::addLegalizeMachineIR() {
  addPass(new Legalizer());
  addPass(createB3PhiLegalizerPass());
  return false;
}

// Do not add the RegBankSelect pass, as we only ever need virtual registers.
bool B3PassConfig::addRegBankSelect() {
  addPass(new RegBankSelect());
  return false;
}

// Add the custom B3InstructionSelect from above.
bool B3PassConfig::addGlobalInstructionSelect() {
  addPass(new InstructionSelect());
  return false;
}
