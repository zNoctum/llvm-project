//===-- B3TargetInfo.cpp - B3 Target Implementation ----*- C++ -*---===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/B3TargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheB3Target() {
  static Target TheB3Target;
  return TheB3Target;
}


extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeB3TargetInfo() {
  RegisterTarget<Triple::b3> X(getTheB3Target(), "b3",
                                    "Bare Bones Backend", "B3");
}