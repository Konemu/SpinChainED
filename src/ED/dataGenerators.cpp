/**
 * This cpp file contains functions used for generating data of interest (excitation energies, specific heat and
 * susceptibilities) for later plotting in Python.
*/

#include "dataGenerators.h"

#include <utility>

using std::complex;
using std::list;
using std::pow;
using std::vector;

using Eigen::MatrixXcd;
using Eigen::MatrixXd;
using Eigen::Dynamic;

// Saves excitation energy (i.e. energy difference between ground and 1st excited state) vor varying values of J1/J2.
// Note: If ground state is degenerate, zero is returned. Some re-sorting is needed because the omp for threads might
// finish out of order.
void saveExcitationErgsForVaryingJ(int N, int dataPointNum, double start, double end, const std::string & path) {
    Eigen::VectorXd J_ratios = Eigen::VectorXd::LinSpaced(dataPointNum, start, end);
    vector<double> diffs;
    vector<double> reSortedJs;
    vector<std::pair<double, double>> out;
    std::mutex m;
#pragma omp parallel for default(none) shared(diffs, J_ratios, reSortedJs, N, m)
    for (int i = 0; i < J_ratios.size(); i++) {
        if (N % 4 == 0 && N >= 8) {
            //list<list<list<MatrixXd>>> H = spinInversionHamiltonian(J_ratios[i], N, 0, N);
            vector<double> erg = getEnergies_memorySaving_threaded_inversion(J_ratios[i], N);
            std::lock_guard<std::mutex> lock(m);
            writeThreadSafe(diffs, {abs(erg[0]-erg[1])});
            writeThreadSafe(reSortedJs, {J_ratios[i]} );
        } else if (N % 2 == 0 && N >= 6) {
            list<list<MatrixXcd>> H = momentumHamiltonian(J_ratios[i], N, 0, N);
            vector<double> erg = getEnergiesFromBlocks(H, true);
            std::lock_guard<std::mutex> lock(m);
            writeThreadSafe(diffs, {abs(erg[0]-erg[1])});
            writeThreadSafe(reSortedJs, {J_ratios[i]} );
        }
    }
    for (int i = 0; i < diffs.size(); i++) {
        out.emplace_back( std::pair<double, double>(reSortedJs[i], diffs[i]) );
    }
    std::sort(out.begin(), out.end());
    list<std::pair<double, double>> out_l(out.begin(), out.end());
    savePairsToFile(out_l, path);
}

void saveSpinGapForVaryingJ(int N, int dataPointNum, double start, double end, const std::string & path) {
    Eigen::VectorXd J_ratios = Eigen::VectorXd::LinSpaced(dataPointNum, start, end);
    vector<std::tuple<double, double, double, double>> diffs;
    vector<double> reSortedJs;
    vector<std::tuple<double, double, double, double, double>> out;
    std::mutex m;
//#pragma omp parallel for default(none) shared(diffs, J_ratios, reSortedJs, N, m)
    for (int i = 0; i < J_ratios.size(); i++) {
        /*if (N % 4 == 0 && N >= 8) {
            list<list<list<MatrixXd>>> H_m0 = spinInversionHamiltonian(J_ratios[i], N, N/2, N/2);
            vector<double> erg_m0 = getEnergiesFromBlocks(H_m0, true);
            list<list<list<MatrixXd>>> H_m1 = spinInversionHamiltonian(J_ratios[i], N, N/2 + 1, N/2 + 1);
            vector<double> erg_m1 = getEnergiesFromBlocks(H_m1, true);
            std::lock_guard<std::mutex> lock(m);
            double val = abs(erg_m0[0] - erg_m1[0]);
            writeThreadSafe(diffs, {val});
            writeThreadSafe(reSortedJs, {J_ratios[i]} );
        } else*/ if (N % 2 == 0 && N >= 6) {
            list<list<MatrixXcd>> H_m0 = momentumHamiltonian(J_ratios[i], N, N/2, N/2);
            std::tuple<double, double, double> e_k_m0 = findLowestErgAndK_momentum(H_m0, N, N/2);
            list<list<MatrixXcd>> H_m1 = momentumHamiltonian(J_ratios[i], N, N/2 + 1, N/2 + 1);
            std::tuple<double, double, double> e_k_m1 = findLowestErgAndK_momentum(H_m1, N, N/2 + 1);
            std::lock_guard<std::mutex> lock(m);
            double val = abs(std::get<0>(e_k_m0) - std::get<0>(e_k_m1));
            writeThreadSafe(diffs, {std::tuple<double, double, double, double>(val, std::get<1>(e_k_m0), std::get<1>(e_k_m1), std::get<2>(e_k_m1))} );
            writeThreadSafe(reSortedJs, {J_ratios[i]} );
        }
    }
    for (int i = 0; i < diffs.size(); i++) {
        out.emplace_back( std::tuple<double, double, double, double, double>(reSortedJs[i], std::get<0>(diffs[i]), std::get<1>(diffs[i]), std::get<2>(diffs[i]), std::get<3>(diffs[i]) ));
    }
    std::sort(out.begin(), out.end());
    list<std::tuple<double, double, double, double, double>> out_l(out.begin(), out.end());
    saveQuintupleToFile(out_l, path);
}

void saveGroundStateErgPerSpinForVaryingJ(int N, int dataPointNum, double start, double end, const std::string & path) {
    Eigen::VectorXd J_ratios = Eigen::VectorXd::LinSpaced(dataPointNum, start, end);
    vector<double> gStateErgs;
    vector<double> reSortedJs;
    vector<std::pair<double, double>> out;
    std::mutex m;
#pragma omp parallel for default(none) shared(gStateErgs, J_ratios, reSortedJs, N, m) //commented out to save memory, compute overnight
    for (int i = 0; i < J_ratios.size(); i++) {
        if (N % 4 == 0 && N >= 8) {
            list<list<list<MatrixXd>>> H = spinInversionHamiltonian(J_ratios[i], N, 0, N);
            vector<double> erg = getEnergiesFromBlocks(H, true);
            std::lock_guard<std::mutex> lock(m);
            writeThreadSafe(gStateErgs, {erg[0] / N});
            writeThreadSafe(reSortedJs, {J_ratios[i]} );
        } else if (N % 2 == 0 && N >= 6) {
            list<list<MatrixXcd>> H = momentumHamiltonian(J_ratios[i], N, 0, N);
            vector<double> erg = getEnergiesFromBlocks(H, true);
            std::lock_guard<std::mutex> lock(m);
            writeThreadSafe(gStateErgs, {erg[0] / N});
            writeThreadSafe(reSortedJs, {J_ratios[i]} );
        }
    }
    for (int i = 0; i < gStateErgs.size(); i++) {
        out.emplace_back( std::pair<double, double>(reSortedJs[i], gStateErgs[i]) );
    }
    std::sort(out.begin(), out.end());
    list<std::pair<double, double>> out_l(out.begin(), out.end());
    savePairsToFile(out_l, path);
}

// Saves specific heat at given temperature/beta for varying values of J1/J2.
void saveSpecificHeatsForVaryingJ(int N, int dataPointNum, double betaOrT, double start, double end, bool isBeta, std::string path) {
    Eigen::VectorXd J_ratios = Eigen::VectorXd::LinSpaced(dataPointNum, start, end);
    vector<double> C;
    vector<double> reSortedJs;
    std::mutex m;
#pragma omp parallel for default(none) shared(C, J_ratios, reSortedJs, N, m, betaOrT, isBeta)
    for (int i = 0; i < J_ratios.size(); i++) {
        double J_ratio = J_ratios[i];
        if (N % 4 == 0 && N >= 8) {
            list<list<list<MatrixXd>>> H = spinInversionHamiltonian(J_ratio, N, 0, N);
            vector<double> erg = getParityErgsThreaded(H, N, true);
            double specH = specificHeat(erg, betaOrT, isBeta) / N;
            std::lock_guard<std::mutex> lock(m);
            writeThreadSafe(C, {specH});
            writeThreadSafe(reSortedJs, {J_ratio} );
        } else if (N % 2 == 0 && N >= 6) {
            list<list<MatrixXcd>> H = momentumHamiltonian(J_ratio, N, 0, N);
            vector<double> erg = getMomentumErgsThreaded(H, N, true);
            double specH = specificHeat(erg, betaOrT, isBeta) / N;
            std::lock_guard<std::mutex> lock(m);
            writeThreadSafe(C, {specH});
            writeThreadSafe(reSortedJs, {J_ratio} );
        }
    }
    vector<std::pair<double, double>> out;
    for (int i = 0; i < C.size(); i++) {
        out.emplace_back( std::pair<double, double>(reSortedJs[i], C[i]) );
    }
    std::sort(out.begin(), out.end());
    list<std::pair<double, double>> out_l(out.begin(), out.end());
    savePairsToFile(out_l, path);
}

// Saves specific heat for a given value of J1/J2 for varying temperature/beta.
void saveSpecificHeatsForVaryingTemp(int N, int dataPointNum, double J_ratio, double start, double end,
                                     bool isBeta, std::string path, bool bench) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    Eigen::VectorXd Ts = Eigen::VectorXd::LinSpaced(dataPointNum, start, end);
    vector<double> C;

    if (N % 4 == 0 && N >= 8) {
        list<list<list<MatrixXd>>> H = spinInversionHamiltonian(J_ratio, N, 0, N);
        vector<double> erg = getParityErgsThreaded(H, N, true);
        for (int i = 0; i < Ts.size(); i++) {
            C.emplace_back(specificHeat(erg, Ts[i], isBeta) / N);
        }
        if (bench) {
            std::chrono::steady_clock::time_point finish = std::chrono::steady_clock::now();
            std::ifstream statFile("/proc/self/stat");
            std::string statLine;
            std::getline(statFile, statLine);
            std::istringstream iss(statLine);
            std::string entry;
            long long memUsage;
            for (int i = 1; i <= 24; i++) {
                std::getline(iss, entry, ' ');
                if (i == 24) {
                    memUsage = stoi(entry);
                }
            }
            long time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - begin).count();
            std::cout << "N = " << N << ", time = " << time_ns/1e9 << " s, ram = " << 4096*memUsage/1e9 << " GB" << std::endl;
        }
    } else if (N % 2 == 0 && N >= 6) {
        list<list<MatrixXcd>> H = momentumHamiltonian(J_ratio, N, 0, N);
        vector<double> erg = getMomentumErgsThreaded(H, N, true);
        for (int i = 0; i < Ts.size(); i++) {
            C.emplace_back(specificHeat(erg, Ts[i], isBeta) / N);
        }
        if (bench) {
            std::chrono::steady_clock::time_point finish = std::chrono::steady_clock::now();
            std::ifstream statFile("/proc/self/stat");
            std::string statLine;
            std::getline(statFile, statLine);
            std::istringstream iss(statLine);
            std::string entry;
            long long memUsage;
            for (int i = 1; i <= 24; i++) {
                std::getline(iss, entry, ' ');
                if (i == 24) {
                    memUsage = stoi(entry);
                }
            }
            long time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - begin).count();
            std::cout << "N = " << N << ", time = " << time_ns/1e9 << " s, ram = " << 4096*memUsage/1e9 << " GB" << std::endl;
        }
    }

    list<std::pair<double, double>> out;
    for (int j = 0; j < dataPointNum; j++) {
        out.emplace_back( std::pair<double, double>(Ts[j], C[j]) );
    }
    savePairsToFile(out, path);
}

// Saves susceptibilities at a given temperature/beta for varying values of J1/J2.
void saveSusceptibilitiesForVaryingJ(int N, int dataPointNum, double betaOrT, double start, double end,
                                     bool isBeta, std::string path) {
    Eigen::VectorXd J_ratios = Eigen::VectorXd::LinSpaced(dataPointNum, start, end);
    vector<int> states = getStates_m(N, N/2);
    MatrixXd S_2 = spinOperator_sq(states, N);

    vector<double> susceptibilities;
    for (double J_ratio : J_ratios) {
        /*if (N % 4 == 0 && N >= 8) {
            list<list<list<MatrixXd>>> H_m0 = spinInversionHamiltonian(J_ratio,  N, N/2, N/2);
            vector<double> ergs;
            MatrixXd U = buildTransformMatrix_parity(H_m0, ergs);

            MatrixXcd T = U.adjoint() * S_2 * U;

            susceptibilities.emplace_back( susceptibility(ergs, betaOrT, isBeta, T) / (double) N );
        } else */if (N % 2 == 0 && N >= 6) {
            list<MatrixXcd> S_2_l = spinOpS2_momentum_m(N, N/2);
            MatrixXcd S_2 = blkdiag(S_2_l, fact(N) / (fact(N/2) * (fact(N/2))));

            list<list<MatrixXcd>> H_m0 = momentumHamiltonian(J_ratio, N, N/2, N/2);
            vector<double> ergs;
            MatrixXcd U = buildTransformMatrix_momentum(H_m0, ergs);

            MatrixXcd T = U.adjoint() * S_2 * U;

            susceptibilities.emplace_back(susceptibility(ergs, betaOrT, isBeta, T) / N );
        }
    }
    list<std::pair<double, double>> out;
    for (int i = 0; i < susceptibilities.size(); i++) {
        out.emplace_back( std::pair<double, double>(J_ratios[i], susceptibilities[i]) );
    }
    savePairsToFile(out, path);
}

// Saves susceptibilities for a given value of J1/J2 for varying temperature/beta.
void saveSusceptibilitesForVaryingTemp(int N, int dataPointNum, double J_ratio, double start, double end,
                                       bool isBeta, std::string path) {
    Eigen::VectorXd Ts = Eigen::VectorXd::LinSpaced(dataPointNum, start, end);
    vector<double> susceptibilities(dataPointNum);

    /*if (N % 4 == 0 && N >= 8) {
        list<list<MatrixXd>> S_2_ll = spinOpS2_spinInv_m0(N);
        list<MatrixXd> S_2_l = blkdiag(S_2_ll);
        MatrixXd S_2 = blkdiag(S_2_l, fact(N) / (fact(N/2) * (fact(N/2))));

        list<list<list<MatrixXd>>> H_m0 = spinInversionHamiltonian(J_ratio, N, N/2, N/2);
        vector<double> ergs;
        MatrixXd U = buildTransformMatrix_parity(H_m0, ergs);

        MatrixXcd T = U.adjoint() * S_2 * U;

        for (int i = 0; i < dataPointNum; i++) {
            susceptibilities[i] =  susceptibility(ergs, Ts[i], isBeta, T) / N ;
        }
    } else */if (N % 2 == 0 && N >= 6) {
        list<MatrixXcd> S_2_l = spinOpS2_momentum_m(N, N/2);
        long siz = fact(N) / (fact(N/2) * fact(N/2));
        MatrixXcd S_2 = blkdiag(S_2_l, siz); //TODO doing this stuff block-wise will save a lot of ram. this can't run for N = 14 on 16G!

        list<list<MatrixXcd>> H_m0 = momentumHamiltonian(J_ratio, N, N/2, N/2);
        vector<double> ergs;
        MatrixXcd U = buildTransformMatrix_momentum(H_m0, ergs);

        MatrixXcd T = U.adjoint() * S_2 * U;

        for (int i = 0; i < dataPointNum; i++) {
            susceptibilities[i] =  susceptibility(ergs, Ts[i], isBeta, T) / N ;
        }
    }

    list<std::pair<double, double>> out;
    for (int j = 0; j < dataPointNum; j++) {
        out.emplace_back( std::pair<double, double>(Ts[j], susceptibilities[j]) );
    }
    savePairsToFile(out, path);
}

// Saves tuples of (E, k) to file.
void saveEnergyDispersion(int N, double J_ratio, std::string path) {
    list<list<MatrixXcd>> H_list = momentumHamiltonian(J_ratio, N, 0, N);
    vector<vector<vector<double>>> ergs = getEnergiesFromBlocksByK(H_list);
    list<std::pair<int, double>> out;
    for (vector<vector<double>> m_block : ergs) {
        int k = -trunc((N+2)/4) + 1;
        for (vector<double> k_block : m_block) {
            for (double erg : k_block) {
                out.emplace_back( std::pair<int, double>(k, erg) );
            }
            k++;
        }
    }
    savePairsToFile(out, path);
}

// Saves tuples of (E, m, k) to file.
void saveEnergyDispersionWithMag(int N, double J_ratio, std::string path) {
    list<list<MatrixXcd>> H_list = momentumHamiltonian(J_ratio, N, 0, N);
    vector<vector<vector<double>>> ergs = getEnergiesFromBlocksByK(H_list);
    list<std::tuple<int, int, double>> out;
    int m = -N/2.0;
    for (const vector<vector<double>> & m_block : ergs) {
        int k = -trunc((N+2)/4) + 1;
        for (const vector<double> & k_block : m_block) {
            for (double erg : k_block) {
                out.emplace_back( std::tuple<int, int, double>(m, k, erg) );
            }
            k++;
        }
        m++;
    }
    saveTripleToFile(out, path);
}

// Used to write data tuples to a file at path.
//TODO templatisieren, das ist sehr kacke
template <typename T, typename U>
void savePairsToFile(const list<std::pair<T, U>> & pairList, std::string path) {
    std::ofstream File;
    File.open(path);
    for (std::pair<T, U> p : pairList) {
        File << p.first << " " << p.second << "\n";
    }
    File.close();
}

template <typename T, typename U, typename V, typename W>
void saveQuadrupleToFile(const list<std::tuple<T, U, V, W>> & pairList, std::string path) {
    std::ofstream File;
    File.open(path);
    for (std::tuple<T, U, V, W> p : pairList) {
        File << std::get<0>(p) << " " << std::get<1>(p) << " " << std::get<2>(p) << " " << std::get<3>(p) << "\n";
    }
    File.close();
}

template <typename T, typename U, typename V, typename W, typename X>
void saveQuintupleToFile(const list<std::tuple<T, U, V, W, X>> & pairList, std::string path) {
    std::ofstream File;
    File.open(path);
    for (std::tuple<T, U, V, W, X> p : pairList) {
        File << std::get<0>(p) << " " << std::get<1>(p) << " " << std::get<2>(p) << " " << std::get<3>(p) << " " << std::get<4>(p) << "\n";
    }
    File.close();
}

// Enables threaded writing to a vector.
template <typename T>
void writeThreadSafe (vector<vector<T>> & writeTo, const vector<T> & writeFrom) {
    static std::mutex mu;
    std::lock_guard<std::mutex> lock(mu);
    writeTo.emplace_back(writeFrom);
}

// Enables threaded writing to a vector.
template <typename T>
void writeThreadSafe (vector<T> & writeTo, const vector<T> & writeFrom) {
    static std::mutex mu;
    std::lock_guard<std::mutex> lock(mu);
    for (T d : writeFrom) {
        writeTo.emplace_back(d);
    }
}

MatrixXd buildTransformMatrix_parity(const list<list<list<MatrixXd>>> & H_m0, vector<double> & ergs) {
    int totalSize = 0;
    list<list<MatrixXd>> EV_blocks;
    for (const list<list<MatrixXd>> & l : H_m0) { // if this ever has more than one element, kaboom
        for (const list<MatrixXd> & m : l) {
            list<MatrixXd> EV_subBlocks;
            for (const MatrixXd & mat : m) {
                totalSize += mat.cols();

                Eigen::SelfAdjointEigenSolver<MatrixXd> sol(mat);
                Eigen::VectorXd blockErgs = sol.eigenvalues().real();
                vector<double> ergs_stl(blockErgs.begin(), blockErgs.end());
                writeThreadSafe(ergs, ergs_stl);

                MatrixXd U_block = sol.eigenvectors();
                EV_subBlocks.emplace_back(U_block);
            }
            EV_blocks.emplace_back(EV_subBlocks);
        }
    }

    list<MatrixXd> temp = blkdiag(EV_blocks);
    MatrixXd ret = blkdiag(temp, totalSize);

    return ret;
}

MatrixXcd buildTransformMatrix_momentum(const list<list<MatrixXcd>> & H_m0, vector<double> & ergs) {
    int totalSize = 0;
    list<MatrixXcd> EV_blocks;

    for (const list<MatrixXcd> & l : H_m0) { // must only contain one entry!
        for (const MatrixXcd & mat : l) {
            if (mat.cols() == 0) {
                continue;
            }

            totalSize += mat.cols();

            Eigen::SelfAdjointEigenSolver<MatrixXcd> sol(mat);
            Eigen::VectorXd blockErgs = sol.eigenvalues().real();
            vector<double> ergs_stl(blockErgs.begin(), blockErgs.end());
            writeThreadSafe(ergs, ergs_stl);

            MatrixXcd U_block = sol.eigenvectors();
            EV_blocks.emplace_back(U_block);
        }
    }


    MatrixXcd ret = blkdiag(EV_blocks, totalSize);

    return ret;
}

vector<double> readDoubleVectorFromFile(const std::string & path) {
    vector<double> vals;
    std::ifstream file;
    file.open(path);
    std::string line;
    while(getline(file, line)) {
        vals.emplace_back( stod(line) );
    }

    return vals;
}


std::tuple<double, double, double> findLowestErgAndK_momentum(const list<list<MatrixXcd>> & H, int N, int n_up) {
    vector<std::tuple<double, double, double>> ergs_w_k_S;
    list<MatrixXcd> S2_list_m = spinOpS2_momentum_m(N, n_up);
    vector<MatrixXcd> S2_vec_m = vector<MatrixXcd>(S2_list_m.begin(), S2_list_m.end());

    for (const list<MatrixXcd> & subList : H) { // should only contain one element
        int k = -trunc((N+2)/4);
        int i = 0;
        for (const MatrixXcd & mat : subList) {
            Eigen::SelfAdjointEigenSolver<MatrixXcd> sol;
            if (mat.cols() == 0) {
                continue;
            }
            sol.compute(mat);
            Eigen::VectorXcd blockEnergies = sol.eigenvalues();
            MatrixXcd U = sol.eigenvectors();
            MatrixXcd S2_transform = U.adjoint() * S2_vec_m[i] * U;
            for (int i = 0; i < blockEnergies.size(); i++) {
                double S = -0.5 + sqrt(0.25 + S2_transform(i, i).real());
                ergs_w_k_S.emplace_back( std::tuple<double, double, double>(blockEnergies(i).real(), k, S ) );
            }
            k++;
            i++;
        }
    }
    std::sort(ergs_w_k_S.begin(), ergs_w_k_S.end());
    return ergs_w_k_S[0];
}

// Saves partition function for a given value of J1/J2 for varying beta.
void savePartitionFunction(int N, int dataPointNum, double J_ratio, double start, double end, std::string path) {
    Eigen::VectorXd Ts = Eigen::VectorXd::LinSpaced(dataPointNum, start, end);
    vector<double> Z;

    if (N % 4 == 0 && N >= 8) {
        list<list<list<MatrixXd>>> H = spinInversionHamiltonian(J_ratio, N, 0, N);
        vector<double> erg = getParityErgsThreaded(H, N, true);
        /*double e_0 = erg[0];
        for (double & e : erg) {
            e -= e_0;
        }*/
        for (int i = 0; i < Ts.size(); i++) {
            Z.emplace_back(partitionFunction(erg, Ts[i], true) / N);
        }
    } else if (N % 2 == 0 && N >= 6) {
        list<list<MatrixXcd>> H = momentumHamiltonian(J_ratio, N, 0, N);
        vector<double> erg = getMomentumErgsThreaded(H, N, true);
        /*double e_0 = erg[0];
        for (double & e : erg) {
            e -= e_0;
        }*/
        for (int i = 0; i < Ts.size(); i++) {
            Z.emplace_back(partitionFunction(erg, Ts[i], true) / N);
        }
    }

    list<std::pair<double, double>> out;
    for (int j = 0; j < dataPointNum; j++) {
        out.emplace_back( std::pair<double, double>(Ts[j], Z[j]) );
    }
    savePairsToFile(out, path);
}