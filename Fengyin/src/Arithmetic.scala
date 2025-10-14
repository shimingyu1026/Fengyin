package Fengyin

import chisel3._
import chisel3.util._

// 数据类型的定义
object BFloat16 {
  val EXP_WIDTH   = 8
  val SIG_WIDTH   = 7
  val TOTAL_WIDTH = 1 + EXP_WIDTH + SIG_WIDTH

  def apply(): BFloat16 = new BFloat16()
}

// BFloat16 的配置
case class BFloat16Config(
  isPipelined: Boolean // TRUE: 使用 DecoupledIO 和流水线; FALSE: 使用纯组合逻辑
)

class BFloat16 extends Bundle {
  val bits = UInt(BFloat16.TOTAL_WIDTH.W)
  // 方便访问的常量
  val expWidth: Int = BFloat16.EXP_WIDTH
  val sigWidth: Int = BFloat16.SIG_WIDTH
  val bias:     Int = (1 << (BFloat16.EXP_WIDTH - 1)) - 1 // 指数偏移量 127

  /** 从原始位中提取符号位 */
  def sign: UInt = bits(BFloat16.TOTAL_WIDTH - 1)

  /** 从原始位中提取指数部分 */
  def exponent: UInt = bits(BFloat16.TOTAL_WIDTH - 2, BFloat16.SIG_WIDTH)

  /** 从原始位中提取尾数部分 */
  def significand: UInt = bits(BFloat16.SIG_WIDTH - 1, 0)
}



class Int16 extends Bundle {
  val bits = SInt(16.W)
}

class Int32 extends Bundle {
  val bits = SInt(32.W)
}




abstract class Arithmetic[T <: Data] {
  implicit def cast(t: T): ArithmeticOps[T]
}

abstract class ArithmeticOps[T <: Data](self: T) {
  def *(t:    T): T
  // def mac(m1: T, m2: T): T // Returns (m1 * m2 + self)
  def +(t: T): T
  def -(t:    T): T
  // def >>(u:   UInt): T // This is a rounding shift! Rounds away from 0
  def >(t:    T): Bool
  // def identity: T
  // def withWidthOf(t:      T): T
  // def clippedToWidthOf(t: T): T // Like "withWidthOf", except that it saturates
  // def relu:    T
  // def zero:    T
  // def minimum: T

  // // Optional parameters, which only need to be defined if you want to enable various optimizations for transformers
  // def divider(denom_t: UInt, options: Int = 0): Option[(DecoupledIO[UInt], DecoupledIO[T])] = None
  // def sqrt: Option[(DecoupledIO[UInt], DecoupledIO[T])] = None
  // def reciprocal[U <: Data](u:                    U, options: Int = 0): Option[(DecoupledIO[UInt], DecoupledIO[U])] = None
  // def mult_with_reciprocal[U <: Data](reciprocal: U) = self
}

// class BFloat16Arithmetic(config: BFloat16Config) extends Arithmetic[BFloat16] {

//   override implicit def cast(self: BFloat16): ArithmeticOps[BFloat16] = new BFloat16ArithmeticOps(self)(config)
// }

// class BFloat16ArithmeticOps(
//   self:            BFloat16
// )(
//   implicit config: BFloat16Config)
//     extends ArithmeticOps[BFloat16](self) {

//   // ----------------------------------------------------------------------
//   // 重载加法运算符
//   // ----------------------------------------------------------------------
//   override def +(t: BFloat16): BFloat16 = {

//     val result = Wire(new BFloat16())

//     if (config.isPipelined) {
//       result := 0.U

//     } else {
//       result := 0.U
//     }

//     result
//   }
// }

object Arithmetic {

  implicit object Int16Arithmetic extends Arithmetic[Int16] {
    override implicit def cast(self: Int16): ArithmeticOps[Int16] = new ArithmeticOps(self) {
      override def +(t:    Int16) = self + t
      override def -(t:    Int16) = self - t
      override def *(t:    Int16) = self * t
      override def >(t:    Int16) = self > t

    }
  }
}
