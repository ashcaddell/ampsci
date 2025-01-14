#include "Physics/periodicTable.hpp"
#include "Physics/AtomData.hpp"
#include "Physics/AtomData_PeriodicTable.hpp"
#include "Physics/NuclearData.hpp"
#include "Physics/PhysConst_constants.hpp"
#include "qip/String.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

namespace AtomData {

void instructions() {
  std::cout
      << "Usage:\n"
         "Cs         Info for Cs with default A\n"
         "Cs 137     Info for Cs-137\n"
         "Cs all     Info for all available Cs isotopes\n"
         "(Note: numbers come from online database, and should be checked)\n";
}

void printData(const Nuclear::Isotope &nuc) {

  std::cout << AtomData::atomicSymbol(nuc.Z) << "-" << nuc.A << " (Z=" << nuc.Z
            << ", A=" << nuc.A << ")\n";

  std::cout << "r_rms = ";
  nuc.r_ok() ? std::cout << nuc.r_rms : std::cout << "??";
  std::cout << ", c = ";
  nuc.r_ok() ? std::cout << Nuclear::c_hdr_formula_rrms_t(nuc.r_rms) :
               std::cout << "??";
  std::cout << ", mu = ";
  nuc.mu_ok() ? std::cout << nuc.mu : std::cout << "??";

  std::cout << ", I = ";
  nuc.I_ok() ? std::cout << nuc.I_N : std::cout << "??";
  std::cout << ", parity = ";
  nuc.parity_ok() ? std::cout << nuc.parity : std::cout << "??";
  std::cout << "\n";
}

int parse_A(const std::string &A_str, int z) {

  int a = 0;
  if (qip::string_is_integer(A_str)) {
    a = std::stoi(A_str);
  } else {
    std::cout << "Invalid A: " << A_str << "\n";
    instructions();
    std::abort();
  }

  if (a <= 0)
    a = AtomData::defaultA(z);
  return a;
}
//==============================================================================
void printConstants() //
{
  std::cout << "\n";
  printf("1/alpha = %.9f\n", PhysConst::c);
  printf("alpha   = %.14f\n", PhysConst::alpha);
  printf("c       = %.0f m/s\n", PhysConst::c_SI);
  printf("hbar    = %.10e Js\n", PhysConst::hbar_SI);
  printf("mp/me   = %.8f\n", PhysConst::m_p);
  printf("me      = %.10f MeV\n", PhysConst::m_e_MeV);
  printf("aB      = %.12e m\n", PhysConst::aB_m);
  printf("Hy      = %.12f eV = %.12e Hz\n", PhysConst::Hartree_eV,
         PhysConst::Hartree_Hz);
  std::cout << "\n";
  return;
}

//==============================================================================
void periodicTable(std::string z_str, std::string a_str) {
  if (a_str == "")
    a_str = "0";

  instructions();
  AtomData::printTable();

  while (true) {

    const auto z = AtomData::atomic_Z(z_str);
    if (z != 0) {

      z_str = AtomData::atomicSymbol(z);

      auto name = AtomData::atomicName(z);

      std::vector<Nuclear::Isotope> isotopes;
      int a_default = parse_A("0", z);
      if (a_str == "all" || a_str == "list") {
        isotopes = Nuclear::findIsotopeList(z);
      } else {
        int a = parse_A(a_str, z);
        isotopes.push_back(Nuclear::findIsotopeData(z, a));
      }

      auto core_str = AtomData::guessCoreConfigStr(z);
      auto core_vec = AtomData::core_parser(core_str);

      std::cout << "\n"
                << z_str << ",  " << name << ".\n"
                << "Z = " << z << ";  A = " << a_default << " (default)\n\n";
      std::cout << "Electron config: " << core_str << "   (guess)\n"
                << " = ";
      for (const auto &term : core_vec) {
        if (term.frac() < 1)
          std::cout << "| ";
        std::cout << term.symbol() << " ";
      }
      std::cout << "\n";

      std::cout << "\nIsotpe data:";
      if (isotopes.empty()) {
        std::cout << " none known\n";
      }
      for (const auto &nuc : isotopes) {
        std::cout << "\n";
        printData(nuc);
      }
    }

    std::cout << "\nEnter atom (and optional isotope), or ctrl+c to exit\n";
    std::string s1;
    std::getline(std::cin, s1);
    std::stringstream ss(s1);
    ss >> z_str >> a_str;
    if (a_str == "")
      a_str = "0";
    std::cout << "----------------------------------------------\n";
  }
}

} // namespace AtomData