//===-- B3.h - Top-level interface for SPIR-V representation -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_B3_B3_H
#define LLVM_LIB_TARGET_B3_B3_H

#include "MCTargetDesc/B3MCTargetDesc.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/Target/TargetMachine.h"
#include "B3Subtarget.h"

namespace llvm {
class B3TargetMachine;
class InstructionSelector;

InstructionSelector *
createB3InstructionSelector(const B3TargetMachine &TM,
                               const B3Subtarget &Subtarget,
                               const RegisterBankInfo &RBI);
FunctionPass *createB3PhiLegalizerPass();
void initializeB3PhiLegalizerPass(PassRegistry &);
FunctionPass *createB3BranchLegalizerPass();
void initializeB3BranchLegalizerPass(PassRegistry &);

/*
ModulePass *createB3PrepareFunctionsPass(const B3TargetMachine &TM);
FunctionPass *createB3StripConvergenceIntrinsicsPass();
FunctionPass *createB3RegularizerPass();
FunctionPass *createB3PreLegalizerPass();
FunctionPass *createB3EmitIntrinsicsPass(B3TargetMachine *TM);

void initializeB3ModuleAnalysisPass(PassRegistry &);
void initializeB3ConvergenceRegionAnalysisWrapperPassPass(PassRegistry &);
void initializeB3PreLegalizerPass(PassRegistry &);
void initializeB3PostLegalizerPass(PassRegistry &);
void initializeB3EmitIntrinsicsPass(PassRegistry &);
*/
} // namespace llvm

#endif // LLVM_LIB_TARGET_B3_B3_H
