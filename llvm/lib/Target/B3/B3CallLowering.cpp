//===----------------------------------------------------------------------===//
//
// This file implements the lowering of LLVM calls to machine code calls for
// GlobalISel.
//
//===----------------------------------------------------------------------===//

#include "B3CallLowering.h"
#include "B3.h"
#include "B3InstrInfo.h"
#include "B3RegisterBankInfo.h"
#include "B3RegisterInfo.h"
#include "B3Subtarget.h"
#include "B3Utils.h"

#include "llvm/CodeGen/FunctionLoweringInfo.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Support/ModRef.h"

using namespace llvm;

const static TargetRegisterClass *tyToTRC(Type *Ty) {
  if (Ty->isIntegerTy(32))
    return &B3::I32RegClass;
  if (Ty->isIntegerTy(64))
    return &B3::I64RegClass;
  if (Ty->isFloatTy())
    return &B3::F32RegClass;
  if (Ty->isDoubleTy())
    return &B3::F64RegClass;
  if (Ty->isPointerTy())
    return &B3::PTRRegClass;
  report_fatal_error("Trying to emit unsupported type!");
}

B3CallLowering::B3CallLowering(const B3TargetLowering &TLI)
    : CallLowering(&TLI) {}

bool B3CallLowering::lowerReturn(MachineIRBuilder &MIRBuilder,
                                    const Value *Val, ArrayRef<Register> VRegs,
                                    FunctionLoweringInfo &FLI,
                                    Register SwiftErrorVReg) const {
  // Currently all return types should use a single register.
  // TODO: handle the case of multiple registers.
  if (VRegs.size() > 1)
    return false;

  const auto &STI = MIRBuilder.getMF().getSubtarget();
  if (Val) {
    Register VReg = VRegs[0];

    const TargetRegisterClass *TRC = tyToTRC(Val->getType());
    unsigned Opc = B3_TYPE_ADJUST(TRC, B3::Ret);
    MIRBuilder.getMRI()->setRegClass(VReg, TRC);
    return MIRBuilder.buildInstr(Opc).addUse(VReg).constrainAllUses(MIRBuilder.getTII(), *STI.getRegisterInfo(), *STI.getRegBankInfo());
  }
  return MIRBuilder.buildInstr(B3::RetVoid).constrainAllUses(MIRBuilder.getTII(), *STI.getRegisterInfo(), *STI.getRegBankInfo());
}

bool B3CallLowering::lowerFormalArguments(MachineIRBuilder &MIRBuilder,
                                             const Function &F,
                                             ArrayRef<ArrayRef<Register>> VRegs,
                                             FunctionLoweringInfo &FLI) const {
  if (VRegs.size() == 0)
    return true;

  const auto &MRI = MIRBuilder.getMRI();
  unsigned i = 0;
  for (const auto &Arg : F.args()) {
    if (VRegs[i].size() > 1)
      return false;

    const TargetRegisterClass *TRC = tyToTRC(Arg.getType());
    unsigned Opc = B3_TYPE_ADJUST(TRC, B3::Arg);
    MIRBuilder.buildInstr(Opc).addDef(VRegs[i][0]).addImm(i);
    MRI->setRegClass(VRegs[i][0], TRC);
    ++i;
  }
  return true;
}

bool B3CallLowering::lowerCall(MachineIRBuilder &MIRBuilder,
                                  CallLoweringInfo &Info) const {
  //assert(Info.OrigRet.Regs.size() <= 1 && "Expect call returns to only have one or no return reg!");
  if (Info.OrigRet.Regs.size() > 1) {
    dbgs() << Info.Callee;
    report_fatal_error("fuck");
  }
  unsigned Opc;
  if (Info.IsTailCall)
    Opc = B3::TCCall;
  else if (Info.OrigRet.Regs.empty())
    Opc = B3::CCallVoid;
  else
    Opc = B3_TYPE_ADJUST(tyToTRC(Info.OrigRet.Ty), B3::CCall);
  auto MIB = MIRBuilder.buildInstr(Opc);
  for (auto Arg : Info.OrigArgs) {
    assert(Arg.Regs.size() == 1 && "Call arg has multiple VRegs");
    MIB.addUse(Arg.Regs[0]);
  }
  return true;
}
