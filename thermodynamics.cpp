//
// Created by mmaschke on 03/05/22.
//

#include "thermodynamics.h"

using std::vector;

double partitionFunction(const vector<double> & ergs, double betaOrT, bool isBeta) {
    double Z = 0;
    double temp_representative;
    if (isBeta) {
        temp_representative = betaOrT;
    } else {
        temp_representative = 1/betaOrT;
    }
    for (double erg : ergs) {
        Z += std::exp( -temp_representative * erg );
    }
    return Z;
}

double specificHeat(const vector<double> & ergs, double betaOrT, bool isBeta) {
    double Z = partitionFunction(ergs, betaOrT, isBeta);

    double temp_representative;
    if (isBeta) {
        temp_representative = betaOrT;
    } else {
        temp_representative = 1/betaOrT;
    }

    double avg_H_sq = 0;
    double avg_H = 0;
    for (double erg : ergs) {
        avg_H_sq += std::exp(-temp_representative * erg) * pow(erg, 2);
        avg_H    += std::exp(-temp_representative * erg) * erg;
    }
    avg_H_sq *= 1/Z;
    avg_H    *= 1/Z;
    return pow(temp_representative,2) * ( avg_H_sq - pow(avg_H, 2) );
}

double susceptibilityOld(const vector<double> & ergs, double betaOrT, bool isBeta, const Eigen::MatrixXcd & U,const Eigen::MatrixXd & S_2) {
    double S_2_avg = 0;
    Eigen::MatrixXcd S_2_transform =  U.adjoint() * S_2 * U;
    double Z = partitionFunction(ergs, betaOrT, isBeta);

    double temp_representative;
    if (isBeta) {
        temp_representative = betaOrT;
    } else {
        temp_representative = 1/betaOrT;
    }

    for (int i = 0; i < S_2.cols(); i++) {
        S_2_avg += std::exp(-temp_representative * ergs[i]) * S_2_transform(i, i).real();
    }

    double S_2_avg_real = S_2_avg/(3.0*Z);
    return temp_representative * S_2_avg_real;
}

double susceptibility(const vector<double> & ergs, double betaOrT, bool isBeta, const Eigen::MatrixXcd & U,const Eigen::MatrixXd & S_2) {
    double S_2_avg = 0;
    double Z = 0;
    Eigen::MatrixXcd S_2_transform =  U.adjoint() * S_2 * U;

    double temp_representative;
    if (isBeta) {
        temp_representative = betaOrT;
    } else {
        temp_representative = 1/betaOrT;
    }

    for (int i = 0; i < S_2.cols(); i++) {
        double S = -0.5 + sqrt(0.25 + S_2_transform(i, i).real());
        Z += std::exp( -temp_representative * ergs[i] ) * (2 * S + 1);
        S_2_avg += std::exp(-temp_representative * ergs[i]) * S * (S + 1) * (2 * S + 1);
    }

    S_2_avg = S_2_avg/(3.0*Z);
    return temp_representative * S_2_avg;
}


