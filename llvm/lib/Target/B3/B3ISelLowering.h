//===-- B3ISelLowering.h - SPIR-V DAG Lowering Interface -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that SPIR-V uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_B3_B3ISELLOWERING_H
#define LLVM_LIB_TARGET_B3_B3ISELLOWERING_H

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
class B3Subtarget;

class B3TargetLowering : public TargetLowering {
  const B3Subtarget &STI;

public:
  explicit B3TargetLowering(const TargetMachine &TM, const B3Subtarget &ST);

  bool isLegalAddressingMode(const DataLayout &DL,
                                             const AddrMode &AM,
                                             Type *Ty, unsigned AS,
                                             Instruction *I) const override;

  bool allowsMisalignedMemoryAccesses(
    EVT /*VT*/, unsigned /*AddrSpace*/, Align /*Align*/,
    MachineMemOperand::Flags /*Flags*/, unsigned *Fast) const override;

  // This is to prevent sexts of non-i64 vector indices which are generated
  // within general IRTranslator hence type generation for it is omitted.
  MVT getVectorIdxTy(const DataLayout &DL) const override {
    return MVT::getIntegerVT(32);
  }

  // Call the default implementation and finalize target lowering by inserting
  // extra instructions required to preserve validity of SPIR-V code imposed by
  // the standard.
  void finalizeLowering(MachineFunction &MF) const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_B3_B3ISELLOWERING_H
