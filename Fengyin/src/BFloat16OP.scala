package Fengyin

import chisel3._
import chisel3.util._

class BFloat16ADDComb extends Module {
  val io  = IO(new Bundle {
    val in1 = Input(BFloat16())
    val in2 = Input(BFloat16())
    val out = Output(BFloat16())
  })
  // 两个输入标识符
  val in1 = 1.U
  val in2 = 0.U
  // 指数和尾数宽度
  val E   = io.in1.expWidth
  val M   = io.in1.sigWidth
  val B   = io.in1.bias

  val M_u    = M + 4 // 尾数部分扩展4位
  // =========================================================================
  // 阶段 I: 分解和特殊情况
  // =========================================================================
  val sign_a = io.in1.sign
  val exp_a  = io.in1.exponent
  val man_a  = io.in1.significand

  val sign_b = io.in2.sign
  val exp_b  = io.in2.exponent
  val man_b  = io.in2.significand

  // 1.1 特殊值和零值检测
  val is_zero_a = (exp_a === 0.U) && (man_a === 0.U)
  val is_zero_b = (exp_b === 0.U) && (man_b === 0.U)

  // 1.2 隐含位和扩展尾数 (加入隐含的 '1'，并进行零扩展)
  // 规范化数：尾数 = Cat(1.U, man_a).pad(M_PATH_WIDTH)
  val extended_man_a = Cat(1.U(1.W), man_a, 0.U((M_u - M - 1).W))
  val extended_man_b = Cat(1.U(1.W), man_b, 0.U((M_u - M - 1).W))

  // 1.3     确定较大的数
  val exp_diff = Mux(exp_a > exp_b, in1, in2)

  val exp_big   = Mux(exp_diff === in1, exp_a, exp_b)
  val exp_small = Mux(exp_diff === in1, exp_b, exp_a)

  val man_big   = Mux(exp_diff === in1, extended_man_a, extended_man_b)
  val man_small = Mux(exp_diff === in1, extended_man_b, extended_man_a)

  val sign_big   = Mux(exp_diff === in1, sign_a, sign_b)
  val sign_small = Mux(exp_diff === in1, sign_b, sign_a)

  // =========================================================================
  // 阶段 II: 对齐 (Alignment)
  // =========================================================================

  val shift_amount      = exp_big - exp_small // 总是正数
  val shifted_man_small = man_small >> shift_amount
  // =========================================================================
  // 阶段 III: 相加/相减 (Add/Subtract Significands)
  // =========================================================================
  val do_add            = sign_big === sign_small
//TODO
}
