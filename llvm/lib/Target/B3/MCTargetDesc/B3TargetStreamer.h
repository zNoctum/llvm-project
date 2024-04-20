//===-- B3TargetStreamer.h - B3 Target Streamer ----------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LIB_TARGET_B3_MCTARGETDESC_B3TARGETSTREAMER_H
#define LIB_TARGET_B3_MCTARGETDESC_B3TARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"

namespace llvm {
class MCSection;

class B3TargetStreamer : public MCTargetStreamer {
private:
  SmallVector<std::string, 4> DwarfFiles;
  bool HasSections = false;
public:
  B3TargetStreamer(MCStreamer &S);
  ~B3TargetStreamer() override;

  void outputDwarfFileDirectives();
  void closeLastSection();

  void emitDwarfFileDirective(StringRef Directive) override;
  void changeSection(const MCSection *CurSection, MCSection *Section,
                     const MCExpr *SubSection, raw_ostream &OS) override;

  void emitRawBytes(StringRef Data) override;
};

class B3AsmTargetStreamer : public B3TargetStreamer {
public:
  B3AsmTargetStreamer(MCStreamer &);
  ~B3AsmTargetStreamer() override;
};
} // namespace llvm

#endif // LIB_TARGET_B3_MCTARGETDESC_B3TARGETSTREAMER_H
