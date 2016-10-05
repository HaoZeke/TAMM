#include <iostream>
#include <map>
#include <string>
#include "define.h"
#include "input.h"

//#include "equations.h"

using namespace std;
using namespace tamm;

namespace tamm {
struct Equations {
  std::vector<RangeEntry> range_entries;
  std::vector<IndexEntry> index_entries;
  std::map<std::string, TensorEntry> tensor_entries;
  std::vector<OpEntry> op_entries;
};
void ccsd_t1_equations(Equations &eqs);
}

namespace tamm {
void pretty_print(Equations &eqs);
void print_flatten(Equations &eqs, int opid);
}

int count_v(const TensorEntry &te, const std::vector<RangeType> &rts) {
  int nv = 0;
  for (int i = 0; i < te.ndim; i++) {
    int rid = te.range_ids[i];
    if (rts[rid] == TO) {
      nv += 0;
    } else if (rts[rid] == TV) {
      nv += 1;
    } else if (rts[rid] == TN) {
      nv += 1;  //@BUG @FIXME. treats TN as TV.
    } else {
      assert(0);
    }
  }
  return nv;
}

std::vector<RangeType> compute_range_type(
    const std::vector<tamm::RangeEntry> &re) {
  std::vector<RangeType> rts(re.size(), TO);

  for (int i = 0; i < re.size(); i++) {
    std::string rname = (re[i].name);
    if (!strcmp(rname.c_str(), OSTR)) {
      rts[i] = TO;
      continue;
    } else if (!strcmp(rname.c_str(), VSTR)) {
      rts[i] = TV;
      continue;
    } else if (!strcmp(rname.c_str(), NSTR)) {
      rts[i] = TN;
      continue;
    } else {
      printf("Unsupported range type %s\n", rname.c_str());
      exit(1);
    }
  }
  return rts;
}

struct dep {
  int s, d;
  dep(int s_, int d_) : s(s_), d(d_) {}
};

std::vector<dep> compute_deps(const tamm::Equations &eqs) {
  std::vector<dep> deps;
  int n = eqs.op_entries.size();

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < i; j++) {
      std::string ra1, ra2, wa, rb1, rb2, wb;
      const OpEntry &opi = eqs.op_entries[i];
      switch (opi.optype) {
        case OpTypeAdd:
          wa = opi.add.tc;
          ra1 = opi.add.ta;
          break;
        case OpTypeMult:
          wa = opi.mult.tc;
          ra1 = opi.mult.ta;
          ra2 = opi.mult.tb;
          break;
        default:
          assert(0);
      }
      const OpEntry &opj = eqs.op_entries[j];
      switch (opj.optype) {
        case OpTypeAdd:
          wb = opj.add.tc;
          rb1 = opj.add.ta;
          break;
        case OpTypeMult:
          wb = opj.mult.tc;
          rb1 = opj.mult.ta;
          rb2 = opj.mult.tb;
          break;
        default:
          assert(0);
      }
      if (!ra1.compare(wb) || !ra2.compare(wb) || !rb1.compare(wa) ||
          !rb2.compare(wa)) {
        deps.push_back(dep(j, i));
      }
    }
  }
  return deps;
}

void print_ilp_info(tamm::Equations &eqs) {
  std::map<std::string, Tensor> tensors;
  std::vector<Operation> ops;

  std::vector<RangeType> rts = compute_range_type(eqs.range_entries);

  std::cout << "ntensors: " << eqs.tensor_entries.size() << endl
            << "nops: " << eqs.op_entries.size() << endl;
  int maxdim = -1;

  // for(int i=0; i<eqs.tensor_entries.size(); i++) {
  for (std::map<std::string, TensorEntry>::iterator i =
           eqs.tensor_entries.begin();
       i != eqs.tensor_entries.end(); i++) {
    maxdim = max(maxdim, eqs.tensor_entries[i->first].ndim);
  }
  // for(int i=0; i<eqs.tensor_entries.size(); i++) {
  for (std::map<std::string, TensorEntry>::iterator i =
           eqs.tensor_entries.begin();
       i != eqs.tensor_entries.end(); i++) {
    const TensorEntry &te = eqs.tensor_entries[i->first];
    // std::cout<<"size["<<i+1<<"]: ";
    std::cout << "size[" << te.name << "]: ";
    if (te.ndim < maxdim) {
      std::cout << "0" << endl;
    } else {
      std::cout << (1 << count_v(te, rts)) << endl;
    }
  }
  std::vector<dep> deps = compute_deps(eqs);
  // std::cout<<"ndeps :"<<deps.size()<<endl;
  for (int i = 0; i < deps.size(); i++) {
    std::cout << "dep[" << deps[i].s + 1 << "] : " << deps[i].d + 1 << endl;
  }
  for (int i = 0; i < eqs.op_entries.size(); i++) {
    std::cout << "access[" << i + 1 << "] : ";
    const tamm::OpEntry &opi = eqs.op_entries[i];
    switch (opi.optype) {
      case OpTypeAdd:
        std::cout << opi.add.tc << "," << opi.add.ta << endl;
        break;
      case OpTypeMult:
        std::cout << opi.mult.tc << "," << opi.mult.ta << "," << opi.mult.tb
                  << endl;
        break;
      default:
        assert(0);
    }
  }
}

int main() {
  Equations eqs;
  ccsd_t1_equations(eqs);

  // std::cout<<"nops="<<eqs.op_entries.size()<<endl;
  // print_flatten(eqs, eqs.op_entries.size()-2);
  pretty_print(eqs);
  // print_ilp_info(eqs);

  return 0;
}
