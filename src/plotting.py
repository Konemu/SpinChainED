from matplotlib import pyplot as plt
import numpy as np
from matplotlib.ticker import MaxNLocator

plt.rcParams['text.usetex'] = True
plt.rc('axes', labelsize=16)
plt.rc('axes', titlesize=20)

J_ratios = ["0_100000", "1_000000", "2_000000", "5_000000"]
Ts = ["0_200000", "0_500000", "1_000000"]

"""
# Dispersion
for J_ratio in J_ratios:
    for N in np.linspace(8, 12, 2):
        fig, ax = plt.subplots()
        path = "/home/mmaschke/BA_Code/Data/Dispersion/DispN"+str(int(N))+"J"+J_ratio+".txt"
        file = open(path)
        lines = file.readlines()
        ms = []
        ks = []
        ergs = []
        for line in lines:
            data = line.split(" ")
            ms.append( float( data[0]) )
            ks.append( float( data[1]) )
            ergs.append( float( data[2].replace("\n", "") ) )
        lab = "$N$ = "+ str( int(N) )
        sc = ax.scatter(ks, ergs, s=400, c=ms, cmap="hsv", marker="_", linewidths=0.5)
        ax.legend()
        J_ratioNum = J_ratio.replace("_", ".")
        ax.set(xlabel="Momentum quantum number $k$", ylabel="State Energy $E$ ($J_2$)", title="$J_1/J_2 =\\,$"+J_ratioNum+", $N= $ "+str(N))
        ax.xaxis.set_major_locator(MaxNLocator(integer=True))
        fig.colorbar(sc, cmap="hsv", values=ms, label="Magnetization $m$")
        plt.show()

# Susceptibility (J)
for T in Ts:
    fig, ax = plt.subplots()
    
    for N in np.linspace(6, 12, 4):
        path = "/home/mmaschke/BA_Code/Data/SusceptibilitiesForJ/SuscN"+str(int(N))+"T"+T+".txt"
        file = open(path)
        Js = []
        chi = []
        for i in range(200) :
            stri = file.readline()
            data = stri.split(" ")
            Js.append( float( data[0]) )
            chi.append( float( data[1].replace("\n", "") ) )
        lab = "$N$ = "+ str( int(N) )
        ax.plot(Js, chi, label=lab)
    ax.legend()
    T = T.replace("_", ".")
    ax.set(xlabel="$J_1/J_2$", ylabel="Susceptibility per Spin $\\chi / N$ ", title="$T =\\,$"+T)
    plt.show()

#Specific Heat (J)
for T in Ts:
    fig, ax = plt.subplots()
    
    for N in np.linspace(6, 14, 5):
        path = "/home/mmaschke/BA_Code/Data/SpecificHeatsForJ/SpecHeatN"+str(int(N))+"T"+T+".txt"
        file = open(path)
        Js = []
        specHeat = []
        for i in range(200) :
            stri = file.readline()
            data = stri.split(" ")
            Js.append( float( data[0]) )
            specHeat.append( float( data[1].replace("\n", "") ) )
        lab = "$N$ = "+ str( int(N) )
        ax.plot(Js, specHeat, label=lab)
    ax.legend()
    T = T.replace("_", ".")
    ax.set(xlabel="$J_1/J_2$", ylabel="Specific heat per Spin $C/N$", title="$T =\\,$"+T)
    plt.show()

# Specific Heat (T)
for J_ratio in J_ratios:
    fig, ax = plt.subplots()
    
    for N in np.linspace(6, 16, 6):
        path = "/home/mmaschke/BA_Code/Data/SpecificHeats/SpecHeatN"+str(int(N))+"J"+J_ratio+".txt"
        file = open(path)
        Ts = []
        specHeat = []
        for i in range(200) :
            stri = file.readline()
            data = stri.split(" ")
            Ts.append( float( data[0]) )
            specHeat.append( float( data[1].replace("\n", "") ) )
        lab = "$N$ = "+ str( int(N) )
        ax.plot(Ts, specHeat, label=lab)
    ax.legend()
    J_ratio = J_ratio.replace("_", ".")
    ax.set(xlabel="$T$ ($J_2$)", ylabel="Specific heat per Spin $C/N$", title="$J_1/J_2 =\\,$"+J_ratio)
    plt.show()

# Susceptibility (T)
for J_ratio in J_ratios:
    fig, ax = plt.subplots()

    for N in np.linspace(6, 14, 5):
        path = "/home/mmaschke/BA_Code/Data/Susceptibilities/SuscN" + str(int(N)) + "J" + J_ratio + ".txt"
        file = open(path)
        Ts = []
        susc = []
        for i in range(200):
            stri = file.readline()
            data = stri.split(" ")
            if data[0] == "-nan" or data[0] == "nan":
                continue
            Ts.append(float(data[0]) * 8.5)
            susc.append(float(data[1].replace("\n", "")))
        lab = "$N$ = " + str(int(N))
        ax.plot(Ts, susc, label=lab)
    ax.legend()
    J_ratio = J_ratio.replace("_", ".")
    ax.set(xlabel="$T$ ($J_2$)", ylabel="Susceptibility per Spin $\\chi / N$", title="$J_1/J_2 =\\,$" + J_ratio)
    plt.show()

# Excitation Erg
fig, ax = plt.subplots()
for N in np.linspace(6, 18, 7):
    path = "/home/mmaschke/BA_Code/Data/ExcitationErgs/ExcErgs"+str(int(N))+".txt"
    file = open(path)
    Ts = []
    excErg = []
    for i in range(200) :
        stri = file.readline()
        data = stri.split(" ")
        Ts.append( float( data[0]) )
        excErg.append( float( data[1].replace("\n", "") ) )
    lab = "$N$ = "+ str( int(N) )
    ax.plot(Ts, excErg, label=lab)
    ax.legend()
    ax.set(xlabel="$J_1/J_2$", ylabel="1st Excitation Energy $\\Delta E$ ($J_2$)")
plt.show()

# Ground state erg
fig, ax = plt.subplots()
for N in np.linspace(6, 16, 6):
    path = "/home/mmaschke/BA_Code/Data/GroundStateErgs/GSErgs"+str(int(N))+".txt"
    file = open(path)
    Ts = []
    excErg = []
    for i in range(200) :
        stri = file.readline()
        data = stri.split(" ")
        Ts.append( float( data[0]) )
        excErg.append( float( data[1].replace("\n", "") ) )
    lab = "$N$ = "+ str( int(N) )
    ax.plot(Ts, excErg, label=lab)
    ax.legend()
    ax.set(xlabel="$J_1/J_2$", ylabel="Ground state energy per spin $E_0 / N$ ($J_2$)")
plt.show()
"""
