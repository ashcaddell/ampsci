# Dependencies for DiracODE

$(BD)/Adams_bound.o: $(SD)/DiracODE/Adams_bound.cpp $(SD)/DiracODE/Adams_bound.hpp \
$(SD)/DiracODE/Adams_coefs.hpp $(SD)/Wavefunction/DiracSpinor.hpp \
$(SD)/DiracODE/DiracODE.hpp $(SD)/Maths/Grid.hpp $(SD)/Maths/LinAlg_MatrixVector.hpp \
$(SD)/Maths/NumCalc_quadIntegrate.hpp $(SD)/IO/SafeProfiler.hpp
	$(COMP)

$(BD)/Adams_continuum.o: $(SD)/DiracODE/Adams_continuum.cpp \
$(SD)/DiracODE/Adams_continuum.hpp $(SD)/DiracODE/Adams_bound.hpp \
$(SD)/DiracODE/DiracODE.hpp $(SD)/Maths/Grid.hpp $(SD)/DiracODE/Adams_coefs.hpp
	$(COMP)

$(BD)/Adams_Greens.o: $(SD)/DiracODE/Adams_Greens.cpp \
$(SD)/DiracODE/Adams_Greens.hpp $(SD)/DiracODE/Adams_bound.hpp \
$(SD)/Wavefunction/DiracSpinor.hpp $(SD)/DiracODE/DiracODE.hpp $(SD)/Maths/Grid.hpp \
$(SD)/Maths/NumCalc_quadIntegrate.hpp $(SD)/DiracODE/Adams_coefs.hpp \
$(SD)/IO/SafeProfiler.hpp
	$(COMP)
