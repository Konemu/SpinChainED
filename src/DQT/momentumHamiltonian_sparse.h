#ifndef SPINCHAINED_MOMENTUMHAMILTONIAN_SPARSE_H
#define SPINCHAINED_MOMENTUMHAMILTONIAN_SPARSE_H

#include <vector>
#include <list>
#include <complex>

#include <Eigen/Sparse>
#include <Eigen/Dense>

#include "../ED/helperFunctions.h"
#include "../ED/momentumHamiltonian.h"

Eigen::SparseMatrix<std::complex<double>> momentumHamiltonian_sparse(double J_ratio, int N);


#endif //SPINCHAINED_MOMENTUMHAMILTONIAN_SPARSE_H