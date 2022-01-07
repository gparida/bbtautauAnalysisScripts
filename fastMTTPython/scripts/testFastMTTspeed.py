#Andrew Loeliger#script for testing the fast mtt tool on existing files
#to figure out how fast a speed can be managed on existing files

from bbtautauAnalysisScripts.fastMTTPython.fastMTTtool import *
import ROOT
import math

#let's test this on the TTToSemiLeptonic mt sample

theFile = ROOT.TFile('/data/aloeliger/bbtautauAnalysis/2016/channelSelected/mt_Channel/TTToSemiLeptonic.root')
theTree = theFile.Events

print(theTree.GetEntries())

theFastMTTtool = fastMTTtool()

for i in range(theTree.GetEntries()):
    theTree.GetEntry(i)
    
    theMET = fastMTTmet(
        measuredX = theTree.MET_pt * math.cos(theTree.MET_phi),
        measuredY = theTree.MET_pt * math.sin(theTree.MET_phi),
        xx = theTree.MET_covXX,
        xy = theTree.MET_covXY,
        yy = theTree.MET_covYY
    )
    
    firstLepton = fastMTTlepton(
        pt = theTree.gMuon_pt[0],
        eta = theTree.gMuon_eta[0],
        phi = theTree.gMuon_phi[0],
        m = theTree.gMuon_mass[0],
        leptonType = 'Muon'
    )

    #print("tau mass: {}".format(theTree.allTau_mass[0]))

    secondLepton = fastMTTlepton(
        pt = theTree.allTau_pt[0],
        eta = theTree.allTau_eta[0],
        phi = theTree.allTau_phi[0],
        #m = theTree.allTau_mass[0],
        m = 0.13957, #this seems to be enforced?. I should check with Cecile...
        tauDecayMode = theTree.allTau_decayMode[0],
        leptonType = 'Tau'
    )

    theFastMTTtool.setTheMET(theMET)
    theFastMTTtool.setFirstLepton(firstLepton)
    theFastMTTtool.setSecondLepton(secondLepton)

    print theFastMTTtool.getFastMTTmass()
