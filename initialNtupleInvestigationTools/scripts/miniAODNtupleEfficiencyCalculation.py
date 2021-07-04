#!/usr/bin/env python

#script for going over our miniAOD ntuple and matching boostedTaus to gen taus

import ROOT
import argparse
from tqdm import tqdm

#let's figure out what our actual decay mode was, so we can use it later
def determineGenTauDecayMode(daughterPDGCodes):
    if 13 in daughterPDGCodes or -13 in daughterPDGCodes:
        return 'm'
    elif 12 in daughterPDGCodes or -12 in daughterPDGCodes:
        return 'e'
    else:
        return 't'

def determineDecayChannel(theTree):
    decayModes = []
    decayModes.append(determineGenTauDecayMode(theTree.genTau_daughterParticles_pdgCodes[0]))
    decayModes.append(determineGenTauDecayMode(theTree.genTau_daughterParticles_pdgCodes[1]))
    decayModes.sort()
    return ''.join(x for x in decayModes)

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
    
    boostedTauPtResolutionPlot = ROOT.TH1F('boostedTauResolutionPlot','boostedTauResolutionPlot',50,0.0,2.0)
    tauPtResolutionPlot = ROOT.TH1F('tauResolutionPlot','tauResolutionPlot',50,0.0,2.0)

    boostedTauEtaResolutionPlot = ROOT.TH1F('boostedTauEtaResolutionPlot','boostedTauResolutionPlot',50,0.95,1.05)
    tauEtaResolutionPlot = ROOT.TH1F('tauEtaResolutionPlot','tauResolutionPlot',50,0.95,1.05)

    efficiencyPlots = {
        'overall':{
            'genTauBasePlot': ROOT.TH1F('genTauBasePlot','genTauBasePlot',13,350,1000),
            'boostedTauEfficiencyPlot': ROOT.TH1F('boostedTauEfficiency','boostedTauEfficiency',13,350,1000),
            'tauEfficiencyPlot': ROOT.TH1F('tauEfficiency','tauEfficiency',13,350,1000),
        },
        'et':{
            'genTauBasePlot': ROOT.TH1F('etGenTauBasePlot','etGenTauBasePlot',13,350,1000),
            'boostedTauEfficiencyPlot': ROOT.TH1F('etBoostedTauEfficiency','etBoostedTauEfficiency',13,350,1000),
            'tauEfficiencyPlot': ROOT.TH1F('etTauEfficiency','etTauEfficiency',13,350,1000),
        },
        'mt':{
            'genTauBasePlot': ROOT.TH1F('mtGenTauBasePlot','mtGenTauBasePlot',13,350,1000),
            'boostedTauEfficiencyPlot': ROOT.TH1F('mtBoostedTauEfficiency','mtBoostedTauEfficiency',13,350,1000),
            'tauEfficiencyPlot': ROOT.TH1F('mtTauEfficiency','mtTauEfficiency',13,350,1000),
        },
        'tt':{
            'genTauBasePlot': ROOT.TH1F('ttGenTauBasePlot','ttGenTauBasePlot',13,350,1000),
            'boostedTauEfficiencyPlot': ROOT.TH1F('ttBoostedTauEfficiency','ttBoostedTauEfficiency',13,350,1000),
            'tauEfficiencyPlot': ROOT.TH1F('ttTauEfficiency','ttTauEfficiency',13,350,1000),
        },
    }


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
            
        
        channel = determineDecayChannel(theTree)
        if 't' not in channel:
            continue # we want to consider ones with at least one hadronic tau

        #loop over the gen taus
        for genTauEntry in range(theTree.ngenTau):
            #check if this gen tau decayed hadronically.
            #if it didn't we don't need to consider it in the efficiency calculation
            #that is, there's nothing that would have matched to it anyways
            if (determineGenTauDecayMode(theTree.genTau_daughterParticles_pdgCodes[genTauEntry]) == 'm'
                                        or determineGenTauDecayMode(theTree.genTau_daughterParticles_pdgCodes[genTauEntry]) == 'e'):
                continue
                
            overallTaus += 1.0
            genTauVector = ROOT.TLorentzVector()
            genTauVector.SetPtEtaPhiM(theTree.genTau_pt[genTauEntry],
                                      theTree.genTau_eta[genTauEntry],
                                      theTree.genTau_phi[genTauEntry],
                                      theTree.genTau_m[genTauEntry])
            efficiencyPlots['overall']['genTauBasePlot'].Fill(higgsToTausVector.Pt())
            efficiencyPlots[channel]['genTauBasePlot'].Fill(higgsToTausVector.Pt())

            #let's try to find a boosted tau that matches to this gen tau
            #let's try matching it to the closest boosted tau too.
            finalBoostedTauCandidate = -1
            finalBoostedTauCandidateDR = 0.0
            finalBoostedTauCandidateVector = None
            for boostedTauEntry in range(theTree.nboostedTau):
                boostedTauVector = ROOT.TLorentzVector()
                boostedTauVector.SetPtEtaPhiM(theTree.boostedTau_pt[boostedTauEntry],
                                              theTree.boostedTau_eta[boostedTauEntry],
                                              theTree.boostedTau_phi[boostedTauEntry],
                                              theTree.boostedTau_m[boostedTauEntry])
                if (genTauVector.DeltaR(boostedTauVector) <= args.coneSize 
                    and boostedTauVector.Pt() > 20
                    and abs(boostedTauVector.Eta()) < 2.3
                    and theTree.boostedTau_passesVLooseID[boostedTauEntry]
                    and boostedTauEntry not in alreadyUsedBoostedTaus
                    and (finalBoostedTauCandidate < 0 
                         or genTauVector.DeltaR(boostedTauVector) < finalBoostedTauCandidateDR)):
                    finalBoostedTauCandidate = boostedTauEntry
                    finalBoostedTauCandidateDR = genTauVector.DeltaR(boostedTauVector)
                    finalBoostedTauCandidateVector = boostedTauVector 
            #after the boosted tau selection, if we found one that is matching, let's fill some stuff
            if finalBoostedTauCandidate > 0:
                reconstructedBoostedTaus += 1.0
                efficiencyPlots['overall']['boostedTauEfficiencyPlot'].Fill(higgsToTausVector.Pt())
                efficiencyPlots[channel]['boostedTauEfficiencyPlot'].Fill(higgsToTausVector.Pt())
                boostedTauPtResolutionPlot.Fill(finalBoostedTauCandidateVector.Pt()/genTauVector.Pt())
                boostedTauEtaResolutionPlot.Fill(finalBoostedTauCandidateVector.Eta()/genTauVector.Eta())
                alreadyUsedBoostedTaus.append(finalBoostedTauCandidate)
                
            finalTauCandidate = -1
            finalTauCandidateDR = 0.0
            finalTauCandidateVector = None
            for tauEntry in range(theTree.ntau):
                tauVector = ROOT.TLorentzVector()
                tauVector.SetPtEtaPhiM(theTree.tau_pt[tauEntry],
                                              theTree.tau_eta[tauEntry],
                                              theTree.tau_phi[tauEntry],
                                              theTree.tau_m[tauEntry])
                if (genTauVector.DeltaR(tauVector) <= args.coneSize 
                    and tauVector.Pt() > 20
                    and abs(tauVector.Eta()) < 2.3
                    and theTree.tau_passesVLooseID[tauEntry]
                    and tauEntry not in alreadyUsedTaus
                    and (finalTauCandidate < 0
                         or genTauVector.DeltaR(tauVector) < finalTauCandidateDR)):
                    finalTauCandidate = tauEntry
                    finalTauCandidateDR = genTauVector.DeltaR(tauVector)
                    finalTauCandidateVector = tauVector
            if finalTauCandidate > 0:
                reconstructedTaus += 1.0
                efficiencyPlots['overall']['tauEfficiencyPlot'].Fill(higgsToTausVector.Pt())
                efficiencyPlots[channel]['tauEfficiencyPlot'].Fill(higgsToTausVector.Pt())
                tauPtResolutionPlot.Fill(finalTauCandidateVector.Pt()/genTauVector.Pt())
                tauEtaResolutionPlot.Fill(finalTauCandidateVector.Eta()/genTauVector.Eta())
                alreadyUsedTaus.append(finalTauCandidate)
    print("boosted tau reconstruction percentage: {percentage:3.2f}%".format(percentage = ((reconstructedBoostedTaus/overallTaus)*100)))
    print("tau reconstruction percentage: {percentage:3.2f}%".format(percentage = ((reconstructedTaus/overallTaus)*100)))

    for key in efficiencyPlots:
        theCanvas = ROOT.TCanvas('c1','c1')

        efficiencyPlots[key]['boostedTauEfficiencyPlot'].Sumw2()
        efficiencyPlots[key]['tauEfficiencyPlot'].Sumw2()

        efficiencyPlots[key]['boostedTauEfficiencyPlot'].Divide(efficiencyPlots[key]['genTauBasePlot'])
        efficiencyPlots[key]['tauEfficiencyPlot'].Divide(efficiencyPlots[key]['genTauBasePlot'])

        efficiencyPlots[key]['boostedTauEfficiencyPlot'].SetMarkerStyle(20)
        efficiencyPlots[key]['boostedTauEfficiencyPlot'].SetMarkerColor(ROOT.kBlack)

        efficiencyPlots[key]['tauEfficiencyPlot'].SetMarkerStyle(22)
        efficiencyPlots[key]['tauEfficiencyPlot'].SetMarkerColor(ROOT.kRed)
        efficiencyPlots[key]['tauEfficiencyPlot'].SetLineColor(ROOT.kRed)

        efficiencyPlots[key]['boostedTauEfficiencyPlot'].SetMaximum(1.5)
        efficiencyPlots[key]['boostedTauEfficiencyPlot'].SetMinimum(0.0)
        efficiencyPlots[key]['boostedTauEfficiencyPlot'].Draw('e1')
        efficiencyPlots[key]['tauEfficiencyPlot'].Draw('e1 SAME')

        efficiencyPlots[key]['boostedTauEfficiencyPlot'].GetXaxis().SetTitle('p_{t}^{H}')
        efficiencyPlots[key]['boostedTauEfficiencyPlot'].GetXaxis().SetTitleOffset(1.1)
        efficiencyPlots[key]['boostedTauEfficiencyPlot'].GetYaxis().SetTitle('Reconstruction efficiency for cone size {coneSize}'.format(coneSize=args.coneSize))
        efficiencyPlots[key]['boostedTauEfficiencyPlot'].SetTitle('')

        theLegend = ROOT.TLegend(0.5,0.7,0.9,0.9)
        theLegend.AddEntry(efficiencyPlots[key]['boostedTauEfficiencyPlot'],'Boosted Reconstruction','pl')
        theLegend.AddEntry(efficiencyPlots[key]['tauEfficiencyPlot'],'Standard Reconstruction','pl')
        theLegend.Draw()

        cmsLatex = ROOT.TLatex()
        cmsLatex.SetTextSize(0.04)
        cmsLatex.SetNDC(True)
        cmsLatex.SetTextFont(61)
        cmsLatex.SetTextAlign(11)
        cmsLatex.DrawLatex(0.1,0.92,"CMS")
        cmsLatex.SetTextFont(52)
        cmsLatex.DrawLatex(0.1+0.08,0.92,"Preliminary")

        channelLatexString = ''
        if key == 'et':
            channelLatexString = '#tau_{e}#tau_{h}'
        elif key == 'mt':
            channelLatexString = '#tau_{#mu}#tau_{h}'
        elif key == 'tt':
            channelLatexString = '#tau_{h}#tau_{h}'

        channelLatex = ROOT.TLatex()
        channelLatex.SetTextSize(0.06)
        channelLatex.SetNDC(True)
        channelLatex.DrawLatex(0.4,0.75,channelLatexString)

        if args.pause:
            raw_input("Press Enter to continue...")
        theCanvas.SaveAs('reconstructionEfficiency_'+key+'_coneSize_'+str(args.coneSize).replace('.','p')+'.png')
        theCanvas.SaveAs('reconstructionEfficiency_'+key+'_coneSize_'+str(args.coneSize).replace('.','p')+'.pdf')

    ptResolutionCanvas = ROOT.TCanvas('cReso','cReso')
    
    boostedTauPtResolutionPlot.SetLineColor(ROOT.kBlue)
    boostedTauPtResolutionPlot.SetMarkerStyle(20)
    boostedTauPtResolutionPlot.SetMarkerColor(ROOT.kBlack)
    tauPtResolutionPlot.SetLineColor(ROOT.kRed)
    tauPtResolutionPlot.SetMarkerStyle(22)
    tauPtResolutionPlot.SetMarkerColor(ROOT.kRed)

    boostedTauEtaResolutionPlot.SetLineColor(ROOT.kBlue)
    boostedTauEtaResolutionPlot.SetMarkerStyle(20)
    boostedTauEtaResolutionPlot.SetMarkerColor(ROOT.kBlack)
    tauEtaResolutionPlot.SetLineColor(ROOT.kRed)
    tauEtaResolutionPlot.SetMarkerStyle(22)
    tauEtaResolutionPlot.SetMarkerColor(ROOT.kRed)

    resoLegend = ROOT.TLegend(0.55,0.7,0.9,0.9)
    resoLegend.AddEntry(boostedTauPtResolutionPlot,'Boosted Reconstruction','pl')
    resoLegend.AddEntry(tauPtResolutionPlot,'Standard Reconstruction','pl')
    resoLegend.Draw()

    boostedTauPtResolutionPlot.Scale(1.0/boostedTauPtResolutionPlot.Integral())
    boostedTauPtResolutionPlot.Draw()
    boostedTauPtResolutionPlot.SetTitle("Tau p_{t} Resolution \n (For Taus Matched to a Gen Tau)")
    boostedTauPtResolutionPlot.GetXaxis().SetTitle("Reconstructed p_{t} / Gen p_{t}")
    boostedTauPtResolutionPlot.GetYaxis().SetTitle('Fraction')
    tauPtResolutionPlot.Scale(1.0/tauPtResolutionPlot.Integral())
    tauPtResolutionPlot.Draw("SAME")
    resoLegend.Draw()
    
    if args.pause:
        raw_input("Press Enter to continue...")
    ptResolutionCanvas.SaveAs('ptResolution_coneSize_'+str(args.coneSize).replace('.','p')+'.png')
    ptResolutionCanvas.SaveAs('ptResolution_coneSize_'+str(args.coneSize).replace('.','p')+'.pdf')

    boostedTauEtaResolutionPlot.Scale(1.0/boostedTauEtaResolutionPlot.Integral())
    boostedTauEtaResolutionPlot.Draw()
    boostedTauEtaResolutionPlot.SetTitle("Tau #eta Resolution \n (For Taus Matched to a Gen Tau)")
    boostedTauEtaResolutionPlot.GetXaxis().SetTitle("Reconstructed #eta / Gen #eta")
    boostedTauEtaResolutionPlot.GetYaxis().SetTitle('Fraction')
    tauEtaResolutionPlot.Scale(1.0/tauEtaResolutionPlot.Integral())
    tauEtaResolutionPlot.Draw("SAME")
    resoLegend.Draw()
    
    if args.pause:
        raw_input("Press Enter to continue...")

    ptResolutionCanvas.SaveAs('etaResolution_coneSize_'+str(args.coneSize).replace('.','p')+'.png')
    ptResolutionCanvas.SaveAs('etaResolution_coneSize_'+str(args.coneSize).replace('.','p')+'.pdf')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='purpose built script for examining the miniAOD ntuple output')
    parser.add_argument('--inputFile',help='name of the input file',required=True)
    parser.add_argument('--coneSize',help='size of the cone to try to match taus to gen tau',type=float,default=0.1)
    parser.add_argument('--pause',help='pause after each plot',action='store_true')
    args = parser.parse_args()
    main(args)
