//===-- B3RegisterInfo.h - B3 Register Information -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the B3 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_B3_B3REGISTERINFO_H
#define LLVM_LIB_TARGET_B3_B3REGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "B3GenRegisterInfo.inc"

namespace llvm {

struct B3RegisterInfo : public B3GenRegisterInfo {
  B3RegisterInfo();
  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;
  BitVector getReservedRegs(const MachineFunction &MF) const override;
  bool eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override {
    return false;
  }
  Register getFrameRegister(const MachineFunction &MF) const override {
    return 0;
  }
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_B3_B3REGISTERINFO_H
