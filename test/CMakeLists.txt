set(LINKED_LIBS fmt::fmt solvers shared_lib)

new_test(solvers_test solvers.cpp ${LINKED_LIBS})
