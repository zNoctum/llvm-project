//===--- B3Utils.h ---- B3 Utility Functions -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains miscellaneous utility functions.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_B3_B3UTILS_H
#define LLVM_LIB_TARGET_B3_B3UTILS_H

#include "B3Subtarget.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

namespace llvm {

#define B3_TYPE_ADJUST(TRC, v)  typeAdjust(TRC, v##I32, v##I64, v##F32, v##F64, v##PTR)
template<typename T>
static const T typeAdjust(const llvm::TargetRegisterClass *TRC, T i32, T i64, T f32, T f64, T ptr) {
  switch (TRC->getID()) {
  case B3::I32RegClassID:
    return i32;
  case B3::I64RegClassID:
    return i64;
  case B3::F32RegClassID:
    return f32;
  case B3::F64RegClassID:
    return f64;
  case B3::PTRRegClassID:
    return ptr;
  default:
    report_fatal_error("unexpected reg class!");
  }
}

static bool isB3Return(unsigned Opc) {
  switch (Opc) {
  case B3::RetI32:
  case B3::RetI64:
  case B3::RetF32:
  case B3::RetF64:
  case B3::RetVoid:
    return true;
  default:
    return false;
  }
}
} // llvm

#endif // LLVM_LIB_TARGET_B3_B3UTILS_H
