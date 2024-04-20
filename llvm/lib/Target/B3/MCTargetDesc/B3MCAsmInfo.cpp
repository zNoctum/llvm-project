//===-- B3MCAsmInfo.h - SPIR-V asm properties --------------*- C++ -*--====//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the B3MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "B3MCAsmInfo.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

B3MCAsmInfo::B3MCAsmInfo(const Triple &TT,
                               const MCTargetOptions &Options) {
  IsLittleEndian = true;

  CommentString = "//";

  HasSingleParameterDotFile = false;

  InlineAsmStart = " begin inline asm";
  InlineAsmEnd = " end inline asm";

  SupportsDebugInformation = false;
  HasFunctionAlignment = false;
  HasDotTypeDotSizeDirective = false;
  HiddenDeclarationVisibilityAttr = HiddenVisibilityAttr = MCSA_Invalid;
  ProtectedVisibilityAttr = MCSA_Invalid;

  Data8bitsDirective = ".b8 ";
  Data16bitsDirective = nullptr; // not supported
  Data32bitsDirective = ".b32 ";
  Data64bitsDirective = ".b64 ";
  ZeroDirective = ".b8";
  AsciiDirective = nullptr; // not supported
  AscizDirective = nullptr; // not supported
  SupportsQuotedNames = false;
  SupportsExtendedDwarfLocDirective = false;
  SupportsSignedData = false;

  PrivateGlobalPrefix = ".G";
  PrivateLabelPrefix = ".L";

  // @TODO: Can we just disable this?
  WeakDirective = "// .weak\t";
  GlobalDirective = "// .globl\t";

  UseIntegratedAssembler = false;

  // Avoid using parens for identifiers starting with $ - ptxas does
  // not expect them.
  UseParensForDollarSignNames = false;

  // ptxas does not support DWARF `.file fileno directory filename'
  // syntax as of v11.X.
  EnableDwarfFileDirectoryDefault = false;
}

bool B3MCAsmInfo::shouldOmitSectionDirective(StringRef SectionName) const {
  return true;
}
