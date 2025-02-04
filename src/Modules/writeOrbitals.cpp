#include "writeOrbitals.hpp"
#include "IO/InputBlock.hpp"
#include "Modules/modules_list.hpp"
#include "Wavefunction/DiracSpinor.hpp"
#include "Wavefunction/Wavefunction.hpp"
#include "qip/String.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace Module {

//==============================================================================
static void write_orbitals(const std::string &fname,
                           const std::vector<DiracSpinor> &orbs, int l) {
  if (orbs.empty())
    return;
  const auto &gr = orbs.front().grid();

  std::ofstream of(fname);
  of << "r ";
  for (auto &psi : orbs) {
    if (psi.l() != l && l >= 0)
      continue;
    of << "\"" << psi.symbol(true) << "\" ";
  }
  of << "\n";

  of << "# f block\n";
  for (std::size_t i = 0; i < gr.num_points(); i++) {
    of << gr.r(i) << " ";
    for (auto &psi : orbs) {
      if (psi.l() != l && l >= 0)
        continue;
      of << psi.f(i) << " ";
    }
    of << "\n";
  }

  of << "\n# g block\n";
  for (std::size_t i = 0; i < gr.num_points(); i++) {
    of << gr.r(i) << " ";
    for (auto &psi : orbs) {
      if (psi.l() != l && l >= 0)
        continue;
      of << psi.g(i) << " ";
    }
    of << "\n";
  }

  of.close();
  std::cout << "Orbitals written to file: " << fname << "\n";
}

//==============================================================================
void writeOrbitals(const IO::InputBlock &input, const Wavefunction &wf) {
  const std::string ThisModule = "Module::WriteOrbitals";
  input.check({{"label", ""}, {"l", ""}});
  // If we are just requesting 'help', don't run module:
  if (input.has_option("help")) {
    return;
  }

  std::cout << "\n Running: " << ThisModule << "\n";
  const auto label = input.get<std::string>("label", "");
  // to write only for specific l. l<0 means all
  auto l = input.get("l", -1);

  std::string oname = wf.atomicSymbol();
  if (label != "")
    oname += "_" + label;

  write_orbitals(oname + "_core.txt", wf.core(), l);
  write_orbitals(oname + "_valence.txt", wf.valence(), l);
  write_orbitals(oname + "_basis.txt", wf.basis(), l);
  write_orbitals(oname + "_spectrum.txt", wf.spectrum(), l);
}

} // namespace Module