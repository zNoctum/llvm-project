//===-- B3RegisterBanks.td - Describe B3 RegBanks ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

def I32RegBank : RegisterBank<"i32Bank", [I32]>;
def I64RegBank : RegisterBank<"i64Bank", [I64]>;
def F32RegBank : RegisterBank<"f32Bank", [F32]>;
def F64RegBank : RegisterBank<"f64Bank", [F64]>;
def PTRRegBank : RegisterBank<"ptrBank", [PTR]>;