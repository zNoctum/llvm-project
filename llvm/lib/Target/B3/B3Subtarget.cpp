//===-- B3Subtarget.cpp - SPIR-V Subtarget Information ------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the SPIR-V specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "B3Subtarget.h"
#include "B3.h"
#include "B3TargetMachine.h"
#include "B3RegisterBankInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/TargetParser/Host.h"

using namespace llvm;

#define DEBUG_TYPE "spirv-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "B3GenSubtargetInfo.inc"

const CallLowering *B3Subtarget::getCallLowering() const {
  return CallLoweringInfo.get();
}
const B3InstrInfo *B3Subtarget::getInstrInfo() const {
  return &InstrInfo;
}
const B3FrameLowering *B3Subtarget::getFrameLowering() const {
  return &FrameLowering;
}
const B3TargetLowering *B3Subtarget::getTargetLowering() const {
  return &TLInfo;
}
const B3RegisterInfo *B3Subtarget::getRegisterInfo() const {
  return &InstrInfo.getRegisterInfo();
}
InstructionSelector *B3Subtarget::getInstructionSelector() const {
  return InstSelector.get();
}
const B3LegalizerInfo *B3Subtarget::getLegalizerInfo() const {
  return LegalizerInfo.get();
}

const RegisterBankInfo *B3Subtarget::getRegBankInfo() const {
  return RegBankInfo.get();
}

B3Subtarget::B3Subtarget(const Triple &TT, const std::string &CPU,
                               const std::string &FS,
                               const B3TargetMachine &TM)
    : B3GenSubtargetInfo(TT, CPU, /*TuneCPU=*/CPU, FS),
      InstrInfo(),
      FrameLowering(initSubtargetDependencies(CPU, FS)), TLInfo(TM, *this)
      /*TargetTriple(TT)*/ {
  CallLoweringInfo = std::make_unique<B3CallLowering>(TLInfo);
  LegalizerInfo = std::make_unique<B3LegalizerInfo>(*this);
  auto *RBI = new B3RegisterBankInfo(*getRegisterInfo());
  RegBankInfo.reset(RBI);
  InstSelector.reset(
      createB3InstructionSelector(TM, *this, *RegBankInfo.get()));
}

B3Subtarget &B3Subtarget::initSubtargetDependencies(StringRef CPU,
                                                          StringRef FS) {
  return *this;
}

bool B3Subtarget::canDirectlyComparePointers() const {
  return true;
}