//===- B3RegisterBankInfo.h -----------------------------------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the targeting of the RegisterBankInfo class for B3.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_B3_B3REGISTERBANKINFO_H
#define LLVM_LIB_TARGET_B3_B3REGISTERBANKINFO_H

#include "llvm/CodeGen/RegisterBank.h"
#include "llvm/CodeGen/RegisterBankInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGBANK_DECLARATIONS
#include "B3GenRegisterBank.inc"



namespace llvm {

class TargetRegisterInfo;

class B3GenRegisterBankInfo : public RegisterBankInfo {
protected:
  enum PartialMappingIdx {
    PMI_None = -1,
    PMI_I32 = 1,
    PMI_I64 = 2,
    PMI_F32 = 3,
    PMI_F64 = 4,
    PMI_PTR = 5,
    PMI_Min = PMI_I32,
  };

  static const RegisterBankInfo::PartialMapping PartMappings[];
  static const RegisterBankInfo::ValueMapping ValMappings[];
  static const PartialMappingIdx BankIDToCopyMapIdx[];

  /// Get the pointer to the ValueMapping representing the RegisterBank
  /// at \p RBIdx.
  ///
  /// The returned mapping works for instructions with the same kind of
  /// operands for up to 3 operands.
  ///
  /// \pre \p RBIdx != PartialMappingIdx::None
  static const RegisterBankInfo::ValueMapping *
  getValueMapping(PartialMappingIdx RBIdx);

  /// Get the pointer to the ValueMapping of the operands of a copy
  /// instruction from the \p SrcBankID register bank to the \p DstBankID
  /// register bank with a size of \p Size.
  static const RegisterBankInfo::ValueMapping *
  getCopyMapping(unsigned DstBankID, unsigned SrcBankID, unsigned Size);

#define GET_TARGET_REGBANK_CLASS
#include "B3GenRegisterBank.inc"
};

// This class provides the information for the target register banks.
class B3RegisterBankInfo final : public B3GenRegisterBankInfo {
public:
  B3RegisterBankInfo(const TargetRegisterInfo &TRI);

  const RegisterBank &getRegBankFromRegClass(const TargetRegisterClass &RC,
                                             LLT Ty) const override;
  const InstructionMapping &
  getInstrMapping(const MachineInstr &MI) const override;

  InstructionMappings
  getInstrAlternativeMappings(const MachineInstr &MI) const override;

private:
  /// Maximum recursion depth for hasFPConstraints.
  const unsigned MaxFPRSearchDepth = 2;

  /// \returns true if \p MI only uses and defines FPRs.
  bool hasFPConstraints(const MachineInstr &MI, const MachineRegisterInfo &MRI,
                        const TargetRegisterInfo &TRI,
                        unsigned Depth = 0) const;

  /// \returns true if \p MI only uses FPRs.
  bool onlyUsesFP(const MachineInstr &MI, const MachineRegisterInfo &MRI,
                  const TargetRegisterInfo &TRI, unsigned Depth = 0) const;

  /// \returns true if \p MI only defines FPRs.
  bool onlyDefinesFP(const MachineInstr &MI, const MachineRegisterInfo &MRI,
                     const TargetRegisterInfo &TRI, unsigned Depth = 0) const;
};
} // namespace llvm
#endif // LLVM_LIB_TARGET_B3_B3REGISTERBANKINFO_H
