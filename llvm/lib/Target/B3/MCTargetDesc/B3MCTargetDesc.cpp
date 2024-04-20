//===-- B3MCTargetDesc.cpp - SPIR-V Target Descriptions ----*- C++ -*---===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides SPIR-V specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "B3MCTargetDesc.h"
#include "B3InstPrinter.h"
#include "B3MCAsmInfo.h"
#include "B3TargetStreamer.h"
#include "TargetInfo/B3TargetInfo.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "B3GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "B3GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "B3GenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createB3MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitB3MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createB3MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  return X;
}

static MCSubtargetInfo *
createB3MCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createB3MCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

namespace {

class B3MCInstrAnalysis : public MCInstrAnalysis {
public:
  explicit B3MCInstrAnalysis(const MCInstrInfo *Info)
      : MCInstrAnalysis(Info) {}
};

static MCInstPrinter *createB3MCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  assert(SyntaxVariant == 0);
  return new B3InstPrinter(MAI, MII, MRI);
}

static MCTargetStreamer *createTargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &,
                                                 MCInstPrinter *, bool) {
  return new B3AsmTargetStreamer(S);
}

static MCTargetStreamer *createTargetNullStreamer(MCStreamer &S) {
  return new B3TargetStreamer(S);
}


} // end anonymous namespace

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeB3TargetMC() {
  Target *T = &getTheB3Target();
  RegisterMCAsmInfo<B3MCAsmInfo> X(*T);
  TargetRegistry::RegisterMCInstrInfo(*T, createB3MCInstrInfo);
  TargetRegistry::RegisterMCRegInfo(*T, createB3MCRegisterInfo);
  TargetRegistry::RegisterMCSubtargetInfo(*T, createB3MCSubtargetInfo);
  TargetRegistry::RegisterMCInstPrinter(*T, createB3MCInstPrinter);
  TargetRegistry::RegisterAsmTargetStreamer(*T, createTargetAsmStreamer);
  TargetRegistry::RegisterNullTargetStreamer(*T, createTargetNullStreamer);
}
