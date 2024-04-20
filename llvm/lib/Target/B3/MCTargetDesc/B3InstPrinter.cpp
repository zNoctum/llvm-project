//===-- B3InstPrinter.cpp - Output SPIR-V MCInsts as ASM -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints a SPIR-V MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "B3InstPrinter.h"
#include "B3.h"
#include "B3InstrInfo.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;
using namespace llvm::B3;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#include "B3GenAsmWriter.inc"

void B3InstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                 StringRef Annot, const MCSubtargetInfo &STI,
                                 raw_ostream &O) {
  switch (MI->getOpcode()) {
  case TCCall:
  case CCallI32:
  case CCallI64:
  case CCallF32:
  case CCallF64:
  case CCallPTR:
  case CCallVoid: {
    printOperand(MI, 0, O);
    printCall(MI, 1, O);
    break;
  }
  default:
    printInstruction(MI, Address, O);
  }
}

void B3InstPrinter::printCall(const MCInst *MI, unsigned Skip, raw_ostream &O) {
    unsigned NumOps = MI->getNumOperands();
  O << "(";
  if (NumOps > Skip)
    printOperand(MI, Skip, O);
  for (unsigned OpNo = Skip + 1; OpNo < NumOps; OpNo++) {
    O << ", ";
    printOperand(MI, OpNo, O);
  }
  O << ")";
}


void B3InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O, const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");

  if (OpNo < MI->getNumOperands()) {
    const MCOperand &Op = MI->getOperand(OpNo);
    if (Op.isReg())
      O << '@' << (Register::virtReg2Index(Op.getReg()) + 1);
    else if (Op.isImm())
      O << formatImm(Op.getImm());
    else if (Op.isSFPImm())
      O << formatImm(Op.getSFPImm());
    else if (Op.isDFPImm())
      O << formatImm(Op.getDFPImm());
    else if (Op.isExpr())
      Op.getExpr()->print(O, &MAI, false);
    else {
      llvm_unreachable("Unexpected operand type");
    }
  }
}

void B3InstPrinter::printStringImm(const MCInst *MI, unsigned OpNo,
                                      raw_ostream &O) {
  report_fatal_error("TODO: printStringImm");
}