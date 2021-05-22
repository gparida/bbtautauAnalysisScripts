#!/usr/bin/env python

#script for going over our miniAOD ntuple and matching boostedTaus to gen taus

import ROOT
import argparse
from tqdm import tqdm

#unfortunately, due to the complicated nature of matching requirements
#we can't quite just do this from the tree commands
def main(args):
    
    ROOT.gStyle.SetOptStat(0)

    theFile = ROOT.TFile(args.inputFile)
    theTree = theFile.plots.Get('Events')    

    #for each event/higgs we look at each gen tau
    #then for each gen tau, we add to the denominator for the higgs pt
    #then we look at the boosted tau collection
    #and try to find a match within whatever cone-size
    #same thing for HPS taus
    
    overallTaus = 0.0
    reconstructedBoostedTaus = 0.0
    reconstructedTaus = 0.0

    genTauBasePlot = ROOT.TH1F('genTauBasePlot','genTauBasePlot',13,350,1000)
    boostedTauEfficiencyPlot = ROOT.TH1F('boostedTauEfficiency','boostedTauEfficiency',13,350,1000)
    tauEfficiencyPlot = ROOT.TH1F('tauEfficiency','tauEfficiency',13,350,1000)

    for entry in tqdm(range(theTree.GetEntries())):
        theTree.GetEntry(entry)
        alreadyUsedBoostedTaus = []
        alreadyUsedTaus = []
       
        higgsToTausVector = ROOT.TLorentzVector()
        higgsToTausVector.SetPtEtaPhiM(theTree.genHiggs_pt[0],
                                       theTree.genHiggs_eta[0],
                                       theTree.genHiggs_phi[0],
                                       theTree.genHiggs_m[0])

        #if for some reason, we have not have two gen taus, let's get rid of the event
        if theTree.ngenTau != 2:
            continue
            
        #loop over the gen taus
        for genTauEntry in range(theTree.ngenTau):
            overallTaus += 1.0
            genTauVector = ROOT.TLorentzVector()
            genTauVector.SetPtEtaPhiM(theTree.genTau_pt[genTauEntry],
                                      theTree.genTau_eta[genTauEntry],
                                      theTree.genTau_phi[genTauEntry],
                                      theTree.genTau_m[genTauEntry])
            genTauBasePlot.Fill(higgsToTausVector.Pt())

            #let's try to find a boosted tau that matches to this gen tau
            for boostedTauEntry in range(theTree.nboostedTau):
                boostedTauVector = ROOT.TLorentzVector()
                boostedTauVector.SetPtEtaPhiM(theTree.boostedTau_pt[boostedTauEntry],
                                              theTree.boostedTau_eta[boostedTauEntry],
                                              theTree.boostedTau_phi[boostedTauEntry],
                                              theTree.boostedTau_m[boostedTauEntry])
                if genTauVector.DeltaR(boostedTauVector) <= args.coneSize and boostedTauEntry not in alreadyUsedBoostedTaus:
                    reconstructedBoostedTaus += 1.0
                    boostedTauEfficiencyPlot.Fill(higgsToTausVector.Pt())
                    alreadyUsedBoostedTaus.append(boostedTauEntry)
            for tauEntry in range(theTree.ntau):
                tauVector = ROOT.TLorentzVector()
                tauVector.SetPtEtaPhiM(theTree.tau_pt[tauEntry],
                                              theTree.tau_eta[tauEntry],
                                              theTree.tau_phi[tauEntry],
                                              theTree.tau_m[tauEntry])
                if genTauVector.DeltaR(tauVector) <= args.coneSize and tauEntry not in alreadyUsedTaus:
                    reconstructedTaus += 1.0
                    tauEfficiencyPlot.Fill(higgsToTausVector.Pt())
                    alreadyUsedTaus.append(tauEntry)
    print("boosted tau reconstruction percentage: {percentage:3.2f}%".format(percentage = ((reconstructedBoostedTaus/overallTaus)*100)))
    print("tau reconstruction percentage: {percentage:3.2f}%".format(percentage = ((reconstructedTaus/overallTaus)*100)))

    theCanvas = ROOT.TCanvas('c1','c1')
    
    boostedTauEfficiencyPlot.Sumw2()
    tauEfficiencyPlot.Sumw2()

    boostedTauEfficiencyPlot.Divide(genTauBasePlot)
    tauEfficiencyPlot.Divide(genTauBasePlot)

    boostedTauEfficiencyPlot.SetMarkerStyle(20)
    boostedTauEfficiencyPlot.SetMarkerColor(ROOT.kBlack)
    
    tauEfficiencyPlot.SetMarkerStyle(22)
    tauEfficiencyPlot.SetMarkerColor(ROOT.kRed)
    tauEfficiencyPlot.SetLineColor(ROOT.kRed)
    
    boostedTauEfficiencyPlot.SetMaximum(1.5)
    boostedTauEfficiencyPlot.SetMinimum(0.0)
    boostedTauEfficiencyPlot.Draw('e1')
    tauEfficiencyPlot.Draw('e1 SAME')

    boostedTauEfficiencyPlot.GetXaxis().SetTitle('p_{t}^{H}')
    boostedTauEfficiencyPlot.GetXaxis().SetTitleOffset(1.1)
    boostedTauEfficiencyPlot.GetYaxis().SetTitle('Reconstruction efficiency for cone size {coneSize}'.format(coneSize=args.coneSize))
    boostedTauEfficiencyPlot.SetTitle('')

    theLegend = ROOT.TLegend(0.5,0.7,0.9,0.9)
    theLegend.AddEntry(boostedTauEfficiencyPlot,'Boosted Reconstruction','pl')
    theLegend.AddEntry(tauEfficiencyPlot,'Standard Reconstruction','pl')
    theLegend.Draw()

    cmsLatex = ROOT.TLatex()
    cmsLatex.SetTextSize(0.04)
    cmsLatex.SetNDC(True)
    cmsLatex.SetTextFont(61)
    cmsLatex.SetTextAlign(11)
    cmsLatex.DrawLatex(0.1,0.92,"CMS")
    cmsLatex.SetTextFont(52)
    cmsLatex.DrawLatex(0.1+0.08,0.92,"Preliminary")

    raw_input("Press Enter to continue...")
    theCanvas.SaveAs('reconstructionEfficiency_coneSize_'+str(args.coneSize).replace('.','p')+'.png')
    theCanvas.SaveAs('reconstructionEfficiency_coneSize_'+str(args.coneSize).replace('.','p')+'.pdf')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='purpose built script for examining the miniAOD ntuple output')
    parser.add_argument('--inputFile',help='name of the input file',required=True)
    parser.add_argument('--coneSize',help='size of the cone to try to match taus to gen tau',type=float,default=0.1)
    args = parser.parse_args()
    main(args)
