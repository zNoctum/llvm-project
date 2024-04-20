//===-- B3InstrInfo.h - SPIR-V Instruction Information -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the SPIR-V implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_B3_B3INSTRINFO_H
#define LLVM_LIB_TARGET_B3_B3INSTRINFO_H

#include "B3RegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "B3GenInstrInfo.inc"

namespace llvm {

class B3InstrInfo : public B3GenInstrInfo {
  const B3RegisterInfo RI;

public:
  B3InstrInfo();

  const B3RegisterInfo &getRegisterInfo() const { return RI; }
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_B3_B3INSTRINFO_H
