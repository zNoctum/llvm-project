//===- B3ISelLowering.cpp - SPIR-V DAG Lowering Impl ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the B3TargetLowering class.
//
//===----------------------------------------------------------------------===//

#include "B3ISelLowering.h"
#include "B3.h"
#include "B3InstrInfo.h"
#include "B3RegisterBankInfo.h"
#include "B3RegisterInfo.h"
#include "B3Subtarget.h"
#include "B3TargetMachine.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

#define DEBUG_TYPE "b3-lower"

using namespace llvm;

B3TargetLowering::B3TargetLowering(const TargetMachine &TM,
                                   const B3Subtarget &ST)
    : TargetLowering(TM), STI(ST) {
  setBooleanContents(ZeroOrOneBooleanContent);

  addRegisterClass(MVT::i32, &B3::I32RegClass);
  addRegisterClass(MVT::i64, &B3::I64RegClass);
  addRegisterClass(MVT::f32, &B3::F32RegClass);
  addRegisterClass(MVT::f64, &B3::F64RegClass);
  addRegisterClass(MVT::i64, &B3::PTRRegClass);

  computeRegisterProperties(STI.getRegisterInfo());


  setLoadExtAction(ISD::EXTLOAD, MVT::f64, MVT::f32, Expand);
  setTruncStoreAction(MVT::f64, MVT::f32, Expand);
  for (auto T : MVT::integer_valuetypes())
    for (auto Ext : {ISD::EXTLOAD, ISD::ZEXTLOAD, ISD::SEXTLOAD})
      setLoadExtAction(Ext, T, MVT::i1, Promote);

  for (auto Op :
       {ISD::BSWAP, ISD::SMUL_LOHI, ISD::UMUL_LOHI, ISD::MULHS, ISD::MULHU,
        ISD::SDIVREM, ISD::UDIVREM, ISD::SHL_PARTS, ISD::SRA_PARTS,
        ISD::SRL_PARTS, ISD::ADDC, ISD::ADDE, ISD::SUBC, ISD::SUBE}) {
    for (auto T : {MVT::i32, MVT::i64})
      setOperationAction(Op, T, Expand);
  }

  for (auto Op : {ISD::Constant, ISD::ConstantFP})
    for (auto T : {MVT::i32, MVT::i64})
      setOperationAction(Op, T, Custom);

  for (auto T : {MVT::i32, MVT::i64})
    setOperationAction(ISD::ADD, T, Expand);

  setOperationAction(ISD::BR_JT, MVT::Other, Custom);
  //setOperationAction(ISD::UNDEF, MVT::i64, Custom);
  setMinimumJumpTableEntries(3);
}

bool B3TargetLowering::isLegalAddressingMode(const DataLayout &DL,
                                             const AddrMode &AM,
                                             Type *Ty, unsigned AS,
                                             Instruction *I) const {
  if (!isInt<32>(AM.BaseOffs))
    return false;

  if (AM.Scale != 0)
    return false;
  return true;
}

bool B3TargetLowering::allowsMisalignedMemoryAccesses(
    EVT /*VT*/, unsigned /*AddrSpace*/, Align /*Align*/,
    MachineMemOperand::Flags /*Flags*/, unsigned *Fast) const {
  if (Fast)
    *Fast = 1;
  return true;
}

// to pre-IRTranslation passes eventually
void B3TargetLowering::finalizeLowering(MachineFunction &MF) const {
  TargetLowering::finalizeLowering(MF);
}
