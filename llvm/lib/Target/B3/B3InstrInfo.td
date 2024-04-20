

class B3Inst<dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction
{
  let Namespace = "B3";

  let OutOperandList = outs;
  let InOperandList  = ins;

  let AsmString   = asmstr;
  let Pattern     = pattern;
}

// TODO: Add proper selection patterns and remove the SPIRV esque manual instruction selector

def bb_op : Operand<OtherVT>;

class B3UnaryInst <string ty, RegisterClass RC, string asmstr, list<dag> pattern>
    : B3Inst<(outs RC:$dst), (ins RC:$src), ty#" $dst = "#asmstr#"($src)", pattern>;

class B3BinInst <string ty, RegisterClass RC, string asmstr, list<dag> pattern>
    : B3Inst<(outs RC:$dst), (ins RC:$a, RC:$b), ty#" $dst = "#asmstr#"($a, $b)", pattern>;

class B3Ext<string ty, string name, RegisterClass OUT, RegisterClass IN>
    : B3Inst<(outs OUT:$dst), (ins IN:$src), ty#" $dst = "#name#"($src)", []>;

class B3Load<string ty, string name, RegisterClass RC>
    : B3Inst<(outs RC:$dst), (ins PTR:$base, i32imm:$offset), ty#" $dst = "#name#"($base, $offset)", []>;

class B3Store<string name, RegisterClass RC>
    : B3Inst<(outs), (ins RC:$val, PTR:$base, i32imm:$offset), "Void "#name#"($val, $base, $offset)", []>;

class B3Cmp<string name, RegisterClass RC>
    : B3Inst<(outs I32:$dst), (ins RC:$lhs, RC:$rhs), "Int32 $dst = "#name#"($lhs, $rhs)", []>;

multiclass B3UnaryFloat <string asmstr, SDNode op> {
  def F32 : B3UnaryInst<"Float", F32, asmstr, [(set F32:$dst, (op f32:$src))]>;
  def F64 : B3UnaryInst<"Double", F64, asmstr, [(set F64:$dst, (op f64:$src))]>;
}

multiclass B3BinInt <string asmstr, SDNode op> {
  def I32 : B3BinInst<"Int32", I32, asmstr, [(set I32:$dst, (op i32:$a, i32:$b))]>;
  def I64 : B3BinInst<"Int64", I64, asmstr, [(set I64:$dst, (op i64:$a, i64:$b))]>;
}

multiclass B3Bin <string asmstr, SDNode iop, SDNode fop> {
  def I32 : B3BinInst<"Int32", I32, asmstr, [(set I32:$dst, (iop i32:$a, i32:$b))]>;
  def I64 : B3BinInst<"Int64", I64, asmstr, [(set I64:$dst, (iop i64:$a, i64:$b))]>;
  def F32 : B3BinInst<"Float", F32, asmstr, [(set F32:$dst, (fop f32:$a, f32:$b))]>;
  def F64 : B3BinInst<"Double", F64, asmstr, [(set F64:$dst, (fop f64:$a, f64:$b))]>;
}

defm Add : B3Bin<"Add", add, fadd>;
defm Sub : B3Bin<"Sub", sub, fsub>;
defm Mul : B3Bin<"Mul", mul, fmul>;
defm Div : B3Bin<"Div", sdiv, fdiv>;

defm And : B3BinInt<"BitAnd", and>;
defm Or  : B3BinInt<"BitOr", or>;
defm Xor : B3BinInt<"BitXor", xor>;

defm Abs : B3UnaryFloat<"Abs", fabs>;
defm Ceil : B3UnaryFloat<"Ceil", fceil>;
defm Floor : B3UnaryFloat<"Floor", ffloor>;
defm Sqrt : B3UnaryFloat<"Sqrt", fsqrt>;

def SExt8  : B3Ext<"Int32", "SExt8",  I32, I32>;
def SExt16 : B3Ext<"Int32", "SExt16", I32, I32>;
def SExt32 : B3Ext<"Int64", "SExt32", I64, I32>;
def ZExt32 : B3Ext<"Int64", "ZExt32", I64, I32>;

def LoadZ8  : B3Load<"Int32", "LoadZ8", I32>;
def LoadZ16 : B3Load<"Int32", "LoadZ16", I32>;
def LoadI32 : B3Load<"Int32", "Load", I32>;
def LoadI64 : B3Load<"Int32", "Load", I64>;
def LoadF32 : B3Load<"Float", "Load", F32>;
def LoadF64 : B3Load<"Double", "Load", F64>;

def Cast64 : B3Inst<(outs F64:$dst), (ins I64:$src), "Double $dst = Cast($src)", []>;

def Store8   : B3Store<"StoreZ8", I32>;
def Store16  : B3Store<"StoreZ16", I32>;
def StoreI32 : B3Store<"Store", I32>;
def StoreI64 : B3Store<"Store", I64>;
def StoreF32 : B3Store<"Store", F32>;
def StoreF64 : B3Store<"Store", F64>;

def GtI32 : B3Inst<(outs I32:$dst), (ins I32:$a, I32:$b), "Int32 $dst = GreaterThan($a, $b)", []>;
def EqI32 : B3Cmp<"Equal", I32>;
def EqF32 : B3Cmp<"Equal", F32>;
def EqI64 : B3Cmp<"Equal", I64>;
def EqF64 : B3Cmp<"Equal", F64>;

// This is a pseudo instruction that will be lowered into a nop on 64 bit platforms and into a ZExt32 on 32 bit platforms
def CastPTR : B3Inst<(outs I64:$dst), (ins PTR:$src), "Int64 $dst = CastPtr($src)", []>;
def PtrI64 : B3Inst<(outs PTR:$dst), (ins I64:$src), "IntPtr $dst = Cast($src)", []>;

def : Pat<(setcc I32:$a, I32:$b, SETEQ), (EqI32 I32:$a, I32:$b)>;

def UndefI32 : B3Inst<(outs I32:$dst), (ins), "Int32 $dst = Undef()", []>;
def UndefI64 : B3Inst<(outs I64:$dst), (ins), "Int64 $dst = Undef()", []>;
def UndefF32 : B3Inst<(outs F32:$dst), (ins), "Float $dst = Undef()", []>;
def UndefF64 : B3Inst<(outs F64:$dst), (ins), "Double $dst = Undef()", []>;
def UndefPTR : B3Inst<(outs PTR:$dst), (ins), "IntPtr $dst = Undef()", []>;

def AddPTR : B3Inst<(outs PTR:$dst), (ins PTR:$a, PTR:$b), "IntPtr $dst = Add($a, $b)", [(set PTR:$dst, (ptradd PTR:$a, PTR:$b))]>;

// No ConstPTR because we dont know if its i32 or i64
def ConstI32 : B3Inst<(outs I32:$dst), (ins i32imm:$imm), "Int32 $dst = Const32($imm)", []>;
def ConstI64 : B3Inst<(outs I64:$dst), (ins i64imm:$imm), "Int64 $dst = Const64($imm)", []>;
def ConstF32 : B3Inst<(outs F32:$dst), (ins f32imm:$imm), "Float $dst = ConstFloat($imm)", []>;
def ConstF64 : B3Inst<(outs F64:$dst), (ins f64imm:$imm), "Double $dst = ConstDouble($imm)", []>;

def RetVoid : B3Inst<(outs), (ins), "Void Return()", []>;
def RetI32 : B3Inst<(outs), (ins I32:$r), "Void Return($r)", []>;
def RetI64 : B3Inst<(outs), (ins I64:$r), "Void Return($r)", []>;
def RetF32 : B3Inst<(outs), (ins F32:$r), "Void Return($r)", []>;
def RetF64 : B3Inst<(outs), (ins F64:$r), "Void Return($r)", []>;
def RetPTR : B3Inst<(outs), (ins PTR:$r), "Void Return($r)", []>;

def Jmp : B3Inst<(outs), (ins bb_op:$block), "Void Jump($block)", [(br bb:$block)]>;

def BrI32 : B3Inst<(outs), (ins I32:$cmp, bb_op:$true, bb_op:$false), "Void Branch($cmp, $true, $false)", []>;

def ArgI32 : B3Inst<(outs I32:$d), (ins i32imm:$pos), "Int32 $d = Arg($pos)", []>;
def ArgI64 : B3Inst<(outs I64:$d), (ins i32imm:$pos), "Int64 $d = Arg($pos)", []>;
def ArgF32 : B3Inst<(outs F32:$d), (ins i32imm:$pos), "Float $d = Arg($pos)", []>;
def ArgF64 : B3Inst<(outs F64:$d), (ins i32imm:$pos), "Double $d = Arg($pos)", []>;
def ArgPTR : B3Inst<(outs PTR:$d), (ins i32imm:$pos), "IntPtr $d = Arg($pos)", []>;

def PhiI32 : B3Inst<(outs I32:$d), (ins), "Int32 $d = Phi()", []>;
def PhiI64 : B3Inst<(outs I64:$d), (ins), "Int64 $d = Phi()", []>;
def PhiF32 : B3Inst<(outs F32:$d), (ins), "Float $d = Phi()", []>;
def PhiF64 : B3Inst<(outs F64:$d), (ins), "Double $d = Phi()", []>;
def PhiPTR : B3Inst<(outs PTR:$d), (ins), "IntPtr $d = Phi()", []>;
def UpsilonI32 : B3Inst<(outs), (ins I32:$v, I32:$phi), "Void Upsilon($v, $phi)", []>;
def UpsilonI64 : B3Inst<(outs), (ins I64:$v, I64:$phi), "Void Upsilon($v, $phi)", []>;
def UpsilonF32 : B3Inst<(outs), (ins F32:$v, F32:$phi), "Void Upsilon($v, $phi)", []>;
def UpsilonF64 : B3Inst<(outs), (ins F64:$v, F64:$phi), "Void Upsilon($v, $phi)", []>;
def UpsilonPTR : B3Inst<(outs), (ins PTR:$v, PTR:$phi), "Void Upsilon($v, $phi)", []>;

// TODO: Add this to B3
def TCCall : B3Inst<(outs), (ins), "Void TCCall()", []>;

def CCallI32 : B3Inst<(outs I32:$d), (ins), "Int32 $d = CCall()", []>;
def CCallI64 : B3Inst<(outs I64:$d), (ins), "Int64 $d = CCall()", []>;
def CCallF32 : B3Inst<(outs F32:$d), (ins), "Float $d = CCall()", []>;
def CCallF64 : B3Inst<(outs F64:$d), (ins), "Double $d = CCall()", []>;
def CCallPTR : B3Inst<(outs PTR:$d), (ins), "IntPtr $d = CCall()", []>;
def CCallVoid : B3Inst<(outs), (ins), "Void CCall()", []>;
