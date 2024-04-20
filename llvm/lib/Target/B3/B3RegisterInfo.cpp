//===-- SPIRVRegisterInfo.cpp - SPIR-V Register Information -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the SPIR-V implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "B3RegisterInfo.h"
#include "B3.h"
#include "llvm/CodeGen/MachineFunction.h"

#include "B3Subtarget.h"

#define GET_REGINFO_TARGET_DESC
#include "B3GenRegisterInfo.inc"
using namespace llvm;

B3RegisterInfo::B3RegisterInfo() : B3GenRegisterInfo(B3::I32_0) {}

BitVector B3RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  return BitVector(getNumRegs());
}

const MCPhysReg *
B3RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  static const MCPhysReg CalleeSavedReg = {0};
  return &CalleeSavedReg;
}
