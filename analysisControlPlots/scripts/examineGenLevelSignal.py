#!/usr/bin/env python
#script for examining the plots 

import ROOT
import argparse
import json
import re
from tqdm import tqdm
import math

def main(args):
    ROOT.gStyle.SetOptStat(0)
    ROOT.gROOT.SetBatch(ROOT.kTRUE)

    with open (args.inputJson) as jsonFile:
        samplesJson = json.load(jsonFile)

    for sampleKey in samplesJson:
        if not re.search('M'+args.mass, sampleKey):
            continue

        if re.search('GenTT', sampleKey):
            ttFile = ROOT.TFile(samplesJson[sampleKey]['file'])
            ttTree = ttFile.Events
        elif re.search('GenMT', sampleKey):
            mtFile = ROOT.TFile(samplesJson[sampleKey]['file'])
            mtTree = mtFile.Events
        elif re.search('GenET', sampleKey):
            etFile = ROOT.TFile(samplesJson[sampleKey]['file'])
            etTree = etFile.Events
        elif re.search('GenEM', sampleKey):
            emFile = ROOT.TFile(samplesJson[sampleKey]['file'])
            emTree = emFile.Events
    
    #we need an output file, let's make that
    outputFile = ROOT.TFile('genLevelDiagnostics.root','RECREATE')
    ttDir = outputFile.mkdir('ttPlots')
    mtDir = outputFile.mkdir('mtPlots')
    etDir = outputFile.mkdir('etPlots')
    emDir = outputFile.mkdir('emPlots')
    #okay. We have trees, and a place to put them what do we want to know?

    #first, let's make a plot just showing the overall normalized number of events in each.

    print('Normalized event plots...')
    outputFile.cd()
    normalizedEventPlots = ROOT.TH1F('normalizedEvents','normalizedEvents',4,1.0,5.0)
    listOfTrees = [ttTree,mtTree,etTree,emTree]
    for treeIndex in tqdm(range(len(listOfTrees)), desc='trees'):
        theTree = listOfTrees[treeIndex]
        normalizedEvents = 0.0
        for eventIndex in tqdm(range(theTree.GetEntries()), desc='Events', leave=False):
            theTree.GetEntry(eventIndex)
            normalizedEvents += theTree.FinalWeighting
        normalizedEventPlots.SetBinContent(treeIndex+1, normalizedEvents)
    normalizedEventPlots.Draw()
    normalizedEventPlots.GetXaxis().SetBinLabel(1,'#tau#tau channel events')
    normalizedEventPlots.GetXaxis().SetBinLabel(2,'#mu#tau channel events')
    normalizedEventPlots.GetXaxis().SetBinLabel(3,'e#tau channel events')
    normalizedEventPlots.GetXaxis().SetBinLabel(4,'e#mu channel events')
    normalizedEventPlots.Write()
        

    #The first thing we should go through and check is just what kind of object multiplicity plots we have
    print('Object multiplicity plots...')
    #the simple unselected object plots
    ttDir.cd()
    ttBoostedTauMultiplicityName = 'ttBoostedTauMultiplicity'
    ttTauMultiplicityName = 'ttTauMultiplicity'
    ttElectronMultiplicityName = 'ttElectronMultiplicity'
    ttMuonMultiplicityName = 'ttMuonMultiplicity'

    ttTree.Draw('nboostedTau>>%s' %ttBoostedTauMultiplicityName,'FinalWeighting')
    ttBoostedTauMultiplicity = ROOT.gDirectory.Get(ttBoostedTauMultiplicityName)
    ttBoostedTauMultiplicity.Write()

    ttTree.Draw('nTau>>%s'%ttTauMultiplicityName, 'FinalWeighting')
    ttTauMultiplicity = ROOT.gDirectory.Get(ttTauMultiplicityName)
    ttTauMultiplicity.Write()

    ttTree.Draw('nMuon>>%s'%ttMuonMultiplicityName, 'FinalWeighting')
    ttMuonMultiplicity = ROOT.gDirectory.Get(ttMuonMultiplicityName)
    ttMuonMultiplicity.Write()

    ttTree.Draw('nElectron>>%s'%ttElectronMultiplicityName, 'FinalWeighting')
    ttElectronMultiplicity = ROOT.gDirectory.Get(ttElectronMultiplicityName)
    ttElectronMultiplicity.Write()

    ttTree.Draw('nTau:nboostedTau>>ttTauBoostedTauMultiplicity', 'FinalWeighting')
    ttTauBoostedTauMultiplicity = ROOT.gDirectory.Get("ttTauBoostedTauMultiplicity")
    ttTauBoostedTauMultiplicity.Write()

    tauBoostedTauMultiplicityCanvas = ROOT.TCanvas("tauBoostedTauMultiplicityCanvas","tauBoostedTauMultiplicityCanvas")
    ROOT.gStyle.SetPaintTextFormat("4.1f")
    ttTauBoostedTauMultiplicity.Draw("COLZ TEXT") 
    ttTauBoostedTauMultiplicity.GetXaxis().SetTitle("nboostedTau")
    ttTauBoostedTauMultiplicity.GetYaxis().SetTitle("nTau")
    tauBoostedTauMultiplicityCanvas.Write()

    for nTaus in [0,1,2,3]:
        for nboostedTaus in [0,1,2,3]:
            muonPlotName = '%s_%iBoostedTaus_%iTaus'%(ttMuonMultiplicityName,nboostedTaus,nTaus)
            ttTree.Draw('nMuon>>%s'%muonPlotName,
                        "FinalWeighting * ( (nTau == %i) && (nboostedTau == %i) )"%(nTaus,nboostedTaus))
            theMuonPlot = ROOT.gDirectory.Get(muonPlotName)
            theMuonPlot.Write()

            electronPlotName = '%s_%iBoostedTaus_%iTaus'%(ttElectronMultiplicityName,nboostedTaus,nTaus)
            ttTree.Draw('nElectron>>%s'%electronPlotName,
                        'FinalWeighting * ( (nTau==%i) && (nboostedTau == %i) )'%(nTaus,nboostedTaus))
            theElectronPlot = ROOT.gDirectory.Get(electronPlotName)
            theElectronPlot.Write()

    #Okay, we need to now understand whether or not it is our tau selection that is destroying all of our efficiency
    #First, Let's figure out how many leading objects pass our selection of a loose tau ID.

    print('boostedTau selection plots...')
    # we want a plot displaying the following thing
    #1.)How many events do we have where we get a boosted tau
    #2.)How many events do we have where we get a boosted tau that passes our ID requirement
    #3.) The above, plus a second boosted tau that passes our ID requirement
    #4.) the above, plus no other taus, boosted or non boosted that pass our ID requirement in the event
    #5.) 2.) + a standard tau that passes our ID requirement in the event
    #6.) the above plus no other taus boosted or non boosted that pass our ID requirement in the event
    #7.) How many events do we have where we get a tau
    #8.) How many events do we have where get a tau that passes our ID requirement
    #9.) the above, plus a second tau that passes our ID requirement
    #10.) the above plus no other taus or boosted taus that pass any requirements in the event

    # we will also want to compare this to an et and mt plot!
    ttDir.cd()
    boostedTauSelectionCanvas = ROOT.TCanvas('boostedTauCanvas','boostedTauCanvas')
    boostedTauSelectionCanvas.SetBottomMargin(0.4)
    tauIDSelectionBits = 7 #7 == loose MVA ID
    boostedTauSelectionPlot = ROOT.TH1F('boostedTauSelectionPlot','boostedTauSelectionPlot',10, 1.0, 11.0)

    tooManyBoostedTaus = 0
    tooManyTaus = 0
    for i in tqdm(range(ttTree.GetEntries()), desc = 'Tau selection: events:'):
        ttTree.GetEntry(i)
        nboostedTau = ttTree.nboostedTau
        nTau = ttTree.nTau

        boostedTausPassID = []
        tausPassID = []

        for boostedTauIndex in range(nboostedTau):
            boostedTauID = int(''.join(format(ord(i), '016b') for i in ttTree.boostedTau_idMVAnewDM2017v2[boostedTauIndex]) ,2)
            if (boostedTauID >= tauIDSelectionBits):
                boostedTausPassID.append(boostedTauIndex)
            
        for tauIndex in range(nTau):
            tauID = int(''.join(format(ord(i), '016b') for i in ttTree.Tau_idMVAnewDM2017v2[tauIndex]) , 2)
            if (tauID >= tauIDSelectionBits):
                tausPassID.append(tauIndex)

        if(nboostedTau > 0):
            boostedTauSelectionPlot.Fill(1.0, ttTree.FinalWeighting)
            if (len(boostedTausPassID) >= 1):
                boostedTauSelectionPlot.Fill(2.0, ttTree.FinalWeighting)
                if (len(boostedTausPassID) >= 2):
                    boostedTauSelectionPlot.Fill(3.0, ttTree.FinalWeighting)
                    if(len(boostedTausPassID) == 2 and len(tausPassID) == 0):
                        boostedTauSelectionPlot.Fill(4.0, ttTree.FinalWeighting)
                if (len(tausPassID) >= 1):
                    boostedTauSelectionPlot.Fill(5.0, ttTree.FinalWeighting)
                    if (len(boostedTausPassID) == 1 and len(tausPassID) == 1):
                        boostedTauSelectionPlot.Fill(6.0, ttTree.FinalWeighting)
        if(nTau > 0):
            boostedTauSelectionPlot.Fill(7.0, ttTree.FinalWeighting)
            if(len(tausPassID) >= 1):
                boostedTauSelectionPlot.Fill(8.0, ttTree.FinalWeighting)
                if (len(tausPassID) >= 2):
                    boostedTauSelectionPlot.Fill(9.0, ttTree.FinalWeighting)
                    if(len(tausPassID) == 2 and len(boostedTausPassID) == 0):
                        boostedTauSelectionPlot.Fill(10.0, ttTree.FinalWeighting)
    #label it
    boostedTauSelectionPlot.Draw()
    boostedTauSelectionPlot.SetMaximum(1.1*boostedTauSelectionPlot.GetMaximum())
    boostedTauSelectionPlot.Draw()
    boostedTauSelectionPlot.GetXaxis().SetBinLabel(1, "Events with a boosted tau")
    boostedTauSelectionPlot.GetXaxis().SetBinLabel(2, "Events with a boosted tau that passes ID")
    boostedTauSelectionPlot.GetXaxis().SetBinLabel(3, "Events with two boosted taus that pass ID")                
    boostedTauSelectionPlot.GetXaxis().SetBinLabel(4, "Events with two boosted taus that pass ID, and no other taus (HPS or boosted) that pass ID")        
    boostedTauSelectionPlot.GetXaxis().SetBinLabel(5, "Events with a boosted tau that passes ID and a standard tau that passes ID")
    boostedTauSelectionPlot.GetXaxis().SetBinLabel(6, "Events with a boosted tau that passes ID, a standard that passes ID, and no other taus that pass ID")
    boostedTauSelectionPlot.GetXaxis().SetBinLabel(7, "Events with an HPS tau")
    boostedTauSelectionPlot.GetXaxis().SetBinLabel(8, "Events with an HPS tau that passes ID")
    boostedTauSelectionPlot.GetXaxis().SetBinLabel(9, "Events with two HPS taus that pass ID")
    boostedTauSelectionPlot.GetXaxis().SetBinLabel(10, "Events with two HPS taus that pass ID, and no other tau objects that pass ID")
    #tlines will also help keep it straight
    firstSection = ROOT.TLine(3.0, 0.0, 3.0, boostedTauSelectionPlot.GetMaximum())
    firstSection.SetLineColor(ROOT.kBlack)
    firstSection.Draw()
    
    secondSection = ROOT.TLine(5.0, 0.0, 5.0, boostedTauSelectionPlot.GetMaximum())
    secondSection.SetLineColor(ROOT.kBlack)
    secondSection.Draw()

    thirdSection = ROOT.TLine(7.0, 0.0, 7.0, boostedTauSelectionPlot.GetMaximum())
    thirdSection.SetLineColor(ROOT.kBlack)
    thirdSection.Draw()

    #let's go through and label the bins too for convenience
    binLabelLatex = ROOT.TLatex()
    binLabelLatex.SetTextSize(0.025)
    binLabelLatex.SetTextAlign(22) # center over the bin
    for i in range (1,11): #current histogram binning range
        binLabelLatex.DrawLatex(i+0.5, (boostedTauSelectionPlot.GetBinContent(i) + 1) * 1.05, str(int(boostedTauSelectionPlot.GetBinContent(i))))

    boostedTauSelectionCanvas.Write()
    
    
    print ('Electron rejecton plots...')
    #okay, let's look at how good we are at rejecting electrons
    ttDir.cd()
    electronRejectionCanvas = ROOT.TCanvas('electronRejectionCanvas','electronRejectionCanvas')
    electronRejectionCanvasIdentifiedTaus = ROOT.TCanvas('electronRejectionCanvasIdentifiedtaus', 'electronRejectionCanvasIdentifiedTaus')
    electronRejectionCanvasChannelStyle = ROOT.TCanvas('electronRejectionCanvasChannelStyle','electronRejectionCanvasChannelStyle')
    
    #3 histograms
    # one for n electrons no ID
    # same for current ID
    # next for WP 80
    # next for WP 90

    #Would also be good to see this for at least 1 identified tau in the event
    #and for 2 identified boosted taus, 1 boosted and 1 regular identified tau, and 2 regular identified taus in the event
    nElectronsNoID = ROOT.TH1F('nElectronsNoID','nElectronsNoID', 5, 0.0, 5.0)
    nElectronsLooseMVA = ROOT.TH1F('nElectronsLooseMVA','nElectronsLooseMVA', 5, 0.0, 5.0)
    nElectronsWP80 = ROOT.TH1F('nElectronsWP80','nElectronsWP80', 5, 0.0, 5.0)
    nElectronsWP90 = ROOT.TH1F('nElectronsWP90', 'nElectronsWP90', 5, 0.0, 5.0)

    nElectronsNoIDIdentifiedTaus = ROOT.TH1F('nElectronsNoIDIdentifiedTaus', 'nElectronsNoIDIdentifiedTaus', 5, 0.0, 5.0)
    nElectronsLooseMVAIdentifiedTaus = ROOT.TH1F('nElectronsLooseMVAIdentifiedTaus','nElectronsLooseMVAIdentifiedTaus', 5, 0.0, 5.0)
    nElectronsWP80IdentifiedTaus = ROOT.TH1F('nElectronsWP80IdentifiedTaus', 'nElectronsWP80IdentifiedTaus', 5, 0.0, 5.0)
    nElectronsWP90IdentifiedTaus = ROOT.TH1F('nElectronsWP90IdentifiedTaus','nElectronsWP90IdentifiedTaus', 5, 0.0, 5.0)

    nElectronsNoIDChannelStyle = ROOT.TH1F('nElectronsNoIDChannelStyle', 'nElectronsNoIDChannelStyle', 5, 0.0, 5.0)
    nElectronsLooseMVAChannelStyle = ROOT.TH1F('nElectronsLooseMVAChannelStyle','nElectronsLooseMVAChannelStyle', 5, 0.0, 5.0)
    nElectronsWP80ChannelStyle = ROOT.TH1F('nElectronsWP80ChannelStyle', 'nElectronsWP80ChannelStyle', 5, 0.0, 5.0)
    nElectronsWP90ChannelStyle = ROOT.TH1F('nElectronsWP90ChannelStyle', 'nElectronsWP90ChannelStyle', 5, 0.0, 5.0)
    
    for entryIndex in tqdm(range(ttTree.GetEntries()), desc = 'Events'):
        ttTree.GetEntry(entryIndex)
        boostedTausPassingID = []
        tausPassingID = []
        #first, let's just scroll the taus and figure out if anything passes any ID
        for tauIndex in range(ttTree.nTau):
            if ttTree.Tau_idMVAnewDM2017v2[tauIndex] >= tauIDSelectionBits:
                tausPassingID.append(tauIndex)
        for boostedTauIndex in range(ttTree.nboostedTau):
            if ttTree.boostedTau_idMVAnewDM2017v2[boostedTauIndex] >= tauIDSelectionBits:
                boostedTausPassingID.append(boostedTauIndex)
        #okay, now we scroll the electrons and start filling the whatever
        nElectronsNoID.Fill(ttTree.nElectron, ttTree.FinalWeighting)
        electronsPassingLooseMVA = 0
        electronsPassingWP80 = 0
        electronsPassingWP90 = 0

        electronsPassingLooseMVAIdentifiedTaus = 0
        electronsPassingWP80IdentifiedTaus = 0
        electronsPassingWP90IdentifiedTaus = 0

        electronsPassingLooseMVAChannelStyle = 0
        electronsPassingWP80ChannelStyle = 0
        electronsPassingWP90ChannelStyle = 0

        for electronIndex in range(ttTree.nElectron):
            if (ttTree.Electron_mvaFall17V2Iso_WPL[electronIndex]):
                electronsPassingLooseMVA += 1
            if (ttTree.Electron_mvaFall17V2Iso_WP80[electronIndex]):
                electronsPassingWP80 += 1
            if (ttTree.Electron_mvaFall17V2Iso_WP90[electronIndex]):
                electronsPassingWP90 += 1
        nElectronsLooseMVA.Fill(electronsPassingLooseMVA, ttTree.FinalWeighting)
        nElectronsWP80.Fill(electronsPassingWP80, ttTree.FinalWeighting)
        nElectronsWP90.Fill(electronsPassingWP90, ttTree.FinalWeighting)

        #same thing, but for events containing at least one good tau
        if (len(boostedTausPassingID) > 0 or len(tausPassingID) > 0):
            for electronIndex in range(ttTree.nElectron):
                nElectronsNoIDIdentifiedTaus.Fill(ttTree.nElectron,ttTree.FinalWeighting)
                if (ttTree.Electron_mvaFall17V2Iso_WPL[electronIndex]):
                    electronsPassingLooseMVAIdentifiedTaus += 1
                if (ttTree.Electron_mvaFall17V2Iso_WP80[electronIndex]):
                    electronsPassingWP80IdentifiedTaus += 1
                if (ttTree.Electron_mvaFall17V2Iso_WP90[electronIndex]):
                    electronsPassingWP90IdentifiedTaus += 1
            nElectronsLooseMVAIdentifiedTaus.Fill(electronsPassingLooseMVAIdentifiedTaus, ttTree.FinalWeighting)
            nElectronsWP80IdentifiedTaus.Fill(electronsPassingWP80IdentifiedTaus, ttTree.FinalWeighting)
            nElectronsWP90IdentifiedTaus.Fill(electronsPassingWP90IdentifiedTaus, ttTree.FinalWeighting)
            
        #same thing, but for configurations that look similar to our final channel selection
        if ((len(boostedTausPassID) == 2) 
            or (len(boostedTausPassID) == 1 and len(tausPassingID) == 1) 
            or (len(tausPassingID) == 2) ):
            for electronIndex in range(ttTree.nElectron):
                nElectronsNoIDChannelStyle.Fill(ttTree.nElectron,ttTree.FinalWeighting)
                if (ttTree.Electron_mvaFall17V2Iso_WPL[electronIndex]):
                    electronsPassingLooseMVAChannelStyle += 1
                if (ttTree.Electron_mvaFall17V2Iso_WP80[electronIndex]):
                    electronsPassingWP80ChannelStyle += 1
                if (ttTree.Electron_mvaFall17V2Iso_WP90[electronIndex]):
                    electronsPassingWP90ChannelStyle += 1
            nElectronsLooseMVAChannelStyle.Fill(electronsPassingLooseMVA, ttTree.FinalWeighting)
            nElectronsWP80ChannelStyle.Fill(electronsPassingWP80, ttTree.FinalWeighting)
            nElectronsWP90ChannelStyle.Fill(electronsPassingWP90, ttTree.FinalWeighting)
    #okay, now we do the styling on the electron rejection plot
    nElectronsNoID.SetLineColor(ROOT.kBlack)
    nElectronsNoIDIdentifiedTaus.SetLineColor(ROOT.kBlack)
    nElectronsNoIDChannelStyle.SetLineColor(ROOT.kBlack)
    
    nElectronsLooseMVA.SetLineColor(ROOT.kBlue)
    nElectronsLooseMVAIdentifiedTaus.SetLineColor(ROOT.kBlue)
    nElectronsLooseMVAChannelStyle.SetLineColor(ROOT.kBlue)
    
    nElectronsWP80.SetLineColor(ROOT.kRed)
    nElectronsWP80IdentifiedTaus.SetLineColor(ROOT.kRed)
    nElectronsWP80ChannelStyle.SetLineColor(ROOT.kRed)
    
    nElectronsWP90.SetLineColor(ROOT.kGreen)
    nElectronsWP90IdentifiedTaus.SetLineColor(ROOT.kGreen)
    nElectronsWP90ChannelStyle.SetLineColor(ROOT.kGreen)

    electronRejectionCanvas.cd()
    nElectronsNoID.SetMaximum(max(nElectronsNoID.GetMaximum(),nElectronsWP90.GetMaximum()))
    nElectronsNoID.Draw()
    nElectronsLooseMVA.Draw('SAME')
    nElectronsWP80.Draw('SAME')
    nElectronsWP90.Draw('SAME')
    #make the legend
    noTauConditionLegend = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    noTauConditionLegend.AddEntry(nElectronsNoID,"No Electron ID Condition", "l")
    noTauConditionLegend.AddEntry(nElectronsLooseMVA, "Passes LooseMVA", "l")
    noTauConditionLegend.AddEntry(nElectronsWP80, "Passes WP80", "l")
    noTauConditionLegend.AddEntry(nElectronsWP90, "Passes WP90", "l")
    noTauConditionLegend.Draw()
    electronRejectionCanvas.Write()

    electronRejectionCanvasIdentifiedTaus.cd()
    nElectronsNoIDIdentifiedTaus.SetMaximum(max(nElectronsNoID.GetMaximum(), nElectronsWP90.GetMaximum()))
    nElectronsNoIDIdentifiedTaus.Draw()
    nElectronsLooseMVAIdentifiedTaus.Draw('SAME')
    nElectronsWP80IdentifiedTaus.Draw('SAME')
    nElectronsWP90IdentifiedTaus.Draw('SAME')
    #make the legend
    IDTauConditionLegend = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    IDTauConditionLegend.AddEntry(nElectronsNoIDIdentifiedTaus, "No Electron ID Condition", "l")
    IDTauConditionLegend.AddEntry(nElectronsLooseMVAIdentifiedTaus, "Passes LooseMVA", "l")
    IDTauConditionLegend.AddEntry(nElectronsWP80IdentifiedTaus, "Passes WP80", "l")
    IDTauConditionLegend.AddEntry(nElectronsWP90IdentifiedTaus, "Pases WP90" ,"l")
    IDTauConditionLegend.Draw()
    electronRejectionCanvasIdentifiedTaus.Write()

    electronRejectionCanvasChannelStyle.cd()
    nElectronsNoIDChannelStyle.SetMaximum(max(nElectronsNoIDChannelStyle.GetMaximum(), nElectronsNoIDChannelStyle.GetMaximum()))
    nElectronsNoIDChannelStyle.Draw()
    nElectronsLooseMVAChannelStyle.Draw('SAME')
    nElectronsWP80ChannelStyle.Draw('SAME')
    nElectronsWP90ChannelStyle.Draw('SAME')
    #make the legend
    channelStyleLegend = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    channelStyleLegend.AddEntry( nElectronsNoIDChannelStyle,"No Electron ID Condition", "l")
    channelStyleLegend.AddEntry(nElectronsLooseMVAChannelStyle,"Passes LooseMVA" , "l")
    channelStyleLegend.AddEntry(nElectronsWP80ChannelStyle, "Passes WP80", "l")
    channelStyleLegend.AddEntry(nElectronsWP90ChannelStyle, "Passes WP90", "l")
    channelStyleLegend.Draw()
    electronRejectionCanvasChannelStyle.Write()
    
    print('Muon rejection plots...')

    #now we do similarly for the muons
    ttDir.cd()
    muonRejectionCanvas = ROOT.TCanvas('muonRejectionCanvas', 'muonRejectionCanvas')
    muonRejectionCanvasIdentifiedTaus = ROOT.TCanvas('muonRejectionCanvasIdentifiedTaus','muonRejectionCanvasIdentifiedTaus')
    muonRejectionCanvasChannelStyle = ROOT.TCanvas('muonRejectionCanvasChannelStyle','muonRejectionCanvasChannelStyle')

    nMuonsNoID = ROOT.TH1F('nMuonsNoID','nMuonsNoID', 5, 0.0, 5.0)
    nMuonsMVALoose = ROOT.TH1F('nMuonsMVALoose','nMuonsMVALoose', 5, 0.0, 5.0)
    nMuonsMVAMedium = ROOT.TH1F('nMuonsMVAMedium', 'nMuonsMVAMedium', 5, 0.0, 5.0)
    nMuonsMVATight = ROOT.TH1F('nMuonsMVATight','nMuonsMVATight', 5, 0.0, 5.0)
    nMuonsMVAVTight = ROOT.TH1F('nMuonsMVAVTight','nMuonsMVAVTight', 5, 0.0, 5.0)
    nMuonsMVAVVTight = ROOT.TH1F('nMuonsMVAVVTight', 'nMuonsMVAVVTight', 5, 0.0, 5.0)
    
    nMuonsNoIDIdentifiedTaus = ROOT.TH1F('nMuonsNoIDIdentifiedTaus','nMuonsNoIDIdentifiedTaus', 5, 0.0, 5.0)
    nMuonsMVALooseIdentifiedTaus = ROOT.TH1F('nMuonsMVALooseIdentifiedTaus','nMuonsMVALooseIdentifiedTaus', 5, 0.0, 5.0)
    nMuonsMVAMediumIdentifiedTaus = ROOT.TH1F('nMuonsMVAMediumIdentifiedTaus', 'nMuonsMVAMediumIdentifiedTaus', 5, 0.0, 5.0)
    nMuonsMVATightIdentifiedTaus = ROOT.TH1F('nMuonsMVATightIdentifiedTaus','nMuonsMVATightIdentifiedTaus', 5, 0.0, 5.0)
    nMuonsMVAVTightIdentifiedTaus = ROOT.TH1F('nMuonsMVAVTightIdentifiedTaus','nMuonsMVAVTightIdentifiedTaus', 5, 0.0, 5.0)
    nMuonsMVAVVTightIdentifiedTaus = ROOT.TH1F('nMuonsMVAVVTightIdentifiedTaus', 'nMuonsMVAVVTightIdentifiedTaus', 5, 0.0, 5.0)

    nMuonsNoIDChannelStyle = ROOT.TH1F('nMuonsNoIDChannelStyle','nMuonsNoIDChannelStyle', 5, 0.0, 5.0)
    nMuonsMVALooseChannelStyle = ROOT.TH1F('nMuonsMVALooseChannelStyle','nMuonsMVALooseChannelStyle', 5, 0.0, 5.0)
    nMuonsMVAMediumChannelStyle = ROOT.TH1F('nMuonsMVAMediumChannelStyle', 'nMuonsMVAMediumChannelStyle', 5, 0.0, 5.0)
    nMuonsMVATightChannelStyle = ROOT.TH1F('nMuonsMVATightChannelStyle','nMuonsMVATightChannelStyle', 5, 0.0, 5.0)
    nMuonsMVAVTightChannelStyle = ROOT.TH1F('nMuonsMVAVTightChannelStyle','nMuonsMVAVTightChannelStyle', 5, 0.0, 5.0)
    nMuonsMVAVVTightChannelStyle = ROOT.TH1F('nMuonsMVAVVTightChannelStyle', 'nMuonsMVAVVTightChannelStyle', 5, 0.0, 5.0)

    for entryIndex in tqdm(range(ttTree.GetEntries()), desc = 'Events'):
        ttTree.GetEntry(entryIndex)
        boostedTausPassingID = []
        tausPassingID = []
        
        #let's do what we did before and scroll the taus to figure out if anything passes ID
        for tauIndex in range(ttTree.nTau):
            if ttTree.Tau_idMVAnewDM2017v2[tauIndex] >= tauIDSelectionBits:
                tausPassingID.append(tauIndex)
        for boostedTauIndex in range(ttTree.nboostedTau):
            if ttTree.boostedTau_idMVAnewDM2017v2[boostedTauIndex] >= tauIDSelectionBits:
                boostedTausPassingID.append(boostedTauIndex)

        #now we fill muon plots
        nMuonsNoID.Fill(ttTree.nMuon, ttTree.FinalWeighting)
        muonsPassingLooseMVA = 0
        muonsPassingMediumMVA = 0
        muonsPassingTightMVA = 0
        muonsPassingVTightMVA = 0
        muonsPassingVVTightMVA = 0

        muonsPassingLooseMVAIdentifiedTaus = 0
        muonsPassingMediumMVAIdentifiedTaus = 0
        muonsPassingTightMVAIdentifiedTaus = 0
        muonsPassingVTightMVAIdentifiedTaus = 0
        muonsPassingVVTightMVAIdentifiedTaus = 0

        muonsPassingLooseMVAChannelStyle = 0
        muonsPassingMediumMVAChannelStyle = 0
        muonsPassingTightMVAChannelStyle = 0
        muonsPassingVTightMVAChannelStyle = 0
        muonsPassingVVTightMVAChannelStyle = 0

        for muonIndex in range(ttTree.nMuon):
            theMuonIDBits = ''.join(format(ord(i), '016b') for i in ttTree.Muon_mvaId[muonIndex])
            #reduce it to a number?
            theMuonID = int(theMuonIDBits,2)
            if theMuonID >= 1:
                muonsPassingLooseMVA += 1
            if theMuonID >= 2:
                muonsPassingMediumMVA += 1
            if theMuonID >= 3:
                muonsPassingTightMVA += 1
            if theMuonID >= 4:
                muonsPassingVTightMVA += 1
            if theMuonID >= 5:
                muonsPassingVVTightMVA += 1
        #print("Muon IDentification multiplicities")
        #print(ttTree.nMuon)
        #print(muonsPassingLooseMVA)
        #print(muonsPassingMediumMVA)
        #print(muonsPassingTightMVA)
        nMuonsMVALoose.Fill(muonsPassingLooseMVA, ttTree.FinalWeighting)
        nMuonsMVAMedium.Fill(muonsPassingMediumMVA, ttTree.FinalWeighting)
        nMuonsMVATight.Fill(muonsPassingTightMVA, ttTree.FinalWeighting)
        nMuonsMVAVTight.Fill(muonsPassingVTightMVA, ttTree.FinalWeighting)
        nMuonsMVAVVTight.Fill(muonsPassingVVTightMVA, ttTree.FinalWeighting)
        
        if (len(boostedTausPassingID) > 0 or len(tausPassingID) > 0):
            nMuonsNoIDIdentifiedTaus.Fill(ttTree.nMuon, ttTree.FinalWeighting)
            for muonIndex in range(ttTree.nMuon):
                theMuonIDBits = ''.join(format(ord(i), '016b')for i in ttTree.Muon_mvaId[muonIndex])
                theMuonID = int(theMuonIDBits, 2)
                if theMuonID >= 1:
                    muonsPassingLooseMVAIdentifiedTaus += 1
                if theMuonID >= 2:
                    muonsPassingMediumMVAIdentifiedTaus += 1
                if theMuonID >= 3:
                    muonsPassingTightMVAIdentifiedTaus += 1
                if theMuonID >= 4:
                    muonsPassingVTightMVAIdentifiedTaus += 1
                if theMuonID >= 5:
                    muonsPassingVVTightMVAIdentifiedTaus += 1
                    nMuonsMVALoose.Fill(muonsPassingLooseMVA, ttTree.FinalWeighting)
            nMuonsMVALooseIdentifiedTaus.Fill(muonsPassingLooseMVAIdentifiedTaus, ttTree.FinalWeighting)
            nMuonsMVAMediumIdentifiedTaus.Fill(muonsPassingMediumMVAIdentifiedTaus, ttTree.FinalWeighting)
            nMuonsMVATightIdentifiedTaus.Fill(muonsPassingTightMVAIdentifiedTaus, ttTree.FinalWeighting)
            nMuonsMVAVTightIdentifiedTaus.Fill(muonsPassingVTightMVAIdentifiedTaus, ttTree.FinalWeighting)
            nMuonsMVAVVTightIdentifiedTaus.Fill(muonsPassingVVTightMVAIdentifiedTaus, ttTree.FinalWeighting)
            
            
        if ((len(boostedTausPassID) == 2) 
            or (len(boostedTausPassID) == 1 and len(tausPassingID) == 1) 
            or (len(tausPassingID) == 2) ):
            nMuonsNoIDChannelStyle.Fill(ttTree.nMuon, ttTree.FinalWeighting)
            for muonIndex in range(ttTree.nMuon):
                theMuonIDBits = ''.join(format(ord(i), '016b')for i in ttTree.Muon_mvaId[muonIndex])
                theMuonID = int(theMuonIDBits, 2)
                if theMuonID >= 1:
                    muonsPassingLooseMVAChannelStyle += 1
                if theMuonID >= 2:
                    muonsPassingMediumMVAChannelStyle += 1
                if theMuonID >= 3:
                    muonsPassingTightMVAChannelStyle += 1
                if theMuonID >= 4:
                    muonsPassingVTightMVAChannelStyle += 1
                if theMuonID >= 5:
                    muonsPassingVVTightMVAChannelStyle += 1
            nMuonsMVALooseChannelStyle.Fill(muonsPassingLooseMVAChannelStyle, ttTree.FinalWeighting)
            nMuonsMVAMediumChannelStyle.Fill(muonsPassingMediumMVAChannelStyle, ttTree.FinalWeighting)
            nMuonsMVATightChannelStyle.Fill(muonsPassingTightMVAChannelStyle, ttTree.FinalWeighting)
            nMuonsMVAVTightChannelStyle.Fill(muonsPassingVTightMVAChannelStyle, ttTree.FinalWeighting)
            nMuonsMVAVVTightChannelStyle.Fill(muonsPassingVVTightMVAChannelStyle, ttTree.FinalWeighting)
    #let's do the style stuff
    nMuonsNoID.SetLineColor(ROOT.kBlack)
    nMuonsNoIDIdentifiedTaus.SetLineColor(ROOT.kBlack)
    nMuonsNoIDChannelStyle.SetLineColor(ROOT.kBlack)
    
    nMuonsMVALoose.SetLineColor(ROOT.kBlue)
    nMuonsMVALooseIdentifiedTaus.SetLineColor(ROOT.kBlue)
    nMuonsMVALooseChannelStyle.SetLineColor(ROOT.kBlue)

    nMuonsMVAMedium.SetLineColor(ROOT.kRed)
    nMuonsMVAMediumIdentifiedTaus.SetLineColor(ROOT.kRed)
    nMuonsMVAMediumChannelStyle.SetLineColor(ROOT.kRed)

    nMuonsMVATight.SetLineColor(ROOT.kGreen)
    nMuonsMVATightIdentifiedTaus.SetLineColor(ROOT.kGreen)
    nMuonsMVATightChannelStyle.SetLineColor(ROOT.kGreen)

    nMuonsMVAVTight.SetLineColor(ROOT.kCyan)
    nMuonsMVAVTightIdentifiedTaus.SetLineColor(ROOT.kCyan)
    nMuonsMVAVTightChannelStyle.SetLineColor(ROOT.kCyan)
    
    nMuonsMVAVVTight.SetLineColor(ROOT.kPink)
    nMuonsMVAVVTightIdentifiedTaus.SetLineColor(ROOT.kPink)
    nMuonsMVAVVTightChannelStyle.SetLineColor(ROOT.kPink)

    muonRejectionCanvas.cd()
    nMuonsNoID.SetMaximum(max(nMuonsNoID.GetMaximum(),
                              max(nMuonsMVALoose, 
                                  max(nMuonsMVAMedium.GetMaximum(), 
                                      max(nMuonsMVATight.GetMaximum(), nMuonsMVAVVTight.GetMaximum())))) * 1.1)
    nMuonsNoID.Draw()
    nMuonsMVALoose.Draw('SAME')
    nMuonsMVAMedium.Draw('SAME')
    nMuonsMVATight.Draw('SAME')
    nMuonsMVAVTight.Draw('SAME')
    nMuonsMVAVVTight.Draw('SAME')
    #make the legend
    noTauConditionLegend_muons = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    noTauConditionLegend_muons.AddEntry(nMuonsNoID, "No muon ID Condition", "l")
    noTauConditionLegend_muons.AddEntry(nMuonsMVALoose, "Passes Loose MVA", "l")
    noTauConditionLegend_muons.AddEntry(nMuonsMVAMedium, "Passes Medium MVA", "l")
    noTauConditionLegend_muons.AddEntry(nMuonsMVATight, "Passes Tight MVA", "l")
    noTauConditionLegend_muons.AddEntry(nMuonsMVAVTight, "Passes VTight MVA", "l")
    noTauConditionLegend_muons.AddEntry(nMuonsMVAVVTight, "Passes VVTight MVA", "l")
    noTauConditionLegend_muons.Draw()
    muonRejectionCanvas.Write()

    muonRejectionCanvasIdentifiedTaus.cd()
    nMuonsNoIDIdentifiedTaus.SetMaximum(max(nMuonsNoIDIdentifiedTaus.GetMaximum(), nMuonsMVAVVTightIdentifiedTaus.GetMaximum()))
    nMuonsNoIDIdentifiedTaus.Draw()
    nMuonsMVALooseIdentifiedTaus.Draw('SAME')
    nMuonsMVAMediumIdentifiedTaus.Draw('SAME')
    nMuonsMVATightIdentifiedTaus.Draw('SAME')
    nMuonsMVAVTightIdentifiedTaus.Draw('SAME')
    nMuonsMVAVVTightIdentifiedTaus.Draw('SAME')
    #make the legend
    identifiedTauLegend_muons = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    identifiedTauLegend_muons.AddEntry(nMuonsNoIDIdentifiedTaus, "No muon ID Condition", "l")
    identifiedTauLegend_muons.AddEntry(nMuonsMVALooseIdentifiedTaus, "Passes Loose MVA", "l")
    identifiedTauLegend_muons.AddEntry(nMuonsMVAMediumIdentifiedTaus, "Passes Medium MVA", "l")
    identifiedTauLegend_muons.AddEntry(nMuonsMVATightIdentifiedTaus, "Passes Tight MVA", "l")
    identifiedTauLegend_muons.AddEntry(nMuonsMVAVTightIdentifiedTaus, "Passes VTight MVA", "l")
    identifiedTauLegend_muons.AddEntry(nMuonsMVAVVTightIdentifiedTaus, "Passes VVTight MVA", "l")
    identifiedTauLegend_muons.Draw()
    muonRejectionCanvasIdentifiedTaus.Write()

    muonRejectionCanvasChannelStyle.cd()
    nMuonsNoIDChannelStyle.SetMaximum(max(nMuonsNoIDChannelStyle.GetMaximum(), nMuonsMVAVVTightChannelStyle.GetMaximum()))
    nMuonsNoIDChannelStyle.Draw()
    nMuonsMVALooseChannelStyle.Draw('SAME')
    nMuonsMVAMediumChannelStyle.Draw('SAME')
    nMuonsMVATightChannelStyle.Draw('SAME')
    nMuonsMVAVTightChannelStyle.Draw('SAME')
    nMuonsMVAVVTightChannelStyle.Draw('SAME')
    #make the legend
    channelStyleLegend_muons = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    channelStyleLegend_muons.AddEntry(nMuonsNoIDChannelStyle, "No muon ID Condition", "l")
    channelStyleLegend_muons.AddEntry(nMuonsMVALooseChannelStyle, "Passes Loose MVA", "l")
    channelStyleLegend_muons.AddEntry(nMuonsMVAMediumChannelStyle, "Passes Medium MVA", "l")
    channelStyleLegend_muons.AddEntry(nMuonsMVATightChannelStyle, "Passes Tight MVA", "l")
    channelStyleLegend_muons.AddEntry(nMuonsMVAVTightChannelStyle, "Passes VTight MVA", "l")
    channelStyleLegend_muons.AddEntry(nMuonsMVAVVTightChannelStyle, "Passes VVTight MVA", "l")
    channelStyleLegend_muons.Draw()
    muonRejectionCanvasChannelStyle.Write()

    print("Extra tau rejection plots")
    ttDir.cd()
    #let's try just understanding what the distances between taus, and boosted taus are
    closestTauPlot = ROOT.TH1F('closestTauPlot','closestTauPlot',50,0.0,0.15)
    closestTauPlotZoomOut = ROOT.TH1F('closestTauPlotZoomOut','closestTauPlotZoomOut',50,0.0,1.5)
    allTauDistancesPlot = ROOT.TH1F('allTauDistancesPlots','allTauDistancesPlots',50,0.0,1.5)
    
    for eventIndex in tqdm(range(ttTree.GetEntries()), desc='Events'):
        ttTree.GetEntry(eventIndex)
        
        for boostedTauIndex in range(ttTree.nboostedTau):
            boostedTauVector = ROOT.TLorentzVector()
            boostedTauVector.SetPtEtaPhiM(ttTree.boostedTau_pt[boostedTauIndex],
                                          ttTree.boostedTau_eta[boostedTauIndex],
                                          ttTree.boostedTau_phi[boostedTauIndex],
                                          ttTree.boostedTau_mass[boostedTauIndex])
            closestDeltaR = 10000.0
            for tauIndex in range(ttTree.nTau):
                tauVector = ROOT.TLorentzVector()
                tauVector.SetPtEtaPhiM(ttTree.Tau_pt[tauIndex],
                                       ttTree.Tau_eta[tauIndex],
                                       ttTree.Tau_phi[tauIndex],
                                       ttTree.Tau_mass[tauIndex])
                tauBoostedTauDeltaR = boostedTauVector.DeltaR(tauVector)
                allTauDistancesPlot.Fill(tauBoostedTauDeltaR, ttTree.FinalWeighting)
                if tauBoostedTauDeltaR  < closestDeltaR:
                    closestDeltaR = tauBoostedTauDeltaR
            closestTauPlot.Fill(closestDeltaR, ttTree.FinalWeighting)
            closestTauPlotZoomOut.Fill(closestDeltaR, ttTree.FinalWeighting)

    # do styling things
    closestTauPlot.Write()
    closestTauPlotZoomOut.Write()
    allTauDistancesPlot.Write()
    
    tauMultiplicityWithHPSVeto = ROOT.TH1F('tauMultiplicityWithHPSVeto','tauMultiplicityWithHPSVeto',5,0.0,5.0)
    tauMultiplicityWithHPSVetoVVLooseMVA = ROOT.TH1F('tauMultiplicityWithHPSVetoVVLooseMVA','tauMultiplicityWithHPSVetoVVLooseMVA',5,0.0,5.0)
    tauMultiplicityWithHPSVetoVLooseMVA = ROOT.TH1F('tauMultiplicityWithHPSVetoVLooseMVA','tauMultiplicityWithHPSVetoVLooseMVA',5,0.0,5.0)
    tauMultiplicityWithHPSVetoLooseMVA = ROOT.TH1F('tauMultiplicityWithHPSVetoLooseMVA','tauMultiplicityWithHPSVetoLooseMVA',5,0.0,5.0)
    tauMultiplicityWithHPSVetoMediumMVA = ROOT.TH1F('tauMultiplicityWithHPSVetoMediumMVA','tauMultiplicityWithHPSVetoMediumMVA',5,0.0,5.0)
    tauMultiplicityWithHPSVetoTightMVA = ROOT.TH1F('tauMultiplicityWithHPSVetoTightMVA','tauMultiplicityWithHPSVetoTightMVA',5,0.0,5.0)
    tauMultiplicityWithHPSVetoVTightMVA = ROOT.TH1F('tauMultiplicityWithHPSVetoVTightMVA','tauMultiplicityWithHPSVetoVTightMVA',5,0.0,5.0)
    tauMultiplicityWithHPSVetoVVTightMVA = ROOT.TH1F('tauMultiplicityWithHPSVetoVVTightMVA','tauMultiplicityWithHPSVetoVVTightMVA',5,0.0,5.0)
    
    tauBoostedTauMultiplicityWithHPSVeto = ROOT.TH2F('tauBoostedTauMultiplicityWithHPSVeto','tauBoostedTauMultiplicityWithHPSVeto', 5, 0.0, 5.0, 5, 0.0, 5.0)
    tauBoostedTauMultiplicityWithHPSVetoVVLooseMVA = ROOT.TH2F('tauBoostedTauMultiplicityWithHPSVetoVVLooseMVA','tauBoostedTauMultiplicityWithHPSVetoVVLooseMVA', 5, 0.0, 5.0, 5, 0.0, 5.0)
    tauBoostedTauMultiplicityWithHPSVetoVLooseMVA = ROOT.TH2F('tauBoostedTauMultiplicityWithHPSVetoVLooseMVA','tauBoostedTauMultiplicityWithHPSVetoVLooseMVA', 5, 0.0, 5.0, 5, 0.0, 5.0)
    tauBoostedTauMultiplicityWithHPSVetoLooseMVA = ROOT.TH2F('tauBoostedTauMultiplicityWithHPSVetoLooseMVA','tauBoostedTauMultiplicityWithHPSVetoLooseMVA', 5, 0.0, 5.0, 5, 0.0, 5.0)
    tauBoostedTauMultiplicityWithHPSVetoMediumMVA = ROOT.TH2F('tauBoostedTauMultiplicityWithHPSVetoMediumMVA','tauBoostedTauMultiplicityWithHPSVetoMediumMVA', 5, 0.0, 5.0, 5, 0.0, 5.0)
    tauBoostedTauMultiplicityWithHPSVetoTightMVA = ROOT.TH2F('tauBoostedTauMultiplicityWithHPSVetoTightMVA','tauBoostedTauMultiplicityWithHPSVetoTightMVA', 5, 0.0, 5.0, 5, 0.0, 5.0)
    tauBoostedTauMultiplicityWithHPSVetoVTightMVA = ROOT.TH2F('tauBoostedTauMultiplicityWithHPSVetoVTightMVA','tauBoostedTauMultiplicityWithHPSVetoVTightMVA', 5, 0.0, 5.0, 5, 0.0, 5.0)
    tauBoostedTauMultiplicityWithHPSVetoVVTightMVA = ROOT.TH2F('tauBoostedTauMultiplicityWithHPSVetoVVTightMVA','tauBoostedTauMultiplicityWithHPSVetoVVTightMVA', 5, 0.0, 5.0, 5, 0.0, 5.0)

    for eventIndex in tqdm(range(ttTree.GetEntries()), desc = 'Creating tau multiplicity: '):
        ttTree.GetEntry(eventIndex)

        totalBoostedTaus = ttTree.nboostedTau
        totalBoostedTausPassingVVLoose = 0
        totalBoostedTausPassingVLoose = 0
        totalBoostedTausPassingLoose = 0
        totalBoostedTausPassingMedium = 0
        totalBoostedTausPassingTight = 0
        totalBoostedTausPassingVTight = 0
        totalBoostedTausPassingVVTight = 0

        for boostedTauIndex in range(ttTree.nboostedTau):
            boostedTauIDBits = ''.join(format(ord(i), '016b') for i in ttTree.boostedTau_idMVAnewDM2017v2[boostedTauIndex])
            theBoostedTauID = int(boostedTauIDBits, 2)
            if theBoostedTauID >= 1:
                totalBoostedTausPassingVVLoose += 1
            if theBoostedTauID >= 3:
                totalBoostedTausPassingVLoose += 1
            if theBoostedTauID >= 7:
                totalBoostedTausPassingLoose += 1
            if theBoostedTauID >= 15:
                totalBoostedTausPassingMedium += 1
            if theBoostedTauID >= 31:
                totalBoostedTausPassingTight += 1
            if theBoostedTauID >= 63:
                totalBoostedTausPassingVTight += 1
            if theBoostedTauID >= 127:
                totalBoostedTausPassingVVTight += 1

        totalHPSTaus = 0
        totalHPSTausPassingVVLoose = 0
        totalHPSTausPassingVLoose = 0
        totalHPSTausPassingLoose = 0
        totalHPSTausPassingMedium = 0
        totalHPSTausPassingTight = 0
        totalHPSTausPassingVTight = 0
        totalHPSTausPassingVVTight = 0

        for tauIndex in range(ttTree.nTau):
            tauVector = ROOT.TLorentzVector()
            tauVector.SetPtEtaPhiM(ttTree.Tau_pt[tauIndex],
                                   ttTree.Tau_eta[tauIndex],
                                   ttTree.Tau_phi[tauIndex],
                                   ttTree.Tau_mass[tauIndex])
            isGoodTau = True
            for boostedTauIndex in range(ttTree.nboostedTau):
                boostedTauVector = ROOT.TLorentzVector()
                boostedTauVector.SetPtEtaPhiM(ttTree.boostedTau_pt[boostedTauIndex],
                                              ttTree.boostedTau_eta[boostedTauIndex],
                                              ttTree.boostedTau_phi[boostedTauIndex],
                                              ttTree.boostedTau_mass[boostedTauIndex])
                if tauVector.DeltaR(boostedTauVector) < 0.02:
                    isGoodTau = False
            if isGoodTau:
                totalHPSTaus += 1
                tauIDBits = ''.join(format(ord(i), '016b') for i in ttTree.Tau_idMVAnewDM2017v2[tauIndex])
                theTauID = int(tauIDBits, 2)
                if theTauID >= 1:
                    totalHPSTausPassingVVLoose += 1
                if theTauID >= 3:
                    totalHPSTausPassingVLoose += 1
                if theTauID >= 7:
                    totalHPSTausPassingLoose += 1
                if theTauID >= 15:
                    totalHPSTausPassingMedium += 1
                if theTauID >= 31:
                    totalHPSTausPassingTight += 1
                if theTauID >= 63:
                    totalHPSTausPassingVTight += 1
                if theTauID >= 127:
                    totalHPSTausPassingVVTight += 1
        totalTauMultiplicity = totalBoostedTaus + totalHPSTaus
        totalTauMultiplicityPassingVVLoose = totalBoostedTausPassingVVLoose + totalHPSTausPassingVVLoose
        totalTauMultiplicityPassingVLoose = totalBoostedTausPassingVLoose + totalHPSTausPassingVLoose
        totalTauMultiplicityPassingLoose = totalBoostedTausPassingLoose + totalHPSTausPassingLoose
        totalTauMultiplicityPassingMedium = totalBoostedTausPassingMedium + totalHPSTausPassingMedium
        totalTauMultiplicityPassingTight = totalBoostedTausPassingTight + totalHPSTausPassingTight
        totalTauMultiplicityPassingVTight = totalBoostedTausPassingVTight + totalHPSTausPassingVTight
        totalTauMultiplicityPassingVVTight = totalBoostedTausPassingVVTight + totalHPSTausPassingVVTight

        tauMultiplicityWithHPSVeto.Fill(totalTauMultiplicity, ttTree.FinalWeighting)
        tauMultiplicityWithHPSVetoVVLooseMVA.Fill(totalTauMultiplicityPassingVVLoose, ttTree.FinalWeighting)
        tauMultiplicityWithHPSVetoVLooseMVA.Fill(totalTauMultiplicityPassingVLoose, ttTree.FinalWeighting)
        tauMultiplicityWithHPSVetoLooseMVA.Fill(totalTauMultiplicityPassingLoose, ttTree.FinalWeighting)
        tauMultiplicityWithHPSVetoMediumMVA.Fill(totalTauMultiplicityPassingMedium, ttTree.FinalWeighting)
        tauMultiplicityWithHPSVetoTightMVA.Fill(totalTauMultiplicityPassingTight, ttTree.FinalWeighting)
        tauMultiplicityWithHPSVetoVTightMVA.Fill(totalTauMultiplicityPassingVTight, ttTree.FinalWeighting)
        tauMultiplicityWithHPSVetoVVTightMVA.Fill(totalTauMultiplicityPassingVVTight, ttTree.FinalWeighting)

        tauBoostedTauMultiplicityWithHPSVeto.Fill(totalBoostedTaus, totalHPSTaus, ttTree.FinalWeighting)
        tauBoostedTauMultiplicityWithHPSVetoVVLooseMVA.Fill(totalBoostedTausPassingVVLoose, totalHPSTausPassingVVLoose, ttTree.FinalWeighting)
        tauBoostedTauMultiplicityWithHPSVetoVLooseMVA.Fill(totalBoostedTausPassingVLoose, totalHPSTausPassingVLoose, ttTree.FinalWeighting)
        tauBoostedTauMultiplicityWithHPSVetoLooseMVA.Fill(totalBoostedTausPassingLoose, totalHPSTausPassingLoose, ttTree.FinalWeighting)
        tauBoostedTauMultiplicityWithHPSVetoMediumMVA.Fill(totalBoostedTausPassingMedium, totalHPSTausPassingMedium, ttTree.FinalWeighting)
        tauBoostedTauMultiplicityWithHPSVetoTightMVA.Fill(totalBoostedTausPassingTight, totalHPSTausPassingTight, ttTree.FinalWeighting)
        tauBoostedTauMultiplicityWithHPSVetoVTightMVA.Fill(totalBoostedTausPassingVTight, totalHPSTausPassingVTight, ttTree.FinalWeighting)
        tauBoostedTauMultiplicityWithHPSVetoVVTightMVA.Fill(totalBoostedTausPassingVVTight, totalHPSTausPassingVVTight, ttTree.FinalWeighting)

    tauMultiplicityWithHPSVeto.Write()
    tauMultiplicityWithHPSVetoVVLooseMVA.Write()
    tauMultiplicityWithHPSVetoVLooseMVA.Write()
    tauMultiplicityWithHPSVetoLooseMVA.Write()
    tauMultiplicityWithHPSVetoMediumMVA.Write()
    tauMultiplicityWithHPSVetoTightMVA.Write()
    tauMultiplicityWithHPSVetoVTightMVA.Write()
    tauMultiplicityWithHPSVetoVVTightMVA.Write()
    
    tauBoostedTauMultiplicityWithHPSVeto.Write()
    tauBoostedTauMultiplicityWithHPSVetoVVLooseMVA.Write()
    tauBoostedTauMultiplicityWithHPSVetoVLooseMVA.Write()
    tauBoostedTauMultiplicityWithHPSVetoLooseMVA.Write()
    tauBoostedTauMultiplicityWithHPSVetoMediumMVA.Write()
    tauBoostedTauMultiplicityWithHPSVetoTightMVA.Write()
    tauBoostedTauMultiplicityWithHPSVetoVTightMVA.Write()
    tauBoostedTauMultiplicityWithHPSVetoVVTightMVA.Write()

    HPSVetoCanvas = ROOT.TCanvas('HPSVetoCanvas','HPSVetoCanvas')
    HPSVetoCanvas.cd()
    tauBoostedTauMultiplicityWithHPSVeto.Draw('COLZ TEXT')
    tauBoostedTauMultiplicityWithHPSVeto.GetXaxis().SetTitle("nBoostedTau")
    tauBoostedTauMultiplicityWithHPSVeto.GetYaxis().SetTitle("nTau")
    HPSVetoCanvas.Write()

    HPSVetoCanvasVVLooseMVA = ROOT.TCanvas('HPSVetoCanvasVVLooseMVA','HPSVetoCanvasVVLooseMVA')
    HPSVetoCanvasVVLooseMVA.cd()
    tauBoostedTauMultiplicityWithHPSVetoVVLooseMVA.Draw('COLZ TEXT')
    tauBoostedTauMultiplicityWithHPSVetoVVLooseMVA.GetXaxis().SetTitle("nBoostedTau")
    tauBoostedTauMultiplicityWithHPSVetoVVLooseMVA.GetYaxis().SetTitle("nTau")
    HPSVetoCanvasVVLooseMVA.Write()

    HPSVetoCanvasVLooseMVA = ROOT.TCanvas('HPSVetoCanvasVLooseMVA','HPSVetoCanvasVLooseMVA')
    HPSVetoCanvasVLooseMVA.cd()
    tauBoostedTauMultiplicityWithHPSVetoVLooseMVA.Draw('COLZ TEXT')
    tauBoostedTauMultiplicityWithHPSVetoVLooseMVA.GetXaxis().SetTitle("nBoostedTau")
    tauBoostedTauMultiplicityWithHPSVetoVLooseMVA.GetYaxis().SetTitle("nTau")
    HPSVetoCanvasVLooseMVA.Write()

    HPSVetoCanvasLooseMVA = ROOT.TCanvas('HPSVetoCanvasLooseMVA','HPSVetoCanvasLooseMVA')
    HPSVetoCanvasLooseMVA.cd()
    tauBoostedTauMultiplicityWithHPSVetoLooseMVA.Draw('COLZ TEXT')
    tauBoostedTauMultiplicityWithHPSVetoLooseMVA.GetXaxis().SetTitle("nBoostedTau")
    tauBoostedTauMultiplicityWithHPSVetoLooseMVA.GetYaxis().SetTitle("nTau")
    HPSVetoCanvasLooseMVA.Write()

    HPSVetoCanvasMediumMVA = ROOT.TCanvas('HPSVetoCanvasMediumMVA','HPSVetoCanvasMediumMVA')
    HPSVetoCanvasMediumMVA.cd()
    tauBoostedTauMultiplicityWithHPSVetoMediumMVA.Draw('COLZ TEXT')
    tauBoostedTauMultiplicityWithHPSVetoMediumMVA.GetXaxis().SetTitle("nBoostedTau")
    tauBoostedTauMultiplicityWithHPSVetoMediumMVA.GetYaxis().SetTitle("nTau")
    HPSVetoCanvasMediumMVA.Write()

    HPSVetoCanvasTightMVA = ROOT.TCanvas('HPSVetoCanvasTightMVA','HPSVetoCanvasTightMVA')
    HPSVetoCanvasTightMVA.cd()
    tauBoostedTauMultiplicityWithHPSVetoTightMVA.Draw('COLZ TEXT')
    tauBoostedTauMultiplicityWithHPSVetoTightMVA.GetXaxis().SetTitle("nBoostedTau")
    tauBoostedTauMultiplicityWithHPSVetoTightMVA.GetYaxis().SetTitle("nTau")
    HPSVetoCanvasTightMVA.Write()

    HPSVetoCanvasVTightMVA = ROOT.TCanvas('HPSVetoCanvasVTightMVA','HPSVetoCanvasVTightMVA')
    HPSVetoCanvasVTightMVA.cd()
    tauBoostedTauMultiplicityWithHPSVetoVTightMVA.Draw('COLZ TEXT')
    tauBoostedTauMultiplicityWithHPSVetoVTightMVA.GetXaxis().SetTitle("nBoostedTau")
    tauBoostedTauMultiplicityWithHPSVetoVTightMVA.GetYaxis().SetTitle("nTau")
    HPSVetoCanvasVTightMVA.Write()

    HPSVetoCanvasVVTightMVA = ROOT.TCanvas('HPSVetoCanvasVVTightMVA','HPSVetoCanvasVVTightMVA')
    HPSVetoCanvasVVTightMVA.cd()
    tauBoostedTauMultiplicityWithHPSVetoVVTightMVA.Draw('COLZ TEXT')
    tauBoostedTauMultiplicityWithHPSVetoVVTightMVA.GetXaxis().SetTitle("nBoostedTau")
    tauBoostedTauMultiplicityWithHPSVetoVVTightMVA.GetYaxis().SetTitle("nTau")
    HPSVetoCanvasVVTightMVA.Write()

    tauMultiplicityHPSVetoCanvas = ROOT.TCanvas('tauMultiplicityHPSVetoCanvas','tauMultiplicityHPSVetoCanvas')
    tauMultiplicityWithHPSVeto.SetLineColor(ROOT.kBlack)
    tauMultiplicityWithHPSVetoVVLooseMVA.SetLineColor(ROOT.kRed)
    tauMultiplicityWithHPSVetoVLooseMVA.SetLineColor(ROOT.kOrange)
    tauMultiplicityWithHPSVetoLooseMVA.SetLineColor(ROOT.kGreen)
    tauMultiplicityWithHPSVetoMediumMVA.SetLineColor(ROOT.kCyan)
    tauMultiplicityWithHPSVetoTightMVA.SetLineColor(ROOT.kBlue)
    tauMultiplicityWithHPSVetoVTightMVA.SetLineColor(ROOT.kMagenta)
    tauMultiplicityWithHPSVetoVVTightMVA.SetLineColor(ROOT.kPink)
    
    tauMultiplicityWithHPSVeto.SetMaximum(max(tauMultiplicityWithHPSVeto.GetMaximum(), 
                                              max(tauMultiplicityWithHPSVetoVVLooseMVA.GetMaximum(),
                                                  tauMultiplicityWithHPSVetoVVTightMVA.GetMaximum())) * 1.1)
    tauMultiplicityWithHPSVeto.Draw()
    tauMultiplicityWithHPSVetoVVLooseMVA.Draw("SAME")
    tauMultiplicityWithHPSVetoVLooseMVA.Draw("SAME")
    tauMultiplicityWithHPSVetoLooseMVA.Draw("SAME")
    tauMultiplicityWithHPSVetoMediumMVA.Draw("SAME")
    tauMultiplicityWithHPSVetoTightMVA.Draw("SAME")
    tauMultiplicityWithHPSVetoVTightMVA.Draw("SAME")
    tauMultiplicityWithHPSVetoVVTightMVA.Draw("SAME")

    tauMultiplicityHPSVetoLeg = ROOT.TLegend(0.7,0.7,0.9,0.9)
    tauMultiplicityHPSVetoLeg.AddEntry(tauMultiplicityHPSVetoLeg, 'No ID Condition', 'l')
    tauMultiplicityHPSVetoLeg.AddEntry(tauMultiplicityWithHPSVetoVVLooseMVA, 'VVLoose MVA', 'l')
    tauMultiplicityHPSVetoLeg.AddEntry(tauMultiplicityWithHPSVetoVLooseMVA, 'VLoose MVA', 'l')
    tauMultiplicityHPSVetoLeg.AddEntry(tauMultiplicityWithHPSVetoLooseMVA, 'Loose MVA', 'l')
    tauMultiplicityHPSVetoLeg.AddEntry(tauMultiplicityWithHPSVetoMediumMVA, 'Medium MVA', 'l')
    tauMultiplicityHPSVetoLeg.AddEntry(tauMultiplicityWithHPSVetoTightMVA, 'Tight MVA', 'l')
    tauMultiplicityHPSVetoLeg.AddEntry(tauMultiplicityWithHPSVetoVTightMVA, 'VTight MVA', 'l')
    tauMultiplicityHPSVetoLeg.AddEntry(tauMultiplicityWithHPSVetoVVTightMVA, 'VVTight MVA', 'l')
    tauMultiplicityHPSVetoLeg.Draw()
    
    tauMultiplicityHPSVetoCanvas.Write()

    print('et channel object multiplicity plots...')
    etDir.cd()

    etBoostedTauMultiplicityName = 'etBoostedTauMultiplicity'
    etTauMultiplicityName = 'etTauMultiplicity'
    etElectronMultiplicityName = 'etElectronMultiplicity'
    etMuonMultiplicityName = 'etMuonMultiplicity'

    etTree.Draw('nboostedTau>>%s' %etBoostedTauMultiplicityName,'FinalWeighting')
    etBoostedTauMultiplicity = ROOT.gDirectory.Get(etBoostedTauMultiplicityName)
    etBoostedTauMultiplicity.Write()

    etTree.Draw('nTau>>%s'%etTauMultiplicityName, 'FinalWeighting')
    etTauMultiplicity = ROOT.gDirectory.Get(etTauMultiplicityName)
    etTauMultiplicity.Write()

    etTree.Draw('nMuon>>%s'%etMuonMultiplicityName, 'FinalWeighting')
    etMuonMultiplicity = ROOT.gDirectory.Get(etMuonMultiplicityName)
    etMuonMultiplicity.Write()

    etTree.Draw('nElectron>>%s'%etElectronMultiplicityName, 'FinalWeighting')
    etElectronMultiplicity = ROOT.gDirectory.Get(etElectronMultiplicityName)
    etElectronMultiplicity.Write()

    print('et channel selection efficiency plots...')
    etDir.cd()
    #okay, we want to be able to compare to the tt channel version of this
    #so we want a version of the plot with these bins
    #1.) How many events do we have where we get a boosted tau
    #2.) how many events dow e get where we get a boosted tau that passes our ID requirement
    #3.) The above plus an electron that passes our ID requirement
    #4.) The above plus no other taus boosted or otherwise that pass ID
    #5.) the above plus no other electrons that pass ID
    #6.) How many events do we have where we get a regular tau
    #7.) How many events do we have where we get a regular tau that passes our ID requirement
    #8.) the above plus an electron that passes our ID requirement
    #9.) the above plus no other taus boosted or otherwise that pass ID
    #10.) the above plus no other electrons that pass ID
    etSelectionCanvas = ROOT.TCanvas('etSelectionCanvas', 'etSelectionCanvas')
    etSelectionCanvas.SetBottomMargin(0.4)
    etSelectionPlot = ROOT.TH1F('etSelectionPlot','etSelectionPlot', 10, 1.0, 11.0)

    for i in tqdm(range(etTree.GetEntries()), desc = 'et selection: events:'):
        etTree.GetEntry(i)
        
        boostedTausPassID = []
        tausPassID = []
        electronsPassID = []

        for boostedTauIndex in range(etTree.nboostedTau):
            if (etTree.boostedTau_idMVAnewDM2017v2[boostedTauIndex] >= tauIDSelectionBits):
                boostedTausPassID.append(boostedTauIndex)
            
        for tauIndex in range(etTree.nTau):
            if (etTree.Tau_idMVAnewDM2017v2[tauIndex] >= tauIDSelectionBits):
                tausPassID.append(tauIndex)

        for electronIndex in range(etTree.nElectron):
            if(etTree.Electron_mvaFall17V2Iso_WPL[electronIndex]):
                electronsPassID.append(electronIndex)

        if(etTree.nboostedTau > 0):
            etSelectionPlot.Fill(1.0, etTree.FinalWeighting)
            if (len(boostedTausPassID) >= 1):
                etSelectionPlot.Fill(2.0, etTree.FinalWeighting)
                if (len(electronsPassID) >= 1):
                    etSelectionPlot.Fill(3.0, etTree.FinalWeighting)
                    if (len(boostedTausPassID) ==  1 and len(tausPassID) == 0):
                        etSelectionPlot.Fill(4.0, etTree.FinalWeighting)
                        if(len(electronsPassID) == 1):
                            etSelectionPlot.Fill(5.0, etTree.FinalWeighting)
        if(etTree.nTau > 0):
            etSelectionPlot.Fill(6.0, etTree.FinalWeighting)
            if(len(tausPassID) >= 1):
                etSelectionPlot.Fill(7.0, etTree.FinalWeighting)
                if(len(electronsPassID) >= 1):
                    etSelectionPlot.Fill(8.0, etTree.FinalWeighting)
                    if(len(tausPassID) == 1 and len(boostedTausPassID) == 0):
                        etSelectionPlot.Fill(9.0, etTree.FinalWeighting)
                        if (len(electronsPassID) == 1):
                            etSelectionPlot.Fill(10.0, etTree.FinalWeighting)
    #let's start doing the stylistic stuff
    etSelectionCanvas.cd()
    etSelectionPlot.Draw()
    etSelectionPlot.SetMaximum(1.1*etSelectionPlot.GetMaximum())
    etSelectionPlot.Draw()
    etSelectionPlot.GetXaxis().SetBinLabel(1, "Events with a boosted tau")    
    etSelectionPlot.GetXaxis().SetBinLabel(2, "Events with a boosted tau That passes ID")    
    etSelectionPlot.GetXaxis().SetBinLabel(3, "Boosted Tau + ID && Electron + ID")    
    etSelectionPlot.GetXaxis().SetBinLabel(4, "Boosted Tau + ID && Electron + ID && no other other taus passing requirements")    
    etSelectionPlot.GetXaxis().SetBinLabel(5, "Boosted Tau + ID && Electron + ID && no other taus or electrons passing requirements")    
    etSelectionPlot.GetXaxis().SetBinLabel(6, "Events with an HPS tau")    
    etSelectionPlot.GetXaxis().SetBinLabel(7, "Events with an HPS tau that passes ID")    
    etSelectionPlot.GetXaxis().SetBinLabel(8, "HPS Tau + ID && Electron + ID")    
    etSelectionPlot.GetXaxis().SetBinLabel(9, "HPS Tau + ID && Electron + ID && no other taus passing requirements")    
    etSelectionPlot.GetXaxis().SetBinLabel(10, "HPS Tau + ID && Electron + ID && no other taus or electrons passing requirements")

    etFirstSection = ROOT.TLine(6.0, 0.0, 6.0, etSelectionPlot.GetMaximum())
    etFirstSection.SetLineColor(ROOT.kBlack)
    etFirstSection.Draw()

    for i in range(1,11):
        binLabelLatex.DrawLatex(i+0.5, (etSelectionPlot.GetBinContent(i) + 1) * 1.05, str(int(etSelectionPlot.GetBinContent(i))))

    etSelectionCanvas.Write()

    print('et channel other lepton rejection plots...')
    etDir.cd()

    #let's look at electron, muon and tau rejection here
    electronRejectionCanvas = ROOT.TCanvas('electronRejectionCanvas','electronRejectionCanvas')
    electronRejectionCanvasIdentifiedTaus = ROOT.TCanvas('electronRejectionCanvasIdentifiedtaus', 'electronRejectionCanvasIdentifiedTaus')
    electronRejectionCanvasChannelStyle = ROOT.TCanvas('electronRejectionCanvasChannelStyle','electronRejectionCanvasChannelStyle')
    
    #3 histograms
    # one for n electrons no ID
    # same for current ID
    # next for WP 80
    # next for WP 90

    #Would also be good to see this for at least 1 identified tau in the event
    #and for 2 identified boosted taus, 1 boosted and 1 regular identified tau, and 2 regular identified taus in the event
    nElectronsNoID = ROOT.TH1F('nElectronsNoID','nElectronsNoID', 5, 0.0, 5.0)
    nElectronsLooseMVA = ROOT.TH1F('nElectronsLooseMVA','nElectronsLooseMVA', 5, 0.0, 5.0)
    nElectronsWP80 = ROOT.TH1F('nElectronsWP80','nElectronsWP80', 5, 0.0, 5.0)
    nElectronsWP90 = ROOT.TH1F('nElectronsWP90', 'nElectronsWP90', 5, 0.0, 5.0)

    nElectronsNoIDIdentifiedTaus = ROOT.TH1F('nElectronsNoIDIdentifiedTaus', 'nElectronsNoIDIdentifiedTaus', 5, 0.0, 5.0)
    nElectronsLooseMVAIdentifiedTaus = ROOT.TH1F('nElectronsLooseMVAIdentifiedTaus','nElectronsLooseMVAIdentifiedTaus', 5, 0.0, 5.0)
    nElectronsWP80IdentifiedTaus = ROOT.TH1F('nElectronsWP80IdentifiedTaus', 'nElectronsWP80IdentifiedTaus', 5, 0.0, 5.0)
    nElectronsWP90IdentifiedTaus = ROOT.TH1F('nElectronsWP90IdentifiedTaus','nElectronsWP90IdentifiedTaus', 5, 0.0, 5.0)

    nElectronsNoIDChannelStyle = ROOT.TH1F('nElectronsNoIDChannelStyle', 'nElectronsNoIDChannelStyle', 5, 0.0, 5.0)
    nElectronsLooseMVAChannelStyle = ROOT.TH1F('nElectronsLooseMVAChannelStyle','nElectronsLooseMVAChannelStyle', 5, 0.0, 5.0)
    nElectronsWP80ChannelStyle = ROOT.TH1F('nElectronsWP80ChannelStyle', 'nElectronsWP80ChannelStyle', 5, 0.0, 5.0)
    nElectronsWP90ChannelStyle = ROOT.TH1F('nElectronsWP90ChannelStyle', 'nElectronsWP90ChannelStyle', 5, 0.0, 5.0)
    
    for entryIndex in tqdm(range(etTree.GetEntries()), desc = 'Events'):
        etTree.GetEntry(entryIndex)
        boostedTausPassingID = []
        tausPassingID = []
        electronsPassingID = []
        #first, let's just scroll the taus and figure out if anything passes any ID
        for tauIndex in range(etTree.nTau):
            if etTree.Tau_idMVAnewDM2017v2[tauIndex] >= tauIDSelectionBits:
                tausPassingID.append(tauIndex)
        for boostedTauIndex in range(etTree.nboostedTau):
            if etTree.boostedTau_idMVAnewDM2017v2[boostedTauIndex] >= tauIDSelectionBits:
                boostedTausPassingID.append(boostedTauIndex)
        for electronIndex in range(etTree.nElectron):
            if (etTree.Electron_mvaFall17V2Iso_WPL[electronIndex]):
                electronsPassingID.append(electronIndex)
        #okay, now we scroll the electrons and start filling the whatever
        nElectronsNoID.Fill(etTree.nElectron, etTree.FinalWeighting)
        electronsPassingLooseMVA = 0
        electronsPassingWP80 = 0
        electronsPassingWP90 = 0

        electronsPassingLooseMVAIdentifiedTaus = 0
        electronsPassingWP80IdentifiedTaus = 0
        electronsPassingWP90IdentifiedTaus = 0

        electronsPassingLooseMVAChannelStyle = 0
        electronsPassingWP80ChannelStyle = 0
        electronsPassingWP90ChannelStyle = 0

        for electronIndex in range(etTree.nElectron):
            if (etTree.Electron_mvaFall17V2Iso_WPL[electronIndex]):
                electronsPassingLooseMVA += 1
            if (etTree.Electron_mvaFall17V2Iso_WP80[electronIndex]):
                electronsPassingWP80 += 1
            if (etTree.Electron_mvaFall17V2Iso_WP90[electronIndex]):
                electronsPassingWP90 += 1
        nElectronsLooseMVA.Fill(electronsPassingLooseMVA, etTree.FinalWeighting)
        nElectronsWP80.Fill(electronsPassingWP80, etTree.FinalWeighting)
        nElectronsWP90.Fill(electronsPassingWP90, etTree.FinalWeighting)

        #same thing, but for events containing at least one good tau
        if (len(boostedTausPassingID) > 0 or len(tausPassingID) > 0):
            for electronIndex in range(etTree.nElectron):
                nElectronsNoIDIdentifiedTaus.Fill(etTree.nElectron,etTree.FinalWeighting)
                if (etTree.Electron_mvaFall17V2Iso_WPL[electronIndex]):
                    electronsPassingLooseMVAIdentifiedTaus += 1
                if (etTree.Electron_mvaFall17V2Iso_WP80[electronIndex]):
                    electronsPassingWP80IdentifiedTaus += 1
                if (etTree.Electron_mvaFall17V2Iso_WP90[electronIndex]):
                    electronsPassingWP90IdentifiedTaus += 1
            nElectronsLooseMVAIdentifiedTaus.Fill(electronsPassingLooseMVAIdentifiedTaus, etTree.FinalWeighting)
            nElectronsWP80IdentifiedTaus.Fill(electronsPassingWP80IdentifiedTaus, etTree.FinalWeighting)
            nElectronsWP90IdentifiedTaus.Fill(electronsPassingWP90IdentifiedTaus, etTree.FinalWeighting)
            
        #same thing, but for configurations that look similar to our final channel selection
        if (len(electronsPassingID) ==1 
            and (len(boostedTausPassingID) ==1 or len(tausPassingID) == 1)):
            for electronIndex in range(etTree.nElectron):
                nElectronsNoIDChannelStyle.Fill(etTree.nElectron,etTree.FinalWeighting)
                if (etTree.Electron_mvaFall17V2Iso_WPL[electronIndex]):
                    electronsPassingLooseMVAChannelStyle += 1
                if (etTree.Electron_mvaFall17V2Iso_WP80[electronIndex]):
                    electronsPassingWP80ChannelStyle += 1
                if (etTree.Electron_mvaFall17V2Iso_WP90[electronIndex]):
                    electronsPassingWP90ChannelStyle += 1
            nElectronsLooseMVAChannelStyle.Fill(electronsPassingLooseMVA, etTree.FinalWeighting)
            nElectronsWP80ChannelStyle.Fill(electronsPassingWP80, etTree.FinalWeighting)
            nElectronsWP90ChannelStyle.Fill(electronsPassingWP90, etTree.FinalWeighting)
    #okay, now we do the styling on the electron rejection plot
    nElectronsNoID.SetLineColor(ROOT.kBlack)
    nElectronsNoIDIdentifiedTaus.SetLineColor(ROOT.kBlack)
    nElectronsNoIDChannelStyle.SetLineColor(ROOT.kBlack)
    
    nElectronsLooseMVA.SetLineColor(ROOT.kBlue)
    nElectronsLooseMVAIdentifiedTaus.SetLineColor(ROOT.kBlue)
    nElectronsLooseMVAChannelStyle.SetLineColor(ROOT.kBlue)
    
    nElectronsWP80.SetLineColor(ROOT.kRed)
    nElectronsWP80IdentifiedTaus.SetLineColor(ROOT.kRed)
    nElectronsWP80ChannelStyle.SetLineColor(ROOT.kRed)
    
    nElectronsWP90.SetLineColor(ROOT.kGreen)
    nElectronsWP90IdentifiedTaus.SetLineColor(ROOT.kGreen)
    nElectronsWP90ChannelStyle.SetLineColor(ROOT.kGreen)

    electronRejectionCanvas.cd()
    nElectronsNoID.SetMaximum(max(nElectronsWP80.GetMaximum(), 
                                  max(nElectronsLooseMVA.GetMaximum(), nElectronsNoID.GetMaximum())) * 1.1)
    nElectronsNoID.Draw()
    nElectronsLooseMVA.Draw('SAME')
    nElectronsWP80.Draw('SAME')
    nElectronsWP90.Draw('SAME')
    #make the legend
    noTauConditionLegend = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    noTauConditionLegend.AddEntry(nElectronsNoID,"No Electron ID Condition", "l")
    noTauConditionLegend.AddEntry(nElectronsLooseMVA, "Passes LooseMVA", "l")
    noTauConditionLegend.AddEntry(nElectronsWP80, "Passes WP80", "l")
    noTauConditionLegend.AddEntry(nElectronsWP90, "Passes WP90", "l")
    noTauConditionLegend.Draw()
    electronRejectionCanvas.Write()

    electronRejectionCanvasIdentifiedTaus.cd()
    nElectronsNoIDIdentifiedTaus.SetMaximum(max(nElectronsWP80IdentifiedTaus.GetMaximum(), 
                                                max(nElectronsLooseMVAIdentifiedTaus.GetMaximum(), nElectronsNoIDIdentifiedTaus.GetMaximum())) * 1.1)
    nElectronsNoIDIdentifiedTaus.Draw()
    nElectronsLooseMVAIdentifiedTaus.Draw('SAME')
    nElectronsWP80IdentifiedTaus.Draw('SAME')
    nElectronsWP90IdentifiedTaus.Draw('SAME')
    #make the legend
    IDTauConditionLegend = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    IDTauConditionLegend.AddEntry(nElectronsNoIDIdentifiedTaus, "No Electron ID Condition", "l")
    IDTauConditionLegend.AddEntry(nElectronsLooseMVAIdentifiedTaus, "Passes LooseMVA", "l")
    IDTauConditionLegend.AddEntry(nElectronsWP80IdentifiedTaus, "Passes WP80", "l")
    IDTauConditionLegend.AddEntry(nElectronsWP90IdentifiedTaus, "Pases WP90" ,"l")
    IDTauConditionLegend.Draw()
    electronRejectionCanvasIdentifiedTaus.Write()

    electronRejectionCanvasChannelStyle.cd()
    nElectronsNoIDChannelStyle.SetMaximum(max(nElectronsWP80ChannelStyle.GetMaximum(), 
                                              max(nElectronsLooseMVAChannelStyle.GetMaximum(), nElectronsNoIDChannelStyle.GetMaximum())) * 1.1)
    nElectronsNoIDChannelStyle.Draw()
    nElectronsLooseMVAChannelStyle.Draw('SAME')
    nElectronsWP80ChannelStyle.Draw('SAME')
    nElectronsWP90ChannelStyle.Draw('SAME')
    #make the legend
    channelStyleLegend = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    channelStyleLegend.AddEntry( nElectronsNoIDChannelStyle,"No Electron ID Condition", "l")
    channelStyleLegend.AddEntry(nElectronsLooseMVAChannelStyle,"Passes LooseMVA" , "l")
    channelStyleLegend.AddEntry(nElectronsWP80ChannelStyle, "Passes WP80", "l")
    channelStyleLegend.AddEntry(nElectronsWP90ChannelStyle, "Passes WP90", "l")
    channelStyleLegend.Draw()
    electronRejectionCanvasChannelStyle.Write()

    muonRejectionCanvas = ROOT.TCanvas('muonRejectionCanvas', 'muonRejectionCanvas')
    muonRejectionCanvasIdentifiedTaus = ROOT.TCanvas('muonRejectionCanvasIdentifiedTaus','muonRejectionCanvasIdentifiedTaus')
    muonRejectionCanvasChannelStyle = ROOT.TCanvas('muonRejectionCanvasChannelStyle','muonRejectionCanvasChannelStyle')

    nMuonsNoID = ROOT.TH1F('nMuonsNoID','nMuonsNoID', 5, 0.0, 5.0)
    nMuonsMVALoose = ROOT.TH1F('nMuonsMVALoose','nMuonsMVALoose', 5, 0.0, 5.0)
    nMuonsMVAMedium = ROOT.TH1F('nMuonsMVAMedium', 'nMuonsMVAMedium', 5, 0.0, 5.0)
    nMuonsMVATight = ROOT.TH1F('nMuonsMVATight','nMuonsMVATight', 5, 0.0, 5.0)
    nMuonsMVAVTight = ROOT.TH1F('nMuonsMVAVTight','nMuonsMVAVTight', 5, 0.0, 5.0)
    nMuonsMVAVVTight = ROOT.TH1F('nMuonsMVAVVTight', 'nMuonsMVAVVTight', 5, 0.0, 5.0)
    
    nMuonsNoIDIdentifiedTaus = ROOT.TH1F('nMuonsNoIDIdentifiedTaus','nMuonsNoIDIdentifiedTaus', 5, 0.0, 5.0)
    nMuonsMVALooseIdentifiedTaus = ROOT.TH1F('nMuonsMVALooseIdentifiedTaus','nMuonsMVALooseIdentifiedTaus', 5, 0.0, 5.0)
    nMuonsMVAMediumIdentifiedTaus = ROOT.TH1F('nMuonsMVAMediumIdentifiedTaus', 'nMuonsMVAMediumIdentifiedTaus', 5, 0.0, 5.0)
    nMuonsMVATightIdentifiedTaus = ROOT.TH1F('nMuonsMVATightIdentifiedTaus','nMuonsMVATightIdentifiedTaus', 5, 0.0, 5.0)
    nMuonsMVAVTightIdentifiedTaus = ROOT.TH1F('nMuonsMVAVTightIdentifiedTaus','nMuonsMVAVTightIdentifiedTaus', 5, 0.0, 5.0)
    nMuonsMVAVVTightIdentifiedTaus = ROOT.TH1F('nMuonsMVAVVTightIdentifiedTaus', 'nMuonsMVAVVTightIdentifiedTaus', 5, 0.0, 5.0)

    nMuonsNoIDChannelStyle = ROOT.TH1F('nMuonsNoIDChannelStyle','nMuonsNoIDChannelStyle', 5, 0.0, 5.0)
    nMuonsMVALooseChannelStyle = ROOT.TH1F('nMuonsMVALooseChannelStyle','nMuonsMVALooseChannelStyle', 5, 0.0, 5.0)
    nMuonsMVAMediumChannelStyle = ROOT.TH1F('nMuonsMVAMediumChannelStyle', 'nMuonsMVAMediumChannelStyle', 5, 0.0, 5.0)
    nMuonsMVATightChannelStyle = ROOT.TH1F('nMuonsMVATightChannelStyle','nMuonsMVATightChannelStyle', 5, 0.0, 5.0)
    nMuonsMVAVTightChannelStyle = ROOT.TH1F('nMuonsMVAVTightChannelStyle','nMuonsMVAVTightChannelStyle', 5, 0.0, 5.0)
    nMuonsMVAVVTightChannelStyle = ROOT.TH1F('nMuonsMVAVVTightChannelStyle', 'nMuonsMVAVVTightChannelStyle', 5, 0.0, 5.0)

    for entryIndex in tqdm(range(etTree.GetEntries()), desc = 'Events'):
        etTree.GetEntry(entryIndex)
        boostedTausPassingID = []
        tausPassingID = []
        
        #let's do what we did before and scroll the taus to figure out if anything passes ID
        for tauIndex in range(etTree.nTau):
            if etTree.Tau_idMVAnewDM2017v2[tauIndex] >= tauIDSelectionBits:
                tausPassingID.append(tauIndex)
        for boostedTauIndex in range(etTree.nboostedTau):
            if etTree.boostedTau_idMVAnewDM2017v2[boostedTauIndex] >= tauIDSelectionBits:
                boostedTausPassingID.append(boostedTauIndex)

        #now we fill muon plots
        nMuonsNoID.Fill(etTree.nMuon, etTree.FinalWeighting)
        muonsPassingLooseMVA = 0
        muonsPassingMediumMVA = 0
        muonsPassingTightMVA = 0
        muonsPassingVTightMVA = 0
        muonsPassingVVTightMVA = 0

        muonsPassingLooseMVAIdentifiedTaus = 0
        muonsPassingMediumMVAIdentifiedTaus = 0
        muonsPassingTightMVAIdentifiedTaus = 0
        muonsPassingVTightMVAIdentifiedTaus = 0
        muonsPassingVVTightMVAIdentifiedTaus = 0

        muonsPassingLooseMVAChannelStyle = 0
        muonsPassingMediumMVAChannelStyle = 0
        muonsPassingTightMVAChannelStyle = 0
        muonsPassingVTightMVAChannelStyle = 0
        muonsPassingVVTightMVAChannelStyle = 0

        for muonIndex in range(etTree.nMuon):
            theMuonIDBits = ''.join(format(ord(i), '016b')for i in etTree.Muon_mvaId[muonIndex])
            theMuonID = int(theMuonIDBits, 2)
            if theMuonID >= 1:
                muonsPassingLooseMVA += 1
            if theMuonID >= 2:
                muonsPassingMediumMVA += 1
            if theMuonID >= 3:
                muonsPassingTightMVA += 1
            if theMuonID >= 4:
                muonsPassingVTightMVA += 1
            if theMuonID >= 5:
                muonsPassingVVTightMVA += 1
        nMuonsMVALoose.Fill(muonsPassingLooseMVA, etTree.FinalWeighting)
        nMuonsMVAMedium.Fill(muonsPassingMediumMVA, etTree.FinalWeighting)
        nMuonsMVATight.Fill(muonsPassingTightMVA, etTree.FinalWeighting)
        nMuonsMVAVTight.Fill(muonsPassingVTightMVA, etTree.FinalWeighting)
        nMuonsMVAVVTight.Fill(muonsPassingVVTightMVA, etTree.FinalWeighting)
        
        if (len(boostedTausPassingID) > 0 or len(tausPassingID) > 0):
            nMuonsNoIDIdentifiedTaus.Fill(etTree.nMuon, etTree.FinalWeighting)
            for muonIndex in range(etTree.nMuon):
                theMuonIDBits = ''.join(format(ord(i), '016b')for i in etTree.Muon_mvaId[muonIndex])
                theMuonID = int(theMuonIDBits, 2)
                if theMuonID >= 1:
                    muonsPassingLooseMVAIdentifiedTaus += 1
                if theMuonID >= 2:
                    muonsPassingMediumMVAIdentifiedTaus += 1
                if theMuonID >= 3:
                    muonsPassingTightMVAIdentifiedTaus += 1
                if theMuonID >= 4:
                    muonsPassingVTightMVAIdentifiedTaus += 1
                if theMuonID >= 5:
                    muonsPassingVVTightMVAIdentifiedTaus += 1
            nMuonsMVALoose.Fill(muonsPassingLooseMVA, etTree.FinalWeighting)
            nMuonsMVALooseIdentifiedTaus.Fill(muonsPassingLooseMVAIdentifiedTaus, etTree.FinalWeighting)
            nMuonsMVAMediumIdentifiedTaus.Fill(muonsPassingMediumMVAIdentifiedTaus, etTree.FinalWeighting)
            nMuonsMVATightIdentifiedTaus.Fill(muonsPassingTightMVAIdentifiedTaus, etTree.FinalWeighting)
            nMuonsMVAVTightIdentifiedTaus.Fill(muonsPassingVTightMVAIdentifiedTaus, etTree.FinalWeighting)
            nMuonsMVAVVTightIdentifiedTaus.Fill(muonsPassingVVTightMVAIdentifiedTaus, etTree.FinalWeighting)
            
            
        if ((len(boostedTausPassID) == 2) 
            or (len(boostedTausPassID) == 1 and len(tausPassingID) == 1) 
            or (len(tausPassingID) == 2) ):
            nMuonsNoIDChannelStyle.Fill(etTree.nMuon, etTree.FinalWeighting)
            for muonIndex in range(etTree.nMuon):
                theMuonIDBits = ''.join(format(ord(i), '016b')for i in etTree.Muon_mvaId[muonIndex])
                theMuonID = int(theMuonIDBits, 2)
                if theMuonID >= 1:
                    muonsPassingLooseMVAChannelStyle += 1
                if theMuonID >= 2:
                    muonsPassingMediumMVAChannelStyle += 1
                if theMuonID >= 3:
                    muonsPassingTightMVAChannelStyle += 1
                if theMuonID >= 4:
                    muonsPassingVTightMVAChannelStyle += 1
                if theMuonID >= 5:
                    muonsPassingVVTightMVAChannelStyle += 1
            nMuonsMVALooseChannelStyle.Fill(muonsPassingLooseMVAChannelStyle, etTree.FinalWeighting)
            nMuonsMVAMediumChannelStyle.Fill(muonsPassingMediumMVAChannelStyle, etTree.FinalWeighting)
            nMuonsMVATightChannelStyle.Fill(muonsPassingTightMVAChannelStyle, etTree.FinalWeighting)
            nMuonsMVAVTightChannelStyle.Fill(muonsPassingVTightMVAChannelStyle, etTree.FinalWeighting)
            nMuonsMVAVVTightChannelStyle.Fill(muonsPassingVVTightMVAChannelStyle, etTree.FinalWeighting)
    #let's do the style stuff
    nMuonsNoID.SetLineColor(ROOT.kBlack)
    nMuonsNoIDIdentifiedTaus.SetLineColor(ROOT.kBlack)
    nMuonsNoIDChannelStyle.SetLineColor(ROOT.kBlack)
    
    nMuonsMVALoose.SetLineColor(ROOT.kBlue)
    nMuonsMVALooseIdentifiedTaus.SetLineColor(ROOT.kBlue)
    nMuonsMVALooseChannelStyle.SetLineColor(ROOT.kBlue)

    nMuonsMVAMedium.SetLineColor(ROOT.kRed)
    nMuonsMVAMediumIdentifiedTaus.SetLineColor(ROOT.kRed)
    nMuonsMVAMediumChannelStyle.SetLineColor(ROOT.kRed)

    nMuonsMVATight.SetLineColor(ROOT.kGreen)
    nMuonsMVATightIdentifiedTaus.SetLineColor(ROOT.kGreen)
    nMuonsMVATightChannelStyle.SetLineColor(ROOT.kGreen)

    nMuonsMVAVTight.SetLineColor(ROOT.kCyan)
    nMuonsMVAVTightIdentifiedTaus.SetLineColor(ROOT.kCyan)
    nMuonsMVAVTightChannelStyle.SetLineColor(ROOT.kCyan)
    
    nMuonsMVAVVTight.SetLineColor(ROOT.kPink)
    nMuonsMVAVVTightIdentifiedTaus.SetLineColor(ROOT.kPink)
    nMuonsMVAVVTightChannelStyle.SetLineColor(ROOT.kPink)

    muonRejectionCanvas.cd()
    nMuonsNoID.SetMaximum(max(nMuonsNoID.GetMaximum(), nMuonsMVAVVTight.GetMaximum())*1.1)
    nMuonsNoID.Draw()
    nMuonsMVALoose.Draw('SAME')
    nMuonsMVAMedium.Draw('SAME')
    nMuonsMVATight.Draw('SAME')
    nMuonsMVAVTight.Draw('SAME')
    nMuonsMVAVVTight.Draw('SAME')
    #make the legend
    noTauConditionLegend_muons = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    noTauConditionLegend_muons.AddEntry(nMuonsNoID, "No muon ID Condition", "l")
    noTauConditionLegend_muons.AddEntry(nMuonsMVALoose, "Passes Loose MVA", "l")
    noTauConditionLegend_muons.AddEntry(nMuonsMVAMedium, "Passes Medium MVA", "l")
    noTauConditionLegend_muons.AddEntry(nMuonsMVATight, "Passes Tight MVA", "l")
    noTauConditionLegend_muons.AddEntry(nMuonsMVAVTight, "Passes VTight MVA", "l")
    noTauConditionLegend_muons.AddEntry(nMuonsMVAVVTight, "Passes VVTight MVA", "l")
    noTauConditionLegend_muons.Draw()
    muonRejectionCanvas.Write()

    muonRejectionCanvasIdentifiedTaus.cd()
    nMuonsNoIDIdentifiedTaus.SetMaximum(max(nMuonsNoIDIdentifiedTaus.GetMaximum(), nMuonsMVAVVTightIdentifiedTaus.GetMaximum()) * 1.1)
    nMuonsNoIDIdentifiedTaus.Draw()
    nMuonsMVALooseIdentifiedTaus.Draw('SAME')
    nMuonsMVAMediumIdentifiedTaus.Draw('SAME')
    nMuonsMVATightIdentifiedTaus.Draw('SAME')
    nMuonsMVAVTightIdentifiedTaus.Draw('SAME')
    nMuonsMVAVVTightIdentifiedTaus.Draw('SAME')
    #make the legend
    identifiedTauLegend_muons = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    identifiedTauLegend_muons.AddEntry(nMuonsNoIDIdentifiedTaus, "No muon ID Condition", "l")
    identifiedTauLegend_muons.AddEntry(nMuonsMVALooseIdentifiedTaus, "Passes Loose MVA", "l")
    identifiedTauLegend_muons.AddEntry(nMuonsMVAMediumIdentifiedTaus, "Passes Medium MVA", "l")
    identifiedTauLegend_muons.AddEntry(nMuonsMVATightIdentifiedTaus, "Passes Tight MVA", "l")
    identifiedTauLegend_muons.AddEntry(nMuonsMVAVTightIdentifiedTaus, "Passes VTight MVA", "l")
    identifiedTauLegend_muons.AddEntry(nMuonsMVAVVTightIdentifiedTaus, "Passes VVTight MVA", "l")
    identifiedTauLegend_muons.Draw()
    muonRejectionCanvasIdentifiedTaus.Write()

    muonRejectionCanvasChannelStyle.cd()
    nMuonsNoIDChannelStyle.SetMaximum(max(nMuonsNoIDChannelStyle.GetMaximum(), nMuonsMVAVVTight.GetMaximum()) * 1.1)
    nMuonsNoIDChannelStyle.Draw()
    nMuonsMVALooseChannelStyle.Draw('SAME')
    nMuonsMVAMediumChannelStyle.Draw('SAME')
    nMuonsMVATightChannelStyle.Draw('SAME')
    nMuonsMVAVTightChannelStyle.Draw('SAME')
    nMuonsMVAVVTightChannelStyle.Draw('SAME')
    #make the legend
    channelStyleLegend_muons = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    channelStyleLegend_muons.AddEntry(nMuonsNoIDChannelStyle, "No muon ID Condition", "l")
    channelStyleLegend_muons.AddEntry(nMuonsMVALooseChannelStyle, "Passes Loose MVA", "l")
    channelStyleLegend_muons.AddEntry(nMuonsMVAMediumChannelStyle, "Passes Medium MVA", "l")
    channelStyleLegend_muons.AddEntry(nMuonsMVATightChannelStyle, "Passes Tight MVA", "l")
    channelStyleLegend_muons.AddEntry(nMuonsMVAVTightChannelStyle, "Passes VTight MVA", "l")
    channelStyleLegend_muons.AddEntry(nMuonsMVAVVTightChannelStyle, "Passes VVTight MVA", "l")
    channelStyleLegend_muons.Draw()
    muonRejectionCanvasChannelStyle.Write()

    print('mt channel object multplicity plots...')
    mtDir.cd()

    print('mt channel selection efficiency plots...')
    mtDir.cd()
    
    print('mt channel other lepton rejection plots')
    mtDir.cd()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'Tool for examining gen decay mode bins in the signal')
    parser.add_argument('--inputJson',nargs = '?', required = True, help = 'JSON configuration of input files')
    parser.add_argument('--mass',nargs = '?', default = '1000', choices=['1000','1200','1400','1600','1800','2000','2500','3000','3500','4000','4500'],help='Mass working point to load')

    args = parser.parse_args()

    main(args)
