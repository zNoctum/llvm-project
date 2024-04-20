//===-- B3TargetMachine.h - Define TargetMachine for B3 -*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the B3 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_B3_B3TARGETMACHINE_H
#define LLVM_LIB_TARGET_B3_B3TARGETMACHINE_H

#include "B3Subtarget.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class B3TargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  B3Subtarget Subtarget;

public:
  B3TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM,
                     CodeGenOptLevel OL, bool JIT);

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  const B3Subtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  const B3Subtarget *getSubtargetImpl() const { return &Subtarget; }

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  bool addPassesToEmitMC(PassManagerBase &, MCContext *&, raw_pwrite_stream &,
                         bool = true) override {
    return true;
  }

  bool isMachineVerifierClean() const override { return false; }
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_B3_B3TARGETMACHINE_H