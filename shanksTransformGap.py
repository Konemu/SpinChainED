import scipy.interpolate
from matplotlib import pyplot as plt
import numpy as np
import scipy.optimize as opt
from matplotlib.ticker import MaxNLocator
import os
import natsort
import copy

plt.rcParams['text.usetex'] = True
plt.rc('axes', labelsize=24)
plt.rc('axes', titlesize=30)
plt.rcParams["figure.figsize"] = (12, 9)


def shanks(a0, a1, a2):
    return a2 - (a2 - a1) ** 2 / ((a2 - a1) - (a1 - a0))


def epsilon(a):
    N = len(a)
    eps = np.zeros((N, N+1))
    for l in range(N):
        eps[l][1] = a[l]
    for k in range(N+2):
        if k < 2:
            continue
        for l in range(N):
            if l >= N-(k-1):
                continue
            eps[l][k] = eps[l + 1][k - 2] + 1 / (eps[l + 1][k - 1] - eps[l][k - 1])
    if N % 2 == 0:
        return eps[0][N-1]
    else:
        return eps[0][N]


def euler(a1, a2):
    return ()


def weird_transform(Js, Vals):
    A = copy.deepcopy(Vals)

    for y in range(len(Js)):
        A[y] *= 1 / (1 + Js[y])
    return A


def lin(x, a, b):
    return a*x + b


nMin = 6
nMax = 24
nNum = 10  # int((nMax - nMin) / 2) + 1
nNumLow = 5
dataPointNum = 20
ED = False

fullGaps = []
fullErrs = []
gapsLow = []
for N in [6, 8, 10, 12, 14, 16, 18, 20, 22, 24]:
    path1 = "/home/mmaschke/BA_Code/Data/out/GapFit/spin/gapsIt20lowJ" + str(int(N)) + ".txt"
    file1 = open(path1, "r")
    lines1 = file1.readlines()
    fullJs = []
    JsLow = []
    gapsNLow = []
    fullGapsN = []
    fullErrsN = []
    for line1 in lines1:
        data1 = line1.split(" ")
        fullJs.append(float(data1[0]))
        fullGapsN.append(float(data1[1].replace("\n", "")))
        if ED:
            fullErrsN.append(0)
        else:
            fullErrsN.append(float(data1[2].replace("\n", "")))
        if float(data1[0]) < 0.65:
            JsLow.append(float(data1[0]))
            gapsNLow.append(float(data1[1].replace("\n", "")))
    gapsLow.append(gapsNLow)
    fullGaps.append(fullGapsN)
    fullErrs.append(fullErrsN)

gapsHigh = []
gapErrsHigh = []
for N in [6, 10, 14, 18, 22]:
    path2 = "/home/mmaschke/BA_Code/Data/out/GapFit/spin/gapsIt20lowJ" + str(int(N)) + ".txt"
    #if N == 22:
    #    path2 = "/home/mmaschke/BA_Code/Data/out/GapFit/spin/gapsIt20lowJ" + str(int(N)) + ".txt"
    file2 = open(path2, "r")
    lines2 = file2.readlines()
    JsHigh = []
    gapsNHigh = []
    gapErrsNHigh = []
    for line2 in lines2:
        data2 = line2.split(" ")
        if float(data2[0]) >= 0.65:
            data2 = line2.split(" ")
            JsHigh.append(float(data2[0]))
            gapsNHigh.append(float(data2[1].replace("\n", "")))
        if ED:
            gapErrsNHigh.append(0)
        else:
            gapErrsNHigh.append(float(data1[2].replace("\n", "")))
    gapsHigh.append(gapsNHigh)
    gapErrsHigh.append(gapErrsNHigh)

gaps_extrapLow = []
gaps_extrapHigh = []
for j in range(len(gapsLow[0])):
    gapsNLow = []
    for i in range(nNum):
        gapsNLow.append(gapsLow[i][j])
    gaps_extrapLow.append(epsilon(gapsNLow))
for j in range(len(gapsHigh[0])):
    gapsNHigh = []
    for i in range(nNum):
        if i < nNumLow:
            gapsNHigh.append(gapsHigh[i][j])
    gaps_extrapHigh.append(epsilon(gapsNHigh))

fig, ax = plt.subplots()
N = 6
for gap in fullGaps:
    ax.plot(fullJs, weird_transform(fullJs, gap), label="ED, $N=$" + str(N))
    N += 2
ax.plot(JsLow, weird_transform(JsLow, gaps_extrapLow), "r--", label="ED Extrapolation Low")
ax.plot(JsHigh, weird_transform(JsHigh, gaps_extrapHigh), "g--", label="ED Extrapolation High")
ax.set(xlabel="$J_1/J_2$", ylabel="Reduced Spin Gap Energy $\\Delta/(J_1+J_2)$", title="ED Data")
ax.set_ylim(0, 0.8)
ax.set_xlim(0, 2)
ax.legend()
#plt.show()
plt.close(fig)


offsets = []
offsetErrs = []
gaps = gapsLow #+ gapsHigh
Ns = np.linspace(nMin, nMax, nNum)
NsHigh = [6, 10, 14, 18, 22]
RecipNsPlot = np.linspace(0.001, 0.5, 200)
for j in range(dataPointNum):
    fig, ax = plt.subplots()
    jGaps = []
    jErrs = []
    for gapErr in zip(fullGaps, fullErrs):
        jGaps.append(gapErr[0][j])
        jErrs.append(gapErr[1][j])
    jGapsHigh = []
    jErrsHigh = []
    for gapErr in zip(gapsHigh, gapErrsHigh):
        jGapsHigh.append(gapErr[0][j-len(gapsLow[0])])
        jErrsHigh.append(gapErr[1][j])
    if ED:
        if fullJs[j] >= 0.65:
            parameters, covariance = scipy.optimize.curve_fit(lin, 1/np.asarray(NsHigh), jGapsHigh)
        else:
            parameters, covariance = scipy.optimize.curve_fit(lin, 1/np.asarray(Ns), jGaps)
    else:
        if fullJs[j] >= 0.65:
            parameters, covariance = scipy.optimize.curve_fit(lin, 1/np.asarray(NsHigh), jGapsHigh, sigma=jErrsHigh, absolute_sigma=False)
        else:
            parameters, covariance = scipy.optimize.curve_fit(lin, 1/np.asarray(Ns), jGaps, sigma=jErrs, absolute_sigma=False)
    offsets.append(parameters[1])
    offsetErrs.append(np.sqrt(covariance[1][1]))
    ax.errorbar(1/np.asarray(Ns), np.asarray(jGaps)/(1+fullJs[j]), fmt=".-", xerr=None, yerr=np.asarray(jErrs)/(1+fullJs[j]), capsize=2)
    ax.plot(RecipNsPlot, np.asarray(lin(RecipNsPlot, parameters[0], parameters[1]))/(1+fullJs[j]), "--")
    ax.set(xlabel="$1/N$", ylabel="Reduced Spin Gap Energy $\\Delta/(J_1+J_2)$", title="QT Fit, $J_1/J_2=$" + str(fullJs[j]))
    ax.set_ylim(0, 0.8)
    ax.set_xlim(0, 0.5)
    #ax.semilogy()
    #ax.set_xlim(0, 2)
    fig.savefig("/home/mmaschke/BA_Code/Data/plots/GapFit/spin/Extrap/Single_pointsQT/J" + str(fullJs[j]).replace(".", "_") + ".png")
    #plt.show()
    plt.close(fig)

fig, ax = plt.subplots()
N = 6
for gapErr in zip(fullGaps, fullErrs):
    if ED:
        ax.plot(fullJs, weird_transform(fullJs, gapErr[0]), label="QT, $N=$" + str(N))
    else:
        ax.errorbar(fullJs, weird_transform(fullJs, gapErr[0]), yerr=weird_transform(fullJs, gapErr[1]), xerr=None, label="QT, $N=$" + str(N),
                    fmt=".-", capsize=2)
    N += 2
ax.plot(fullJs, weird_transform(fullJs, offsets), "r.--", label="QT Fit-Extrapolation")
ax.fill_between(fullJs,  weird_transform(fullJs, np.asarray(offsets) - np.asarray(offsetErrs)),  weird_transform(fullJs, np.asarray(offsets) + np.asarray(offsetErrs)), color="r", alpha=0.1)
ax.set(xlabel="$J_1/J_2$", ylabel="Reduced Spin Gap Energy $\\Delta/(J_1+J_2)$", title="QT Data, $n=20$")
ax.set_ylim(-0.05, 0.75)
ax.set_xlim(0, 1.25)
ax.legend()
plt.show()
