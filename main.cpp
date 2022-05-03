#include "main.h"
#include <chrono>
#include <utility>

using std::vector;
using std::complex;
using std::list;
using std::pow;

using Eigen::MatrixXcd;
using Eigen::MatrixXd;
using Eigen::Dynamic;

//#define saveErgs
//#define saveExcitationErgs
#define saveSpecificHeat

int main(int argc, char* argv[]) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#ifdef saveErgs
    int N; double j_ratio; std::string path, method;
    if (argc > 4) {
        N = atoi(argv[1]);
        j_ratio = atof(argv[2]);
        path =argv[3];
        method = argv[4];
    } else {
        N = 10;
        j_ratio = 1.0;
        path = "ergs.txt";
        method = "momentum";
    }

    if (method == "naive") {
        MatrixXd H = naiveHamiltonian(j_ratio, N);
        Eigen::VectorXd erg = H.eigenvalues().real();
        std::sort(erg.begin(), erg.end());
        //saveEnergies(erg , path);
    } else if (method == "magnetization") {
        list<MatrixXd> H = magnetizationHamiltonian(j_ratio, N);
        vector<double> erg = getEnergiesFromBlocks(H, N);
        saveEnergies(erg, path);
    } else {
        list<list<MatrixXcd>> H = momentumHamiltonian(j_ratio, N);
        vector<double> erg = getEnergiesFromBlocks(H, N);
        saveEnergies(erg, path);
    }

#endif
#ifdef saveExcitationErgs
    Eigen::VectorXd J_ratios = Eigen::VectorXd::LinSpaced(1000, 0, 1.5);
    int N = 10;
    vector<double> diffs;
    for (double J_ratio : J_ratios) {
        list<list<MatrixXcd>> H = momentumHamiltonian(J_ratio, N);
        vector<double> erg = getEnergiesFromBlocks(H, N);
        diffs.emplace_back( abs(erg[0]-erg[1]) );
    }
    list<std::pair<double, double>> out;
    for (int i = 0; i < diffs.size(); i++) {
        out.emplace_back( std::pair<double, double>(J_ratios[i], diffs[i]) );
    }
    std::ofstream ergFile;
    ergFile.open("excitationEnergies.txt");
    for (std::pair<double, double> p : out) {
        ergFile << p.first << " " << p.second << "\n";
    }
    ergFile.close();

#endif
#ifdef saveSpecificHeat
    int dataPointNum = 500;
    Eigen::VectorXd betas = Eigen::VectorXd::LinSpaced(dataPointNum, 0, 2.5);
    double J_ratio = 2;
    int N = 10;
    vector<double> C(dataPointNum);
    int i = 0;
    list<list<MatrixXcd>> H = momentumHamiltonian(J_ratio, N);
    vector<double> erg = getEnergiesFromBlocks(H, N);
    for (double beta : betas) {
        C[i] = specificHeat(erg, beta)/N ;
        i++;
    }
    list<std::pair<double, double>> out;
    for (int j = 0; j < dataPointNum; j++) {
        out.emplace_back( std::pair<double, double>(betas[j], C[j]) );
    }
    std::ofstream ergFile;
    ergFile.open("/home/mmaschke/BA_Code/Data/specHeatJ1BetaVar.txt");
    for (std::pair<double, double> p : out) {
        ergFile << p.first << " " << p.second << "\n";
    }
    ergFile.close();
#endif
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count()
              << "[ms]" << std::endl;
    return 0;
}

