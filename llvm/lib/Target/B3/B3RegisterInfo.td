//B3RegisterInfo.td-Describe the B3 Registers -*- tablegen -*-
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file describes the B3 register classes and some nominal
/// physical registers.
///
//===----------------------------------------------------------------------===//

class B3Reg<string n> : Register<n> {
  let Namespace = "B3";
}

class B3RegClass<list<ValueType> regTypes, int alignment, dag regList>
     : RegisterClass<"B3", regTypes, alignment, regList>;

//===----------------------------------------------------------------------===//
// Registers
//===----------------------------------------------------------------------===//

// The register allocation framework requires register classes have at least
// one register, so we define a few for the integer / floating point register
// classes since we otherwise don't need a physical register in those classes.
// These are also used a "types" in the generated assembly matcher.
def I32_0 : B3Reg<"%i32.0">;
def I64_0 : B3Reg<"%i64.0">;
def F32_0 : B3Reg<"%f32.0">;
def F64_0 : B3Reg<"%f64.0">;
def PTR_0 : B3Reg<"%ptr.0">;


//===----------------------------------------------------------------------===//
//  Register classes
//===----------------------------------------------------------------------===//

def I32 : B3RegClass<[i32], 32, (add I32_0)>;
def I64 : B3RegClass<[i64], 64, (add I64_0)>;
def F32 : B3RegClass<[f32], 32, (add F32_0)>;
def F64 : B3RegClass<[f64], 64, (add F64_0)>;
def PTR : B3RegClass<[i64], 64, (add PTR_0)>;
