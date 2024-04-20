//===-- B3PhiLegalizer.cpp - Legalize PHI to B3 IR              -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Here we lower G_PHI into Phi() and Upsilon(...)
//
//===----------------------------------------------------------------------===//

#include "B3.h"
#include "B3Subtarget.h"
#include "B3Utils.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Target/TargetIntrinsicInfo.h"
#include "llvm/IR/IRBuilder.h"

#define DEBUG_TYPE "b3-phi-legalizer"

using namespace llvm;

namespace {
class B3PhiLegalizer : public MachineFunctionPass {
  void insertUpsilon(DebugLoc Loc, MachineBasicBlock *MBB, Register VReg, MachineOperand Value);
public:
  static char ID;
  B3PhiLegalizer() : MachineFunctionPass(ID) {
    initializeB3PhiLegalizerPass(*PassRegistry::getPassRegistry());
  }
  bool runOnMachineFunction(MachineFunction &MF) override;
  StringRef getPassName() const override { return "B3 PHI Legalizer"; }
};
} // namespace

void B3PhiLegalizer::insertUpsilon(DebugLoc Loc, MachineBasicBlock *MBB, Register VReg, MachineOperand Value) {
  auto TII = MBB->getParent()->getSubtarget().getInstrInfo();
  unsigned Opc = B3_TYPE_ADJUST(MBB->getParent()->getRegInfo().getRegClassOrNull(VReg), B3::Upsilon);
  if (MBB->empty()) {
    BuildMI(MBB, Loc, TII->get(Opc)).add(Value).addReg(VReg);
    return;
  }
  for (auto &MI : *MBB) {
    if (MI.isBranch()) {
      BuildMI(*MBB, MI, Loc, TII->get(Opc)).add(Value).addReg(VReg);
      break;
    }
  }
}

bool B3PhiLegalizer::runOnMachineFunction(MachineFunction &MF) {
  for (auto &MBB : MF) {
    if (MBB.empty() || !MBB.front().isPHI())
      continue;

    MachineBasicBlock::iterator LastPHIIt =
      std::prev(MBB.SkipPHIsAndLabels(MBB.begin()));
    MachineBasicBlock::iterator AfterPHIsIt = std::next(LastPHIIt);

    while (MBB.front().isPHI()) {
      MachineInstr *MPhi = MBB.remove(&*MBB.begin());

      for (unsigned i = 1; i < MPhi->getNumOperands(); i += 2) {
        insertUpsilon(MPhi->getDebugLoc(), MPhi->getOperand(i + 1).getMBB(), MPhi->getOperand(0).getReg(), MPhi->getOperand(i));
      }
      unsigned Opc = B3_TYPE_ADJUST(MF.getRegInfo().getRegClass(MPhi->getOperand(0).getReg()), B3::Phi);
      MBB.insert(AfterPHIsIt, BuildMI(MF, MPhi->getDebugLoc(), MF.getSubtarget().getInstrInfo()->get(Opc)).add(MPhi->getOperand(0)));
    }
  }

  // Theoretically we should update live variable info but due to us not having
  // a register allocator it should all be fine. i hope

  MF.getProperties().set(MachineFunctionProperties::Property::NoPHIs);

  return true;
}

INITIALIZE_PASS(B3PhiLegalizer, DEBUG_TYPE, "B3 PHI Legalizer", false,
                false)

char B3PhiLegalizer::ID = 0;

FunctionPass *llvm::createB3PhiLegalizerPass() {
  return new B3PhiLegalizer();
}