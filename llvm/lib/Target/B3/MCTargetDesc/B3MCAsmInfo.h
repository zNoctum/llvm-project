//===-- B3MCAsmInfo.h - SPIR-V asm properties --------------*- C++ -*--====//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the B3MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_B3_MCTARGETDESC_B3MCASMINFO_H
#define LLVM_LIB_TARGET_B3_MCTARGETDESC_B3MCASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {

class Triple;

class B3MCAsmInfo : public MCAsmInfo {
public:
  explicit B3MCAsmInfo(const Triple &TT, const MCTargetOptions &Options);
  bool shouldOmitSectionDirective(StringRef SectionName) const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_B3_MCTARGETDESC_B3MCASMINFO_H
