#include "variables.h"
#include <iostream>
#include "tensor.h"
#include "t_mult.h"
#include "t_assign.h"
#include "expression.h"

namespace ctce {

  static Assignment a_t2_7_1;
  static Multiplication m_t2_7_2, m_t2_7_3, m_t2_7;
  static Tensor t2_7_1 = Tensor4(H6B,P3B,H1B,P5B,0,1,2,3,iV_tensor, dist_nw, dim_ov);;

  extern "C" {

    void gen_expr_t2_71_cxx_() {

      static bool set_t2_7 = true;
      Tensor tC, tA, tB;
      DistType idist = (Variables::intorb()) ? dist_nwi : dist_nw;

      if (set_t2_7) {
        //std::cout << "set t2 7.\n";

        /* i1 ( h6 p3 h1 p5 )_v + = 1 * v ( h6 p3 h1 p5 )_v */
        tC = Tensor4(H6B,P3B,H1B,P5B,0,1,2,3,iV_tensor, dist_nw, dim_ov);
        tA = Tensor4(H6B,P3B,H1B,P5B,0,1,2,3,V_tensor, idist, dim_n);
        a_t2_7_1 = Assignment(tC,tA,1.0, ivec(H6B,P3B,H1B,P5B), ivec(H6B,P3B,H1B,P5B));

        /* i1 ( h6 p3 h1 p5 )_vt + = -1 * Sum ( p7 ) * t ( p7 h1 )_t * v ( h6 p3 p5 p7 )_v */
        tC = Tensor4(H6B,P3B,H1B,P5B,0,1,2,3,iVT_tensor, dist_nw, dim_ov);
        tA = Tensor2(P7B,H1B,0,1,T_tensor, dist_nwma, dim_ov);
        //tA = Tensor2(P7B,H1B,0,1,T_tensor, idist, dim_n);
        tB = Tensor4(H6B,P3B,P5B,P7B,0,1,2,2,V_tensor, idist, dim_n);
        m_t2_7_2 = Multiplication(tC,tA,tB,-1.0);

        /* i1 ( h6 p3 h1 p5 )_vt + = -1/2 * Sum ( h8 p7 ) * t ( p3 p7 h1 h8 )_t * v ( h6 h8 p5 p7 )_v */
        tC = Tensor4(H6B,P3B,H1B,P5B,0,1,2,3,iVT_tensor, dist_nw, dim_ov);
        tA = Tensor4(P3B,P7B,H1B,H8B,0,0,1,1,T_tensor, dist_nw, dim_ov);
        tB = Tensor4(H6B,H8B,P5B,P7B,0,0,1,1,V_tensor, idist, dim_n);
        m_t2_7_3 = Multiplication(tC,tA,tB,-0.50);

        /* i0 ( p3 p4 h1 h2 )_vt + = -1 * P( 4 ) * Sum ( h6 p5 ) * t ( p3 p5 h1 h6 )_t * i1 ( h6 p4 h2 p5 )_v */
        tC = Tensor4(P3B,P4B,H1B,H2B,0,0,1,1,iVT_tensor, dist_nw, dim_ov);
        tA = Tensor4(P3B,P5B,H1B,H6B,0,0,1,1,T_tensor, dist_nw, dim_ov);
        tB = Tensor4(H6B,P4B,H2B,P5B,0,1,2,3,V_tensor, idist, dim_ov);
        m_t2_7 = Multiplication(tC,tA,tB,-1.0);

        //ccsd_t2_7_1_createfile_cxx: i1 ( h6 p3 h1 p5 )_v
        t2_7_1 = Tensor4(H6B,P3B,H1B,P5B,0,1,2,3,iV_tensor, dist_nw, dim_ov);

        set_t2_7 = false;
      }
    } // gen_expr_t2_71_cxx_

    // something is wrong with t2_7_1 :(
    /* i1 ( h6 p3 h1 p5 )_v + = 1 * v ( h6 p3 h1 p5 )_v */
    void ccsd_t2_7_1_cxx_(Integer *d_a, Integer *k_a_offset, Integer *d_c, Integer *k_c_offset) {
      t_assign3(d_a, k_a_offset, d_c, k_c_offset, a_t2_7_1);

    } // t2_7_1

    void ccsd_t2_7_1_createfile_cxx_(Integer *k_i1_offset, Integer *d_i1, Integer *size_i1) {
      t2_7_1.create(k_i1_offset, d_i1, size_i1);
    }

    void ccsd_t2_7_1_deletefile_cxx_() {
      t2_7_1.destroy();
    }

    // what is peta?
    /* i1 ( h6 p3 h1 p5 )_vt + = -1 * Sum ( p7 ) * t ( p7 h1 )_t * v ( h6 p3 p5 p7 )_v */
    void ccsd_t2_7_2_cxx_(Integer *d_a, Integer *k_a_offset,
        Integer *d_b, Integer *k_b_offset, Integer *d_c, Integer *k_c_offset) {
      t_mult4(d_a, k_a_offset, d_b, k_b_offset, d_c, k_c_offset, m_t2_7_2);
    } // t2_7_2

    /* i1 ( h6 p3 h1 p5 )_vt + = -1/2 * Sum ( h8 p7 ) * t ( p3 p7 h1 h8 )_t * v ( h6 h8 p5 p7 )_v */
    void ccsd_t2_7_3_cxx_(Integer *d_a, Integer *k_a_offset,
        Integer *d_b, Integer *k_b_offset, Integer *d_c, Integer *k_c_offset) {
      t_mult4(d_a, k_a_offset, d_b, k_b_offset, d_c, k_c_offset, m_t2_7_3);
    } // t2_7_3

    /* i0 ( p3 p4 h1 h2 )_vt + = -1 * P( 4 ) * Sum ( h6 p5 ) * t ( p3 p5 h1 h6 )_t * i1 ( h6 p4 h2 p5 )_v */
    void ccsd_t2_7_cxx_(Integer *d_a, Integer *k_a_offset,
        Integer *d_b, Integer *k_b_offset, Integer *d_c, Integer *k_c_offset) {
      t_mult4(d_a, k_a_offset, d_b, k_b_offset, d_c, k_c_offset, m_t2_7);
    } // t2_7

  } // extern C
}; // namespace ctce

