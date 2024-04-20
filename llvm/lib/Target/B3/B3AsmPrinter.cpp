//===-- B3AsmPrinter.cpp - SPIR-V LLVM assembly writer ------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the SPIR-V assembly language.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/B3InstPrinter.h"
#include "B3.h"
#include "B3InstrInfo.h"
//#include "B3MCInstLower.h"
//#include "B3ModuleAnalysis.h"
#include "B3Subtarget.h"
#include "B3TargetMachine.h"
//#include "B3Utils.h"
#include "TargetInfo/B3TargetInfo.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Object/IRSymtab.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
class B3AsmPrinter : public AsmPrinter {
  StringRef getPassName() const override { return DEBUG_TYPE; }
public:
  explicit B3AsmPrinter(TargetMachine &TM,
                           std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}
bool lowerOperand(const MachineOperand &MO,
                                   MCOperand &MCOp) {
  dbgs() << MO.getType();
  switch (MO.getType()) {
  default: llvm_unreachable("unknown operand type");
  case MachineOperand::MO_Register:
    MCOp = MCOperand::createReg(MO.getReg());
    break;
  case MachineOperand::MO_Immediate:
    MCOp = MCOperand::createImm(MO.getImm());
    break;
  case MachineOperand::MO_FPImmediate: {
    const ConstantFP *Cnt = MO.getFPImm();
    const APFloat &Val = Cnt->getValueAPF();

    switch (Cnt->getType()->getTypeID()) {
    default: report_fatal_error("Unsupported FP type");
    case Type::FloatTyID:
      MCOp = MCOperand::createSFPImm(Val.bitcastToAPInt().getZExtValue());
      break;
    case Type::DoubleTyID:
      MCOp = MCOperand::createDFPImm(Val.bitcastToAPInt().getZExtValue());
      break;
    }
    break;
  }
  case MachineOperand::MO_MachineBasicBlock:
    MCOp = MCOperand::createExpr(MCSymbolRefExpr::create(
        MO.getMBB()->getSymbol(), OutContext));
    break;
  //case MachineOperand::MO_ExternalSymbol:
  //  MCOp = GetSymbolRef(GetExternalSymbolSymbol(MO.getSymbolName()));
  //  break;
  //case MachineOperand::MO_GlobalAddress:
  //  MCOp = GetSymbolRef(getSymbol(MO.getGlobal()));
  //  break;
  }
  return true;
}
  void emitInstruction(const MachineInstr *I) override {
    MCInst Inst;

      Inst.setOpcode(I->getOpcode());
      for (unsigned i = 0, e = I->getNumOperands(); i != e; ++i) {
        const MachineOperand &MO = I->getOperand(i);

        MCOperand MCOp;

        if (lowerOperand(MO, MCOp))
          Inst.addOperand(MCOp);
      }

    EmitToStreamer(*OutStreamer, Inst);
  }
};
} // namespace

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeB3AsmPrinter() {
  RegisterAsmPrinter<B3AsmPrinter> X(getTheB3Target());
}