//===-- B3.td - Describe the B3 Target Machine ------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

include "B3RegisterInfo.td"
include "B3RegisterBanks.td"
include "B3InstrInfo.td"

def B3InstrInfo : InstrInfo;

def B3InstPrinter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  bit isMCAsmWriter = 1;
}

def B3 : Target {
  let InstructionSet = B3InstrInfo;
  let AssemblyWriters = [B3InstPrinter];
}