#include "AtomInfo.hpp"
#include "ChronoTimer.hpp"
#include "DiracOperator.hpp"
#include "HartreeFockClass.hpp"
#include "Nuclear.hpp"
#include "Operators.hpp"
#include "PhysConst_constants.hpp"
#include "UserInput.hpp"
#include "Wavefunction.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  ChronoTimer timer; // start the overall timer

  std::string input_file = (argc > 1) ? argv[1] : "hartreeFock.in";
  std::cout << "Reading input from: " << input_file << "\n";

  // // // Input options
  UserInput input(input_file);
  auto atom = input.get<std::string>("Atom", "Z");
  auto varalpha = sqrt(input.get("Atom", "varAlpha2", 1.0));
  auto A = input.get("Nucleus", "A", -1);
  auto r0 = input.get("Grid", "r0", 1.0e-5);
  auto rmax = input.get("Grid", "rmax", 150.0);
  auto ngp = input.get("Grid", "ngp", 1600);
  auto str_core = input.get<std::string>("HartreeFock", "Core");
  auto eps_HF = input.get("HartreeFock", "Convergance", 1.0e-14);
  auto num_val = input.get("HartreeFock", "num_val", 0);
  auto l_max = input.get("HartreeFock", "l_max", 0);
  auto exclude_exchange = input.get("HartreeFock", "excludeExchange", false);

  bool run_test = input.get("HartreeFock", "run_test", false);

  auto Z = AtomInfo::get_z(atom);
  Wavefunction wf(Z, A, ngp, r0, rmax, varalpha);

  if (exclude_exchange)
    std::cout << "\nRunning Hartree (excluding exchange) for ";
  else
    std::cout << "\nRunning Hartree-Fock for ";
  std::cout << wf.atom() << "\n"
            << wf.nuclearParams() << "\n"
            << wf.rgrid.gridParameters() << "\n"
            << "********************************************************\n";

  // Solve Hartree equations for the core:
  timer.start(); // start the timer for HF
  HartreeFock hf(wf, str_core, eps_HF, exclude_exchange);
  double core_energy = hf.calculateCoreEnergy();
  std::cout << "core: " << timer.lap_reading_str() << "\n";

  // Create list of valence states to solve for
  if ((int)wf.Ncore() >= wf.Znuc())
    num_val = 0;
  auto val_lst = wf.listOfStates_nk(num_val, l_max);

  // Solve for the valence states:
  timer.start();
  for (const auto &nk : val_lst) {
    hf.solveNewValence(nk.n, nk.k);
  }
  if (val_lst.size() > 0)
    std::cout << "Valence: " << timer.lap_reading_str() << "\n";
  // wf.orthonormaliseOrbitals(wf.valence_orbitals, 2);

  // Output results:
  std::cout << "\nHartree Fock: " << wf.atom() << "\n";
  bool sorted = true;
  wf.printCore(sorted);
  std::cout << "E_core = " << core_energy
            << " au;  = " << core_energy * PhysConst::Hartree_invcm << "/cm\n";
  wf.printValence(sorted);

  std::cout << "\n Total time: " << timer.reading_str() << "\n";

  //*********************************************************
  //               TESTS
  //*********************************************************

  bool test_hf_basis = false;
  if (test_hf_basis) {
    auto basis_lst = wf.listOfStates_nk(6, 3);
    std::vector<DiracSpinor> basis = wf.core_orbitals;
    for (const auto &nk : basis_lst) {
      basis.emplace_back(DiracSpinor(nk.n, nk.k, wf.rgrid));
      auto tmp_vex = std::vector<double>{};
      hf.solveValence(basis.back(), tmp_vex);
    }
    wf.orthonormaliseOrbitals(basis, 2);
    wf.printValence(false, basis);
    std::cout << "\n Total time: " << timer.reading_str() << "\n";
  }

  if (run_test) {

    std::cout << "Test approximate exchange potential\n"
              << "calculates: <psi| V_ex - V_ex^approx |psi>\n"
              << "(Note: this is lower-bound on error!)\n";
    for (const auto p_orbs : {&wf.core_orbitals, &wf.valence_orbitals}) {
      for (const auto &psi : *p_orbs) {
        DiracOperator z3(hf.get_vex(psi));
        auto dPsi_appoax = (z3 * psi);
        auto dPsi_real = hf.vex_psia(psi);
        auto delta_e = psi * dPsi_real - psi * dPsi_appoax;
        std::cout << psi.symbol() << " " << delta_e << "\n";
      }
    }

    std::cout << "Test orthonormality [log-scale, should all read 0]:\n";
    for (int i = 0; i < 3; i++) {
      const auto &tmp_b = (i == 2) ? wf.valence_orbitals : wf.core_orbitals;
      const auto &tmp_a = (i == 0) ? wf.core_orbitals : wf.valence_orbitals;
      // Core-Core:
      if (i == 0)
        std::cout << "\nCore-Core\n    ";
      else if (i == 1)
        std::cout << "\nValence-Core\n    ";
      else
        std::cout << "\nValence-Valence\n    ";
      for (auto &psi_b : tmp_b)
        printf("%2i%2i", psi_b.n, psi_b.k);
      std::cout << "\n";
      for (auto &psi_a : tmp_a) {
        printf("%2i%2i", psi_a.n, psi_a.k);
        for (auto &psi_b : tmp_b) {
          if (psi_b > psi_a) {
            std::cout << "    ";
            continue;
          }
          if (psi_a.k != psi_b.k) {
            std::cout << "    ";
            continue;
          }
          double xo = (psi_a * psi_b);
          if (psi_a.n == psi_b.n)
            xo -= 1;
          if (xo == 0)
            printf("   0");
          else
            printf(" %+3.0f", log10(fabs(xo)));
        }
        std::cout << "\n";
      }
    }
    std::cout << "\n(Note: Core orbitals are orthogonalised explicitely, as is "
                 "each valence state [with respect to the core]. However, "
                 "valence states are not explicitely orthogonalised wrt each "
                 "other, since there's no self-consistent way to do this with "
                 "a finite set of valence orbitals).\n";

    std::cout << "\nTesting wavefunctions: <n|H|n>  (numerical error)\n";
    double c = 1. / wf.get_alpha();
    DiracOperator w(c, GammaMatrix::g5, 1, true);
    RadialOperator x_a(wf.rgrid, -1);
    DiracOperator y(c * c, DiracMatrix(0, 0, 0, -2));
    DiracOperator z1(wf.vnuc);
    DiracOperator z2(wf.vdir);
    for (auto &tmp_orbs : {wf.core_orbitals, wf.valence_orbitals}) {
      for (auto &psi : tmp_orbs) {
        auto k = psi.k;
        DiracOperator z3(hf.get_vex(psi));
        // auto vexPsi = (z3 * psi);
        auto vexPsi = hf.vex_psia(psi);
        DiracOperator x_b(c, DiracMatrix(0, 1 - k, 1 + k, 0), 0, true);
        auto rhs = (w * psi) + (x_a * (x_b * psi)) + (y * psi) + (z1 * psi) +
                   (z2 * psi) + vexPsi;
        double R = psi * rhs;
        double ens = psi.en;
        double fracdiff = (R - ens) / ens;
        printf("<%i% i|H|%i% i> = %17.11f, E = %17.11f; % .0e\n", psi.n, psi.k,
               psi.n, psi.k, R, ens, fracdiff);
      }
    }
    std::cout << "\n Total time: " << timer.reading_str() << "\n";
  }

  bool print_wfs = false;
  if (print_wfs) {
    std::ofstream of("hf-orbitals.txt");
    of << "r ";
    for (auto &psi : wf.core_orbitals)
      of << "\"" << psi.symbol(true) << "\" ";
    for (auto &psi : wf.valence_orbitals)
      of << "\"" << psi.symbol(true) << "\" ";
    of << "\n";
    for (std::size_t i = 0; i < wf.rgrid.ngp; i++) {
      of << wf.rgrid.r[i] << " ";
      for (auto &psi : wf.core_orbitals)
        of << psi.f[i] << " ";
      for (auto &psi : wf.valence_orbitals)
        of << psi.f[i] << " ";
      of << "\n";
    }
  }

  bool testpnc = false;
  if (testpnc) {
    double t = Nuclear::default_t; // approximate_t_skin(wf.Anuc());
    auto r_rms = Nuclear::find_rrms(wf.Znuc(), wf.Anuc());
    double c = Nuclear::c_hdr_formula_rrms_t(r_rms);
    PNCnsiOperator hpnc(c, t, wf.rgrid, -wf.Nnuc());
    E1Operator he1(wf.rgrid);

    double Ac = 2. / 6.; // angular coef
    auto a6s_i = wf.getStateIndex(6, -1);
    auto a7s_i = wf.getStateIndex(7, -1);
    auto &a6s = wf.valence_orbitals[a6s_i];
    auto &a7s = wf.valence_orbitals[a7s_i];
    std::cout << "E_pnc: " << wf.Anuc() << "-"
              << AtomInfo::atomicSymbol(wf.Znuc()) << " " << a6s.symbol()
              << " -> " << a7s.symbol() << "\n";

    double pnc = 0;
    for (int i = 0; i < 2; i++) {
      auto &tmp_orbs = (i == 0) ? wf.core_orbitals : wf.valence_orbitals;
      for (auto &np : tmp_orbs) {
        if (np.k != 1)
          continue; // p_1/2 only
        // <7s|d|np><np|hw|6s>/dE6s + <7s|hw|np><np|d|6s>/dE7s
        double pnc1 =
            Ac * (a7s * (he1 * np)) * (np * (hpnc * a6s)) / (a6s.en - np.en);
        double pnc2 =
            Ac * (a7s * (hpnc * np)) * (np * (he1 * a6s)) / (a7s.en - np.en);
        std::cout << "n=" << np.n << " pnc= " << pnc1 << " + " << pnc2 << " = "
                  << pnc1 + pnc2 << "\n";
        pnc += pnc1 + pnc2;
      }
    }
    std::cout << "Total= " << pnc << "\n";
    std::cout << "\n Total time: " << timer.reading_str() << "\n";
  }

  bool test_hfs = false;
  if (test_hfs) {
    std::cout << "\nHyperfine:\n";

    double muN = Nuclear::find_mu(wf.Znuc(), wf.Anuc());
    double IN = Nuclear::find_spin(wf.Znuc(), wf.Anuc());
    auto r_rms_fm = Nuclear::find_rrms(wf.Znuc(), wf.Anuc());
    // 4.1989
    auto r_rms = r_rms_fm / PhysConst::aB_fm;
    std::cout << "mu=" << muN << ", I=" << IN << " ,r=" << r_rms_fm << "\n";

    std::cout << "Gridpoints below Rrms: " << wf.rgrid.getIndex(r_rms) << "\n";

    // example for using lambda
    auto l1 = [](double r, double) { return 1. / (r * r); };
    // auto l1 = [](double r, double rN) { return r > rN ? 1. / (r * r) : 0.;
    // };
    HyperfineOperator vhfs(muN, IN, r_rms, wf.rgrid, l1);
    for (auto phi : wf.valence_orbitals) {
      auto A_tmp = phi * (vhfs * phi);
      double j = phi.j();
      auto factor = PhysConst::Hartree_MHz * phi.k / (j * (j + 1.));
      std::cout << phi.symbol() << ": ";
      std::cout << A_tmp * factor << "\n";
    }
  }

  return 0;
}
