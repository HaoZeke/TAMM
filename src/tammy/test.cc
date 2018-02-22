#include "ao.h"
#include "boundvec.h"
#include "distribution.h"
#include "index_space.h"
#include "labeled_tensor.h"
#include "memory_manager.h"
#include "memory_manager_local.h"
#include "mso.h"
#include "ops.h"
#include "perm_symmetry.h"
#include "proc_group.h"
#include "scheduler.h"
#include "strong_num.h"
#include "sub_ao_mso.h"
#include "tensor.h"
#include "tensor_base.h"
#include "types.h"

// //#include "memory_manager_ga.h"
// #include "proc_group.h"
// #include "tensor_base.h"
// #include "types.h"
// #include "distribution.h"
// #include "generator.h"
// #include "memory_manager_local.h"
// #include "errors.h"
// #include "memory_manager.h"
// #include "perm_symmetry.h"
// #include "strong_num.h"
// #include "block.h"
// #include "labeled_block.h"
// #include "labeled_tensor.h"

// #include "tensor.h"
// #include "index_sort.h"
// #include "execution_context.h"
// #include "operators.h"

//#include "ops.h"

using namespace tammy;

int main() {
    MSO mso;
    AO ao;
    IndexLabel i, j, k, l, m, n, E;
    std::tie(i, j, k, l) = mso.N(0, 1, 2, 3);

    SubAO_MSO sub_ao{mso, ao};
    IndexLabel i1, j1, k1, l1;
    std::tie(i1, j1, k1, l1) = sub_ao.N(0, 1, 2, 3);

    // do we want , or just +. Are these indices neither upper nor lower
    auto T1 = Tensor<double>::create<TensorImpl<double>>(i, j);
    auto T2 = Tensor<double>::create<TensorImpl<double>>(i | j | E);
    auto T3 = Tensor<double>::create<TensorImpl<double>>(E | k | j);
    auto T4 = Tensor<double>::create<TensorImpl<double>>(j + l | E | E);

    // dependent index labels should only be constructed for dependent index
    // spaces
    // auto T2 = Tensor<double>::create<TensorImpl<double>>(i1(k) + j | k | E);
    // auto T3 = Tensor<double>::create<TensorImpl<double>>(E | i + j1(l) | l);
    // auto T4 = Tensor<double>::create<TensorImpl<double>>(i1(k) + j1(l) | k |
    // l);

    // Valid Tensor Creation
    auto V_T1 =
      Tensor<double>::create<TensorImpl<double>>(i1(j), j); // disabled for now
    auto V_T2 = Tensor<double>::create<TensorImpl<double>>(i + j | E | E);
    auto V_T3 = Tensor<double>::create<TensorImpl<double>>(E | i1(k) | k);
    auto V_T4 = Tensor<double>::create<TensorImpl<double>>(E | i1(k) + j | k);
    auto V_T5 =
      Tensor<double>::create<TensorImpl<double>>(E | i1(k) + j1(l) | k + l);
    auto V_T6 = Tensor<double>::create<TensorImpl<double>>(i | j | k);
    auto V_T7 =
      Tensor<double>::create<TensorImpl<double>>(i + j | E | k1(j) + l);

#if 0
  Invalid Tensor Creation
  auto I_T1 = Tensor<double>::create<TensorImpl<double>>(i(k)); // IndexLabel k is not present
  auto I_T2 = Tensor<double>::create<TensorImpl<double>>(i(k) | E | E); // IndexLabel k is not present
  auto I_T3 = Tensor<double>::create<TensorImpl<double>>(i(k) + j(l) | E | k); // IndexLabel l is not present
  auto I_T4 = Tensor<double>::create<TensorImpl<double>>(i(k) + j(l) | E | E); // IndexLabel k and l is not present
  auto I_T5 = Tensor<double>::create<TensorImpl<double>>(i + i | E | E); // duplicated index labels
  auto I_T6 = Tensor<double>::create<TensorImpl<double>>(i(k) + i | E | E); // duplicated index labels

#endif

    // Failing operations
    // General Case: tensor with same index label
    // T1(i,j) = T2(k,k);
    // T1(i,j) += T2(j,j);
    // T1(i,j) = T2(j,i) * T3(k,k);
    // T1(i,i) += T2(i,j) * T3(k,l);

    // // General Case: length of the index label vector does not match the rank
    // in the tensor T1(i,j) += T2(j,i,k); T1(i,j) += T2(j,i) * T3(i,j,l);

    // // Addition Case: each index label should occur exactly once in LHS and
    // RHS T1(i,j) += T2(j,i,j);

    // // Multiplication Case: every outer index label appears in exactly one
    // RHS tensor T1(i,j) = T2(j,i) * T4(i,k);

    // // Multiplication Case: every inner index label appears exactly once in
    // both RHS tensors T1(i,j) += T2(j,i) * T3(k,j);

    // Passing operations
    T1(i, j) = 0;
    T1(i, j) += .52;
    T1(i, j) = T2(j, i);
    T1(i, j) += T2(j, i);
    T1(i, j) = 3 * T2(j, i);
    T1(i, j) += 3 * T2(j, i);
    T1(i, j) = T2(i, k) * T3(k, j);
    T1(i, j) += T2(i, k) * T3(k, j);
    // T1(i,j,k,l) = outer({i-j,-1}, {k-l, 1}) * inner(m,n) * 3 * T2(i,j,m,n) *
    // T4(k,l,m,n);
    T1(i, j, k, l) = 3 * T2(i, j, m, n) * T4(k, l, m, n);
    T1(i, j) += 3 * T2(i, l) * T4(j, l);

    // Scheduler sch{ProcGroup{}, nullptr, nullptr};

    // sch
    //     .tensors(T1,T2,T3)
    //     .live_in(T2, T3)
    //     .live_out(T1)
    //     (  T1(i,j) = 0,
    //         T1(i,j) += .52,
    //         T1(i,j) = T2(j,i),
    //         T1(i,j) += T2(j,i),
    //         T1(i,j) = 3 * T2(j,i),
    //         T1(i,j) += 3 * T2(j,i),
    //         T1(i,j) = T2(i,k) * T3(k,j),
    //         T1(i,j) += T2(i,k) * T3(k,j),
    //         T1(i,j) = 3 * T2(i,l) * T4(j,l),
    //         T1(i,j) += 3 * T2(i,l) * T4(j,l)
    //      )
    //     .execute();
    // #endif

#if 0
  Scheduler()(
      T1(i,j)  = 0,
      T1(i,j) += .52,
      T1(i,j)  =     T2(j,i),
      T1(i,j) +=     T2(j,i),
      T1(i,j)  = 3 * T2(j,i),
      T1(i,j) += 3 * T2(j,i),
      T1(i,j)  =     T2(j,i) * T3(k,j),
      T1(i,j) +=     T2(j,i) * T3(k,j),
      T1(i,j)  = 3 * T2(j,i) * T3(j,l),
      T1(i,j) += 3 * T2(j,i) * T3(j,l)
              ).execute();
#endif

    return 0;
}
