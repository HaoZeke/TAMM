//------------------------------------------------------------------------------
// Copyright (C) 2016, Pacific Northwest National Laboratory
// This software is subject to copyright protection under the laws of the
// United States and other countries
//
// All rights in this computer software are reserved by the
// Pacific Northwest National Laboratory (PNNL)
// Operated by Battelle for the U.S. Department of Energy
//
//------------------------------------------------------------------------------
#include <iostream>
#include "tensor/corf.h"
#include "tensor/equations.h"
#include "tensor/input.h"
#include "tensor/schedulers.h"
#include "tensor/t_assign.h"
#include "tensor/t_mult.h"
#include "tensor/tensor.h"
#include "tensor/tensors_and_ops.h"
#include "tensor/variables.h"

/*
 *i0 ( p3 p4 h1 h2 )_v + = 1 * v ( p3 p4 h1 h2 )_v
 *i0 ( p3 p4 h1 h2 )_vt + = -1 * P( 2 ) * Sum ( h10 ) * t ( p3 h10 )_t * i1 (
 *h10 p4 h1 h2 )_v
 *    i1 ( h10 p3 h1 h2 )_v + = 1 * v ( h10 p3 h1 h2 )_v
 *    i1 ( h10 p3 h1 h2 )_vt + = 1/2 * Sum ( h11 ) * t ( p3 h11 )_t * i2 ( h10
 *h11 h1 h2 )_v
 *        i2 ( h10 h11 h1 h2 )_v + = -1 * v ( h10 h11 h1 h2 )_v
 *        i2 ( h10 h11 h1 h2 )_vt + = 1 * P( 2 ) * Sum ( p5 ) * t ( p5 h1 )_t *
 *i3 ( h10 h11 h2 p5 )_v
 *            i3 ( h10 h11 h1 p5 )_v + = 1 * v ( h10 h11 h1 p5 )_v
 *            i3 ( h10 h11 h1 p5 )_vt + = -1/2 * Sum ( p6 ) * t ( p6 h1 )_t * v
 *( h10 h11 p5 p6 )_v
 *        i2 ( h10 h11 h1 h2 )_vt + = -1/2 * Sum ( p7 p8 ) * t ( p7 p8 h1 h2 )_t
 ** v ( h10 h11 p7 p8 )_v
 *    i1 ( h10 p3 h1 h2 )_vt + = -1 * P( 2 ) * Sum ( p5 ) * t ( p5 h1 )_t * i2 (
 *h10 p3 h2 p5 )_v
 *        i2 ( h10 p3 h1 p5 )_v + = 1 * v ( h10 p3 h1 p5 )_v
 *        i2 ( h10 p3 h1 p5 )_vt + = -1/2 * Sum ( p6 ) * t ( p6 h1 )_t * v ( h10
 *p3 p5 p6 )_v
 *    i1 ( h10 p3 h1 h2 )_ft + = -1 * Sum ( p5 ) * t ( p3 p5 h1 h2 )_t * i2 (
 *h10 p5 )_f
 *        i2 ( h10 p5 )_f + = 1 * f ( h10 p5 )_f
 *        i2 ( h10 p5 )_vt + = -1 * Sum ( h7 p6 ) * t ( p6 h7 )_t * v ( h7 h10
 *p5 p6 )_v
 *    i1 ( h10 p3 h1 h2 )_vt + = 1 * P( 2 ) * Sum ( h7 p9 ) * t ( p3 p9 h1 h7
 *)_t * i2 ( h7 h10 h2 p9 )_v
 *        i2 ( h7 h10 h1 p9 )_v + = 1 * v ( h7 h10 h1 p9 )_v
 *        i2 ( h7 h10 h1 p9 )_vt + = 1 * Sum ( p5 ) * t ( p5 h1 )_t * v ( h7 h10
 *p5 p9 )_v
 *    i1 ( h10 p3 h1 h2 )_vt + = 1/2 * Sum ( p5 p6 ) * t ( p5 p6 h1 h2 )_t * v (
 *h10 p3 p5 p6 )_v
 *i0 ( p3 p4 h1 h2 )_vt + = -1 * P( 2 ) * Sum ( p5 ) * t ( p5 h1 )_t * i1 ( p3
 *p4 h2 p5 )_v
 *    i1 ( p3 p4 h1 p5 )_v + = 1 * v ( p3 p4 h1 p5 )_v
 *    i1 ( p3 p4 h1 p5 )_vt + = -1/2 * Sum ( p6 ) * t ( p6 h1 )_t * v ( p3 p4 p5
 *p6 )_v
 *i0 ( p3 p4 h1 h2 )_tf + = -1 * P( 2 ) * Sum ( h9 ) * t ( p3 p4 h1 h9 )_t * i1
 *( h9 h2 )_f
 *    i1 ( h9 h1 )_f + = 1 * f ( h9 h1 )_f
 *    i1 ( h9 h1 )_ft + = 1 * Sum ( p8 ) * t ( p8 h1 )_t * i2 ( h9 p8 )_f
 *        i2 ( h9 p8 )_f + = 1 * f ( h9 p8 )_f
 *        i2 ( h9 p8 )_vt + = 1 * Sum ( h7 p6 ) * t ( p6 h7 )_t * v ( h7 h9 p6
 *p8 )_v
 *    i1 ( h9 h1 )_vt + = -1 * Sum ( h7 p6 ) * t ( p6 h7 )_t * v ( h7 h9 h1 p6
 *)_v
 *    i1 ( h9 h1 )_vt + = -1/2 * Sum ( h8 p6 p7 ) * t ( p6 p7 h1 h8 )_t * v ( h8
 *h9 p6 p7 )_v
 *i0 ( p3 p4 h1 h2 )_tf + = 1 * P( 2 ) * Sum ( p5 ) * t ( p3 p5 h1 h2 )_t * i1 (
 *p4 p5 )_f
 *    i1 ( p3 p5 )_f + = 1 * f ( p3 p5 )_f
 *    i1 ( p3 p5 )_vt + = -1 * Sum ( h7 p6 ) * t ( p6 h7 )_t * v ( h7 p3 p5 p6
 *)_v
 *    i1 ( p3 p5 )_vt + = -1/2 * Sum ( h7 h8 p6 ) * t ( p3 p6 h7 h8 )_t * v ( h7
 *h8 p5 p6 )_v
 *i0 ( p3 p4 h1 h2 )_vt + = -1/2 * Sum ( h11 h9 ) * t ( p3 p4 h9 h11 )_t * i1 (
 *h9 h11 h1 h2 )_v
 *    i1 ( h9 h11 h1 h2 )_v + = -1 * v ( h9 h11 h1 h2 )_v
 *    i1 ( h9 h11 h1 h2 )_vt + = 1 * P( 2 ) * Sum ( p8 ) * t ( p8 h1 )_t * i2 (
 *h9 h11 h2 p8 )_v
 *        i2 ( h9 h11 h1 p8 )_v + = 1 * v ( h9 h11 h1 p8 )_v
 *        i2 ( h9 h11 h1 p8 )_vt + = 1/2 * Sum ( p6 ) * t ( p6 h1 )_t * v ( h9
 *h11 p6 p8 )_v
 *    i1 ( h9 h11 h1 h2 )_vt + = -1/2 * Sum ( p5 p6 ) * t ( p5 p6 h1 h2 )_t * v
 *( h9 h11 p5 p6 )_v
 *i0 ( p3 p4 h1 h2 )_vt + = -1 * P( 4 ) * Sum ( h6 p5 ) * t ( p3 p5 h1 h6 )_t *
 *i1 ( h6 p4 h2 p5 )_v
 *    i1 ( h6 p3 h1 p5 )_v + = 1 * v ( h6 p3 h1 p5 )_v
 *    i1 ( h6 p3 h1 p5 )_vt + = -1 * Sum ( p7 ) * t ( p7 h1 )_t * v ( h6 p3 p5
 *p7 )_v
 *    i1 ( h6 p3 h1 p5 )_vt + = -1/2 * Sum ( h8 p7 ) * t ( p3 p7 h1 h8 )_t * v (
 *h6 h8 p5 p7 )_v
 *i0 ( p3 p4 h1 h2 )_vt + = 1/2 * Sum ( p5 p6 ) * t ( p5 p6 h1 h2 )_t * v ( p3
 *p4 p5 p6 )_v
 */

/*
 * t2_1:  i0 ( p3 p4 h1 h2 ) += 1 * v ( p3 p4 h1 h2 )
 * t2_2_1_createfile:     i1 ( h10 p3 h1 h2 )
 * t2_2_1:      t2_2_1 ( h10 p3 h1 h2 ) += 1 * v ( h10 p3 h1 h2 )
 * t2_2_2_1_createfile:     i2 ( h10 h11 h1 h2 )
 * t2_2_2_1:     t2_2_2_1 ( h10 h11 h1 h2 ) += -1 * v ( h10 h11 h1 h2 )
 * t2_2_2_2_1_createfile:     i3 ( h10 h11 h1 p5 )
 * t2_2_2_2_1:     t2_2_2_2_1 ( h10 h11 h1 p5 ) += 1 * v ( h10 h11 h1 p5 )
 * t2_2_2_2_2:     t2_2_2_2_1 ( h10 h11 h1 p5 ) += -0.5 * t ( p6 h1 ) * v ( h10
h11 p5 p6 )
 * t2_2_2_2:     t2_2_2_1 ( h10 h11 h1 h2 ) += 1 * t ( p5 h1 ) * t2_2_2_2_1 (
h10 h11 h2 p5 )
 * t2_2_2_2_1_deletefile
 * t2_2_2_3:     t2_2_2_1 ( h10 h11 h1 h2 ) += -0.5 * t ( p7 p8 h1 h2 ) * v (
h10 h11 p7 p8 )
 * t2_2_2:     t2_2_1 ( h10 p3 h1 h2 ) += 0.5 * t ( p3 h11 ) * t2_2_2_1 ( h10
h11 h1 h2 )
 * t2_2_2_1_deletefile
 * t2_2_4_1_createfile:     i2 ( h10 p5 )
 * t2_2_4_1:     t2_2_4_1 ( h10 p5 ) += 1 * f ( h10 p5 )
 * t2_2_4_2:     t2_2_4_1 ( h10 p5 ) += -1 * t ( p6 h7 ) * v ( h7 h10 p5 p6 )
 * t2_2_4:     t2_2_1 ( h10 p3 h1 h2 ) += -1 * t ( p3 p5 h1 h2 ) * t2_2_4_1 (
h10 p5 )
 * t2_2_4_1_deletefile
 * t2_2_5_1_createfile:     i2 ( h7 h10 h1 p9 )
 * t2_2_5_1:     t2_2_5_1 ( h7 h10 h1 p9 ) += 1 * v ( h7 h10 h1 p9 )
 * t2_2_5_2:     t2_2_5_1 ( h7 h10 h1 p9 ) += 1 * t ( p5 h1 ) * v ( h7 h10 p5 p9
)
 * t2_2_5:     t2_2_1 ( h10 p3 h1 h2 ) += 1 * t ( p3 p9 h1 h7 ) * t2_2_5_1 ( h7
h10 h2 p9 )
 * t2_2_5_1_deletefile
 * c2f_t2_t12: t2 ( p1 p2 h3 h4) += 0.5 * t ( p1 h3 ) * t (p2 h4)
 * t2_2_6:     t2_2_1 ( h10 p3 h1 h2 ) += 0.5 * t ( p5 p6 h1 h2 ) * v ( h10 p3
p5 p6 )
 * c2d_t2_t12: t2 ( p1 p2 h3 h4) += -0.5 * t ( p1 h3 ) * t (p2 h4)
 * t2_2:     i0 ( p3 p4 h1 h2 ) += -1 * t ( p3 h10 ) * t2_2_1 ( h10 p4 h1 h2 )
 * t2_2_1_deletefile
 * lt2_3x:     i0 ( p3 p4 h1 h2 ) += -1 * t ( p5 h1 ) * v ( p3 p4 h2 p5 )
 * OFFSET_ccsd_t2_4_1:     i1 ( h9 h1 )
 * t2_4_1:     t2_4_1 ( h9 h1 ) += 1 * f ( h9 h1 )
 * OFFSET_ccsd_t2_4_2_1: i2 ( h9 p8 )
 * t2_4_2_1:     t2_4_2_1 ( h9 p8 ) += 1 * f ( h9 p8 )
 * t2_4_2_2:     t2_4_2_1 ( h9 p8 ) += 1 * t ( p6 h7 ) * v ( h7 h9 p6 p8 )
 * t2_4_2:     t2_4_1 ( h9 h1 ) += 1 * t ( p8 h1 ) * t2_4_2_1 ( h9 p8 )
 * t2_4_3:     t2_4_1 ( h9 h1 ) += -1 * t ( p6 h7 ) * v ( h7 h9 h1 p6 )
 * t2_4_4:     t2_4_1 ( h9 h1 ) += -0.5 * t ( p6 p7 h1 h8 ) * v ( h8 h9 p6 p7 )
 * create i1_local
c    copy d_t1 ==> l_t1_local
 * t2_4:     i0 ( p3 p4 h1 h2 ) += -1 * t ( p3 p4 h1 h9 ) * t2_4_1 ( h9 h2 )
 * delete i1_local
 * DELETEFILE t2_4_1
 * t2_5_1 create:     i1 ( p3 p5 )
 * t2_5_1:     t2_5_1 ( p3 p5 ) += 1 * f ( p3 p5 )
 * t2_5_2:     t2_5_1 ( p3 p5 ) += -1 * t ( p6 h7 ) * v ( h7 p3 p5 p6 )
 * t2_5_3:     t2_5_1 ( p3 p5 ) += -0.5 * t ( p3 p6 h7 h8 ) * v ( h7 h8 p5 p6 )
 * create i1_local
c    copy d_t1 ==> l_t1_local
 * t2_5:     i0 ( p3 p4 h1 h2 ) += 1 * t ( p3 p5 h1 h2 ) * t2_5_1 ( p4 p5 )
 * delete i1_local
 * DELETEFILE t2_5_1
 * OFFSET_t2_6_1:     i1 ( h9 h11 h1 h2 )
 * t2_6_1:     t2_6_1 ( h9 h11 h1 h2 ) += -1 * v ( h9 h11 h1 h2 )
 * OFFSET_t2_6_2_1:     i2 ( h9 h11 h1 p8 )
 * t2_6_2_1:     t2_6_2_1 ( h9 h11 h1 p8 ) += 1 * v ( h9 h11 h1 p8 )
 * t2_6_2_2:     t2_6_2_1 ( h9 h11 h1 p8 ) += 0.5 * t ( p6 h1 ) * v ( h9 h11 p6
p8 )
 * t2_6_2:     t2_6_1 ( h9 h11 h1 h2 ) += 1 * t ( p8 h1 ) * t2_6_2_1 ( h9 h11 h2
p8 )
 * DELETEFILE t2_6_2_1
 * t2_6_3:     t2_6_1 ( h9 h11 h1 h2 ) += -0.5 * t ( p5 p6 h1 h2 ) * v ( h9 h11
p5 p6 )
 * t2_6:     i0 ( p3 p4 h1 h2 ) += -0.5 * t ( p3 p4 h9 h11 ) * t2_6_1 ( h9 h11
h1 h2 )
 * DELETEFILE t2_6_1
 * OFFSET_t2_7_1:     i1 ( h6 p3 h1 p5 )
 * t2_7_1:     t2_7_1 ( h6 p3 h1 p5 ) += 1 * v ( h6 p3 h1 p5 )
 * t2_7_2:     t2_7_1 ( h6 p3 h1 p5 ) += -1 * t ( p7 h1 ) * v ( h6 p3 p5 p7 )
 * t2_7_3:     t2_7_1 ( h6 p3 h1 p5 ) += -0.5 * t ( p3 p7 h1 h8 ) * v ( h6 h8 p5
p7 )
 * t2_7:     i0 ( p3 p4 h1 h2 ) += -1 * t ( p3 p5 h1 h6 ) * t2_7_1 ( h6 p4 h2 p5
)
 * DELETEFILE t2_7_1
 * vt1t1_1_createfile: C     i1 ( h5 p3 h1 h2 )
 * vt1t1_1_2:     vt1t1_1 ( h5 p3 h1 h2 ) += -2 * t ( p6 h1 ) * v ( h5 p3 h2 p6
)
 * vt1t1_1:     i0 ( p3 p4 h1 h2 )t += -0.5 * t ( p3 h5 ) * vt1t1_1 ( h5 p4 h1
h2 )
 * vt1t1_1_deletefile
 * c2f_t2_t12: t2 ( p1 p2 h3 h4) += 0.5 * t ( p1 h3 ) * t (p2 h4)
 * t2_8:   i0 ( p3 p4 h1 h2 )_vt + = 0.5 * t ( p5 p6 h1 h2 ) * v ( p3 p4 p5 p6 )
 * c2d_t2_t12: t2 ( p1 p2 h3 h4) += -0.5 * t ( p1 h3 ) * t (p2 h4)
 */

extern "C" {
  void icsd_t2_1_(Integer *d_v, Integer *k_v_offset,Integer *d_i0, Integer *k_i0_offset);
  void icsd_t2_2_1_(Integer *d_v, Integer *k_v_offset,Integer *d_t2_2_1, Integer *k_t2_2_1_offset);
  void icsd_t2_2_2_1_(Integer *d_v, Integer *k_v_offset,Integer *d_t2_2_2_1, Integer *k_t2_2_2_1_offset);
  void icsd_t2_2_2_2_1_(Integer *d_v, Integer *k_v_offset,Integer *d_t2_2_2_2_1, Integer *k_t2_2_2_2_1_offset);
  void icsd_t2_2_2_2_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_2_2_2_1, Integer *k_t2_2_2_2_1_offset);
  void icsd_t2_2_2_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_t2_2_2_2_1, Integer *k_t2_2_2_2_1_offset,Integer *d_t2_2_2_1, Integer *k_t2_2_2_1_offset);
  void icsd_t2_2_2_3_(Integer *d_t2, Integer *k_t2_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_2_2_1, Integer *k_t2_2_2_1_offset);
  void icsd_t2_2_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_t2_2_2_1, Integer *k_t2_2_2_1_offset,Integer *d_t2_2_1, Integer *k_t2_2_1_offset);
  void icsd_t2_2_4_1_(Integer *d_f, Integer *k_f_offset,Integer *d_t2_2_4_1, Integer *k_t2_2_4_1_offset);
  void icsd_t2_2_4_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_2_4_1, Integer *k_t2_2_4_1_offset);
  void icsd_t2_2_4_(Integer *d_t2, Integer *k_t2_offset,Integer *d_t2_2_4_1, Integer *k_t2_2_4_1_offset,Integer *d_t2_2_1, Integer *k_t2_2_1_offset);
  void icsd_t2_2_5_1_(Integer *d_v, Integer *k_v_offset,Integer *d_t2_2_5_1, Integer *k_t2_2_5_1_offset);
  void icsd_t2_2_5_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_2_5_1, Integer *k_t2_2_5_1_offset);
  void icsd_t2_2_5_(Integer *d_t2, Integer *k_t2_offset,Integer *d_t2_2_5_1, Integer *k_t2_2_5_1_offset,Integer *d_t2_2_1, Integer *k_t2_2_1_offset);
  void icsd_t2_2_6_(Integer *d_c2, Integer *k_c2_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_2_1, Integer *k_t2_2_1_offset);
  void icsd_t2_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_t2_2_1, Integer *k_t2_2_1_offset,Integer *d_i0, Integer *k_i0_offset);
  void icsd_t2_lt2_3x_(Integer *d_t1, Integer *k_t1_offset,Integer *d_v, Integer *k_v_offset,Integer *d_i0, Integer *k_i0_offset);
  void icsd_t2_4_1_(Integer *d_f, Integer *k_f_offset,Integer *d_t2_4_1, Integer *k_t2_4_1_offset);
  void icsd_t2_4_2_1_(Integer *d_f, Integer *k_f_offset,Integer *d_t2_4_2_1, Integer *k_t2_4_2_1_offset);
  void icsd_t2_4_2_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_4_2_1, Integer *k_t2_4_2_1_offset);
  void icsd_t2_4_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_t2_4_2_1, Integer *k_t2_4_2_1_offset,Integer *d_t2_4_1, Integer *k_t2_4_1_offset);
  void icsd_t2_4_3_(Integer *d_t1, Integer *k_t1_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_4_1, Integer *k_t2_4_1_offset);
  void icsd_t2_4_4_(Integer *d_t2, Integer *k_t2_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_4_1, Integer *k_t2_4_1_offset);
  void icsd_t2_4_(Integer *d_t2, Integer *k_t2_offset,Integer *d_t2_4_1, Integer *k_t2_4_1_offset,Integer *d_i0, Integer *k_i0_offset);
  void icsd_t2_5_1_(Integer *d_f, Integer *k_f_offset,Integer *d_t2_5_1, Integer *k_t2_5_1_offset);
  void icsd_t2_5_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_5_1, Integer *k_t2_5_1_offset);
  void icsd_t2_5_3_(Integer *d_t2, Integer *k_t2_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_5_1, Integer *k_t2_5_1_offset);
  void icsd_t2_5_(Integer *d_t2, Integer *k_t2_offset,Integer *d_t2_5_1, Integer *k_t2_5_1_offset,Integer *d_i0, Integer *k_i0_offset);
  void icsd_t2_6_1_(Integer *d_v, Integer *k_v_offset,Integer *d_t2_6_1, Integer *k_t2_6_1_offset);
  void icsd_t2_6_2_1_(Integer *d_v, Integer *k_v_offset,Integer *d_t2_6_2_1, Integer *k_t2_6_2_1_offset);
  void icsd_t2_6_2_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_6_2_1, Integer *k_t2_6_2_1_offset);
  void icsd_t2_6_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_t2_6_2_1, Integer *k_t2_6_2_1_offset,Integer *d_t2_6_1, Integer *k_t2_6_1_offset);
  void icsd_t2_6_3_(Integer *d_t2, Integer *k_t2_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_6_1, Integer *k_t2_6_1_offset);
  void icsd_t2_6_(Integer *d_t2, Integer *k_t2_offset,Integer *d_t2_6_1, Integer *k_t2_6_1_offset,Integer *d_i0, Integer *k_i0_offset);
  void icsd_t2_7_1_(Integer *d_v, Integer *k_v_offset,Integer *d_t2_7_1, Integer *k_t2_7_1_offset);
  void icsd_t2_7_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_7_1, Integer *k_t2_7_1_offset);
  void icsd_t2_7_3_(Integer *d_t2, Integer *k_t2_offset,Integer *d_v, Integer *k_v_offset,Integer *d_t2_7_1, Integer *k_t2_7_1_offset);
  void icsd_t2_7_(Integer *d_t2, Integer *k_t2_offset,Integer *d_t2_7_1, Integer *k_t2_7_1_offset,Integer *d_i0, Integer *k_i0_offset);
  void icsd_t2_vt1t1_1_2_(Integer *d_t1, Integer *k_t1_offset,Integer *d_v, Integer *k_v_offset,Integer *d_vt1t1_1, Integer *k_vt1t1_1_offset);
  void icsd_t2_vt1t1_1_(Integer *d_t1, Integer *k_t1_offset,Integer *d_vt1t1_1, Integer *k_vt1t1_1_offset,Integer *d_i0, Integer *k_i0_offset);
  void icsd_t2_8_(Integer *d_c2, Integer *k_c2_offset,Integer *d_v, Integer *k_v_offset,Integer *d_i0, Integer *k_i0_offset);

  void offset_icsd_t2_2_1_(Integer *l_t2_2_1_offset, Integer *k_t2_2_1_offset, Integer *size_t2_2_1);
  void offset_icsd_t2_2_2_1_(Integer *l_t2_2_2_1_offset, Integer *k_t2_2_2_1_offset, Integer *size_t2_2_2_1);
  void offset_icsd_t2_2_2_2_1_(Integer *l_t2_2_2_2_1_offset, Integer *k_t2_2_2_2_1_offset, Integer *size_t2_2_2_2_1);
  void offset_icsd_t2_2_4_1_(Integer *l_t2_2_4_1_offset, Integer *k_t2_2_4_1_offset, Integer *size_t2_2_4_1);
  void offset_icsd_t2_2_5_1_(Integer *l_t2_2_5_1_offset, Integer *k_t2_2_5_1_offset, Integer *size_t2_2_5_1);
  void offset_icsd_t2_4_1_(Integer *l_t2_4_1_offset, Integer *k_t2_4_1_offset, Integer *size_t2_4_1);
  void offset_icsd_t2_4_2_1_(Integer *l_t2_4_2_1_offset, Integer *k_t2_4_2_1_offset, Integer *size_t2_4_2_1);
  void offset_icsd_t2_5_1_(Integer *l_t2_5_1_offset, Integer *k_t2_5_1_offset, Integer *size_t2_5_1);
  void offset_icsd_t2_6_1_(Integer *l_t2_6_1_offset, Integer *k_t2_6_1_offset, Integer *size_t2_6_1);
  void offset_icsd_t2_6_2_1_(Integer *l_t2_6_2_1_offset, Integer *k_t2_6_2_1_offset, Integer *size_t2_6_2_1);
  void offset_icsd_t2_7_1_(Integer *l_t2_7_1_offset, Integer *k_t2_7_1_offset, Integer *size_t2_7_1);
  void offset_icsd_t2_vt1t1_1_(Integer *l_vt1t1_1_offset,Integer *k_vt1t1_1_offset, Integer *size_vt1t1_1);

}

namespace tamm {

typedef void (*c2fd_fn)(Integer *, Integer *, Integer *, Integer *);

static void CorFortranc2fd(int use_c, Multiplication *m, c2fd_fn fn) {
  if (use_c) {
    m->execute();
  } else {
    Integer da = static_cast<int>(m->tA().ga()),
            da_offset = m->tA().offset_index();
    // Integer db = m.tB().ga(), db_offset = m.tB().offset_index();
    Integer dc = static_cast<int>(m->tC().ga()),
            dc_offset = m->tC().offset_index();
    fn(&da, &da_offset, &dc, &dc_offset);
  }
}

extern "C" {

void icsd_t2_cxx_(Integer *d_f1, Integer *d_i0, Integer *d_t1, Integer *d_t2,
                  Integer *d_v2, Integer *k_f1_offset, Integer *k_i0_offset,
                  Integer *k_t1_offset, Integer *k_t2_offset,
                  Integer *k_v2_offset, Integer *d_c2, Integer *iter_unused) {
  static bool set_t2 = true;
  Assignment op_t2_1;
  Assignment op_t2_2_1;
  Assignment op_t2_2_2_1;
  Assignment op_t2_2_2_2_1;
  Assignment op_t2_2_4_1;
  Assignment op_t2_2_5_1;
  Assignment op_t2_4_1;
  Assignment op_t2_4_2_1;
  Assignment op_t2_5_1;
  Assignment op_t2_6_1;
  Assignment op_t2_6_2_1;
  Assignment op_t2_7_1;
  Multiplication op_t2_2_2_2_2;
  Multiplication op_t2_2_2_2;
  Multiplication op_t2_2_2_3;
  Multiplication op_t2_2_2;
  Multiplication op_t2_2_4_2;
  Multiplication op_t2_2_4;
  Multiplication op_t2_2_5_2;
  Multiplication op_t2_2_5;
  Multiplication op_t2_2_6;
  Multiplication op_t2_2;
  Multiplication op_lt2_3x;
  Multiplication op_t2_4_2_2;
  Multiplication op_t2_4_2;
  Multiplication op_t2_4_3;
  Multiplication op_t2_4_4;
  Multiplication op_t2_4;
  Multiplication op_t2_5_2;
  Multiplication op_t2_5_3;
  Multiplication op_t2_5;
  Multiplication op_t2_6_2_2;
  Multiplication op_t2_6_2;
  Multiplication op_t2_6_3;
  Multiplication op_t2_6;
  Multiplication op_t2_7_2;
  Multiplication op_t2_7_3;
  Multiplication op_t2_7;
  Multiplication op_vt1t1_1_2;
  Multiplication op_vt1t1_1;
  Multiplication op_t2_8;
  
  DistType idist = (Variables::intorb()) ? dist_nwi : dist_nw;
  static Equations eqs;

  if (set_t2) {
    icsd_t2_equations(&eqs);
    set_t2 = false;
  }

  std::map<std::string, tamm::Tensor> tensors;
  std::vector <Operation> ops;
  tensors_and_ops(&eqs, &tensors, &ops);

  Tensor *i0 = &tensors["i0"];
  Tensor *f = &tensors["f"];
  Tensor *v = &tensors["v"];
  Tensor *t1 = &tensors["t1"];
  Tensor *t2 = &tensors["t2"];
  Tensor *t2_2_1 = &tensors["t2_2_1"];
  Tensor *t2_2_2_1 = &tensors["t2_2_2_1"];
  Tensor *t2_2_2_2_1 = &tensors["t2_2_2_2_1"];
  Tensor *t2_2_4_1 = &tensors["t2_2_4_1"];
  Tensor *t2_2_5_1 = &tensors["t2_2_5_1"];
  Tensor *t2_4_1 = &tensors["t2_4_1"];
  Tensor *t2_4_2_1 = &tensors["t2_4_2_1"];
  Tensor *t2_5_1 = &tensors["t2_5_1"];
  Tensor *t2_6_1 = &tensors["t2_6_1"];
  Tensor *t2_6_2_1 = &tensors["t2_6_2_1"];
  Tensor *t2_7_1 = &tensors["t2_7_1"];
  Tensor *vt1t1_1 = &tensors["vt1t1_1"];
  Tensor *c2 = &tensors["c2"];

  /* ----- Insert attach code ------ */
  v->set_dist(idist);
  t1->set_dist(dist_nwma);
  f->attach(*k_f1_offset, 0, *d_f1);
  i0->attach(*k_i0_offset, 0, *d_i0);
  t1->attach(*k_t1_offset, 0, *d_t1);
  t2->attach(*k_t2_offset, 0, *d_t2);
  v->attach(*k_v2_offset, 0, *d_v2);
  c2->attach(*k_t2_offset, 0, *d_c2);

#if 0
  schedule_linear(&tensors, &ops);
  // schedule_linear_lazy(tensors, ops);
  //schedule_levels(&tensors, &ops);
#else
    op_t2_1 = ops[0].add;
    op_t2_2_1 = ops[1].add;
    op_t2_2_2_1 = ops[2].add;
    op_t2_2_2_2_1 = ops[3].add;
    op_t2_2_2_2_2 = ops[4].mult;
    op_t2_2_2_2 = ops[5].mult;
    op_t2_2_2_3 = ops[6].mult;
    op_t2_2_2 = ops[7].mult;
    op_t2_2_4_1 = ops[8].add;
    op_t2_2_4_2 = ops[9].mult;
    op_t2_2_4 = ops[10].mult;
    op_t2_2_5_1 = ops[11].add;
    op_t2_2_5_2 = ops[12].mult;
    op_t2_2_5 = ops[13].mult;
    op_t2_2_6 = ops[14].mult;
    op_t2_2 = ops[15].mult;
    op_lt2_3x = ops[16].mult;
    op_t2_4_1 = ops[17].add;
    op_t2_4_2_1 = ops[18].add;
    op_t2_4_2_2 = ops[19].mult;
    op_t2_4_2 = ops[20].mult;
    op_t2_4_3 = ops[21].mult;
    op_t2_4_4 = ops[22].mult;
    op_t2_4 = ops[23].mult;
    op_t2_5_1 = ops[24].add;
    op_t2_5_2 = ops[25].mult;
    op_t2_5_3 = ops[26].mult;
    op_t2_5 = ops[27].mult;
    op_t2_6_1 = ops[28].add;
    op_t2_6_2_1 = ops[29].add;
    op_t2_6_2_2 = ops[30].mult;
    op_t2_6_2 = ops[31].mult;
    op_t2_6_3 = ops[32].mult;
    op_t2_6 = ops[33].mult;
    op_t2_7_1 = ops[34].add;
    op_t2_7_2 = ops[35].mult;
    op_t2_7_3 = ops[36].mult;
    op_t2_7 = ops[37].mult;
    op_vt1t1_1_2 = ops[38].mult;
    op_vt1t1_1 = ops[39].mult;
    op_t2_8 = ops[40].mult;

    CorFortran(0, &op_t2_1, icsd_t2_1_);
    CorFortran(0, t2_2_1, offset_icsd_t2_2_1_);
    CorFortran(0, &op_t2_2_1, icsd_t2_2_1_);
    CorFortran(0, t2_2_2_1, offset_icsd_t2_2_2_1_);
    CorFortran(0, &op_t2_2_2_1, icsd_t2_2_2_1_);
    CorFortran(0, t2_2_2_2_1, offset_icsd_t2_2_2_2_1_);
    CorFortran(0, &op_t2_2_2_2_1, icsd_t2_2_2_2_1_);
    CorFortran(0, &op_t2_2_2_2_2, icsd_t2_2_2_2_2_);
    CorFortran(0, &op_t2_2_2_2, icsd_t2_2_2_2_);
    destroy(t2_2_2_2_1);
    CorFortran(0, &op_t2_2_2_3, icsd_t2_2_2_3_);
    CorFortran(0, &op_t2_2_2, icsd_t2_2_2_);
    destroy(t2_2_2_1);
    CorFortran(0, t2_2_4_1, offset_icsd_t2_2_4_1_);
    CorFortran(0, &op_t2_2_4_1, icsd_t2_2_4_1_);
    CorFortran(0, &op_t2_2_4_2, icsd_t2_2_4_2_);
    CorFortran(0, &op_t2_2_4, icsd_t2_2_4_);
    destroy(t2_2_4_1);
    CorFortran(0, t2_2_5_1, offset_icsd_t2_2_5_1_);
    CorFortran(0, &op_t2_2_5_1, icsd_t2_2_5_1_);
    CorFortran(0, &op_t2_2_5_2, icsd_t2_2_5_2_);
    CorFortran(0, &op_t2_2_5, icsd_t2_2_5_);
    destroy(t2_2_5_1);
    CorFortran(0, &op_t2_2_6, icsd_t2_2_6_);
    CorFortran(0, &op_t2_2, icsd_t2_2_);
    destroy(t2_2_1);
    CorFortran(0, &op_lt2_3x, icsd_t2_lt2_3x_);
    CorFortran(0, t2_4_1, offset_icsd_t2_4_1_);
    CorFortran(0, &op_t2_4_1, icsd_t2_4_1_);
    CorFortran(0, t2_4_2_1, offset_icsd_t2_4_2_1_);
    CorFortran(0, &op_t2_4_2_1, icsd_t2_4_2_1_);
    CorFortran(0, &op_t2_4_2_2, icsd_t2_4_2_2_);
    CorFortran(0, &op_t2_4_2, icsd_t2_4_2_);
    destroy(t2_4_2_1);
    CorFortran(0, &op_t2_4_3, icsd_t2_4_3_);
    CorFortran(0, &op_t2_4_4, icsd_t2_4_4_);
    CorFortran(0, &op_t2_4, icsd_t2_4_);
    destroy(t2_4_1);
    CorFortran(0, t2_5_1, offset_icsd_t2_5_1_);
    CorFortran(0, &op_t2_5_1, icsd_t2_5_1_);
    CorFortran(0, &op_t2_5_2, icsd_t2_5_2_);
    CorFortran(0, &op_t2_5_3, icsd_t2_5_3_);
    CorFortran(0, &op_t2_5, icsd_t2_5_);
    destroy(t2_5_1);
    CorFortran(0, t2_6_1, offset_icsd_t2_6_1_);
    CorFortran(0, &op_t2_6_1, icsd_t2_6_1_);
    CorFortran(0, t2_6_2_1, offset_icsd_t2_6_2_1_);
    CorFortran(0, &op_t2_6_2_1, icsd_t2_6_2_1_);
    CorFortran(0, &op_t2_6_2_2, icsd_t2_6_2_2_);
    CorFortran(0, &op_t2_6_2, icsd_t2_6_2_);
    destroy(t2_6_2_1);
    CorFortran(0, &op_t2_6_3, icsd_t2_6_3_);
    CorFortran(0, &op_t2_6, icsd_t2_6_);
    destroy(t2_6_1);
    CorFortran(0, t2_7_1, offset_icsd_t2_7_1_);
    CorFortran(0, &op_t2_7_1, icsd_t2_7_1_);
    CorFortran(0, &op_t2_7_2, icsd_t2_7_2_);
    CorFortran(0, &op_t2_7_3, icsd_t2_7_3_);
    CorFortran(0, &op_t2_7, icsd_t2_7_);
    destroy(t2_7_1);
    CorFortran(0, &op_vt1t1_1_2, icsd_t2_vt1t1_1_2_);
    CorFortran(0, vt1t1_1, offset_icsd_t2_vt1t1_1_);
    CorFortran(0, &op_vt1t1_1, icsd_t2_vt1t1_1_);
    CorFortran(0, &op_t2_8, icsd_t2_8_);
#endif  // use c scheduler
  f->detach();
  i0->detach();
  t1->detach();
  t2->detach();
  v->detach();
  c2->detach();
}
}  // extern C
};  // namespace tamm
