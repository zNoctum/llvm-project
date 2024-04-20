//===- B3LegalizerInfo.cpp --- SPIR-V Legalization Rules ------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the targeting of the Machinelegalizer class for SPIR-V.
//
//===----------------------------------------------------------------------===//

#include "B3LegalizerInfo.h"
#include "B3RegisterBankInfo.h"
#include "B3.h"
#include "B3Subtarget.h"

#include "llvm/CodeGen/GlobalISel/LegalizerHelper.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"

using namespace llvm;
using namespace llvm::LegalizeActions;
using namespace llvm::LegalityPredicates;
using namespace TargetOpcode;

B3LegalizerInfo::B3LegalizerInfo(const B3Subtarget &ST) {

  this->ST = &ST;

  const LLT p0  = LLT::pointer(0, 64);
  const LLT s8  = LLT::scalar(8);
  const LLT s16 = LLT::scalar(16);
  const LLT s32 = LLT::scalar(32);
  const LLT s64 = LLT::scalar(64);

  auto allScalars = {s32, s64};
  auto allScalarsAndPointers = {s32, s64, p0};


  getActionDefinitionsBuilder(G_IMPLICIT_DEF).legalFor(allScalarsAndPointers);

  getActionDefinitionsBuilder(G_PHI)
    .legalFor(allScalars)
    .clampScalar(0, s32, s64);
  getActionDefinitionsBuilder(G_BR).alwaysLegal();

  getActionDefinitionsBuilder({G_ZEXT, G_SEXT}).custom();
  getActionDefinitionsBuilder(G_TRUNC).alwaysLegal();

  getActionDefinitionsBuilder({G_ICMP, G_FCMP})
    .legalFor(allScalars)
    .widenScalarToNextPow2(1)
    .clampScalar(0, s32, s64);
  getActionDefinitionsBuilder(G_BRCOND).clampScalar(0, s32, s64).legalFor(allScalars);
  getActionDefinitionsBuilder({G_CONSTANT, G_FCONSTANT}).legalFor(allScalars).clampScalar(0, s32, s64).widenScalarToNextPow2(0);
  getActionDefinitionsBuilder({G_ADD, G_SUB, G_MUL, G_AND, G_OR, G_XOR})
    .legalFor(allScalars)
    .widenScalarToNextPow2(0)
    .clampScalar(0, s32, s64);
  getActionDefinitionsBuilder({G_FADD, G_FSUB, G_FMUL, G_FDIV, G_FREM})
    .legalFor(allScalars)
    .widenScalarToNextPow2(0)
    .clampScalar(0, s32, s64);
  getActionDefinitionsBuilder(G_PTRTOINT).legalForCartesianProduct({s64}, {p0});

  getActionDefinitionsBuilder(G_LOAD).legalForCartesianProduct({p0}, {s32, s64}).custom();
  getActionDefinitionsBuilder(G_STORE).alwaysLegal();
  getActionDefinitionsBuilder({G_INDEXED_LOAD, G_INDEXED_STORE}).clampScalar(0, s32, s64).alwaysLegal();
  getActionDefinitionsBuilder(G_PTR_ADD).legalForCartesianProduct({p0}, allScalars);

  getLegacyLegalizerInfo().computeTables();
  verify(*ST.getInstrInfo());
}


bool B3LegalizerInfo::legalizeCustom(
    LegalizerHelper &Helper, MachineInstr &MI,
    LostDebugLocObserver &LocObserver) const {
  MachineRegisterInfo &MRI = MI.getMF()->getRegInfo();
  unsigned Opc = MI.getOpcode();
  switch (Opc) {
  case G_LOAD: {
    Register Reg = MI.getOperand(0).getReg();
    unsigned Size = MRI.getType(Reg).getSizeInBits();
    switch (Size) {
    case 8:
    case 16:
      MRI.setType(Reg, LLT::scalar(32));
      Helper.MIRBuilder.buildInstr(Size == 16 ? B3::LoadZ16 : B3::LoadZ8).addDef(Reg).add(MI.getOperand(1)).addImm(0);
      MRI.setRegClass(Reg, &B3::I32RegClass);
      MI.eraseFromParent();
      return true;
    case 32:
    case 64:
      Helper.MIRBuilder.buildInstr(Size == 64 ? B3::LoadI64 : B3::LoadI32).addDef(Reg).add(MI.getOperand(1)).addImm(0);
      MRI.setRegClass(Reg, Size == 64 ? &B3::I64RegClass : &B3::I32RegClass);
      MI.eraseFromParent();
      return true;
    default:
      MI.print(dbgs());
      report_fatal_error("unexpected load size in legalizer");
    }
    return true;
  }
  case G_SEXT:
  case G_ZEXT: {
    Register InReg = MI.getOperand(1).getReg();
    Register OutReg = MI.getOperand(0).getReg();
    unsigned InSize = MRI.getType(InReg).getSizeInBits();
    unsigned OutSize = MRI.getType(OutReg).getSizeInBits();
    if (OutSize == 64) {
      if (InSize < 32 && Opc == G_SEXT) {
        Register NewInReg = MRI.createGenericVirtualRegister(LLT::scalar(32));
        Helper.MIRBuilder.buildInstr(InSize == 16 ? B3::SExt8 : B3::SExt16).addDef(NewInReg).addUse(InReg);
        InReg = NewInReg;
      }
      Helper.MIRBuilder.buildInstr(Opc == G_SEXT ? B3::SExt32 : B3::ZExt32).addDef(OutReg).addUse(InReg);
      MI.eraseFromParent();
    } else {
      dbgs() << MRI.getType(MI.getOperand(0).getReg()) << " <- " << MRI.getType(MI.getOperand(1).getReg()) << "\n";
      report_fatal_error("?");
    }
    return true;
  }
  default:
    report_fatal_error("unexpected gMIR instruction for custom legalization!");
  }
  // TODO: implement legalization for other opcodes.
  return true;
}