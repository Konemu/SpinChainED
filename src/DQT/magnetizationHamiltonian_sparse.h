#ifndef SPINCHAINED_MAGNETIZATIONHAMILTONIAN_SPARSE_H
#define SPINCHAINED_MAGNETIZATIONHAMILTONIAN_SPARSE_H

#include <vector>
#include <list>

#include <Eigen/Sparse>

#include "../ED/helperFunctions.h"
#include "../ED/magnetizationHamiltonian.h"

Eigen::SparseMatrix<double> magnetizationHamiltonian_sparse_full(double J_ratio, int N);

std::vector<Eigen::SparseMatrix<double>> magnetizationHamiltonian_sparse(double J_ratio, int N);

Eigen::SparseMatrix<double> spinOp2_magnetization_sparse_full(int N);

std::vector<Eigen::SparseMatrix<double>> spinOp2_magnetization_sparse(int N);

#endif //SPINCHAINED_MAGNETIZATIONHAMILTONIAN_SPARSE_H
