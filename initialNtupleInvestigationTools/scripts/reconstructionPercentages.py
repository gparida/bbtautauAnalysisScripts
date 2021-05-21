#!/usr/bin/env python

#quick script to determine the overall gen contents of reconstructed taus at various places in the spectrum

import ROOT
import argparse

parser = argparse.ArgumentParser(description='Generate plots detailing boosted tau constents')
parser.add_argument('--resultsFileName',help='name of the resulting file',default='results.root')
parser.add_argument('--binningVariable',help='variable to bin the contents plot in',default='boostedTau_pt')
parser.add_argument('--bins',help='overall binning to use. Format: nbins,low,high',default='25,0,500')
parser.add_argument('--conditions',help='cuts to apply to the plots',default='1')
parser.add_argument('--boostedOrHPS',nargs='?',help='consider the genpartflav from boosted or HPS taus',choices=['boostedTau','Tau'],default='boostedTau')
parser.add_argument('--plotTitle',help='Title of the plot',default='')
parser.add_argument('--xTitle',help='Title of the x-axis',default='boostedTauPt')
parser.add_argument('--yTitle',help='Title of the y-axis',default='Fraction')

args = parser.parse_args()

#let's grab the files we're concerned about and start taking a look
fileLocation='/data/aloeliger/preliminarybbtautauNtuples/'

theChain = ROOT.TChain('Events')
theChain.Add(fileLocation+'NanoAOD_M1000.root')
theChain.Add(fileLocation+'NanoAOD_M2000.root')
theChain.Add(fileLocation+'NanoAOD_M4000.root')

#let's open a results file too
resultsFile = ROOT.TFile(args.resultsFileName,'RECREATE')


#let's start generating the various plots we're going to want
theChain.Draw(args.binningVariable+'>>overallPlot('+args.bins+')',
              '('+args.conditions+')')
overallPlot = ROOT.gDirectory.Get('overallPlot').Clone()

theChain.Draw(args.binningVariable+'>>unmatchedDecayPlot('+args.bins+')',
              '('+args.conditions+')'+'*('+args.boostedOrHPS+'_genPartFlav == 0)')
unmatchedDecayPlot = ROOT.gDirectory.Get('unmatchedDecayPlot').Clone()

theChain.Draw(args.binningVariable+'>>promptElectronPlot('+args.bins+')',
              '('+args.conditions+')'+'*('+args.boostedOrHPS+'_genPartFlav == 1)')
promptElectronPlot = ROOT.gDirectory.Get('promptElectronPlot').Clone()

theChain.Draw(args.binningVariable+'>>promptMuonPlot('+args.bins+')',
              '('+args.conditions+')'+'*('+args.boostedOrHPS+'_genPartFlav == 2)')
promptMuonPlot = ROOT.gDirectory.Get('promptMuonPlot').Clone()

theChain.Draw(args.binningVariable+'>>tauToEPlot('+args.bins+')',
              '('+args.conditions+')'+'*('+args.boostedOrHPS+'_genPartFlav == 3)')
tauToEPlot = ROOT.gDirectory.Get('tauToEPlot').Clone()

theChain.Draw(args.binningVariable+'>>tauToMuPlot('+args.bins+')',
              '('+args.conditions+')'+'*('+args.boostedOrHPS+'_genPartFlav == 4)')
tauToMuPlot = ROOT.gDirectory.Get('tauToMuPlot').Clone()

theChain.Draw(args.binningVariable+'>>genuineTauPlot('+args.bins+')',
              '('+args.conditions+')'+'*('+args.boostedOrHPS+'_genPartFlav == 5)')
genuineTauPlot = ROOT.gDirectory.Get('genuineTauPlot').Clone()

overallPlot.Sumw2()
unmatchedDecayPlot.Sumw2()
promptElectronPlot.Sumw2()
promptMuonPlot.Sumw2()
tauToEPlot.Sumw2()
tauToMuPlot.Sumw2()
genuineTauPlot.Sumw2()

unmatchedDecayPlot.Divide(overallPlot)
promptElectronPlot.Divide(overallPlot)
promptMuonPlot.Divide(overallPlot)
tauToEPlot.Divide(overallPlot)
tauToMuPlot.Divide(overallPlot)
genuineTauPlot.Divide(overallPlot)

unmatchedDecayErrorPlot = unmatchedDecayPlot.Clone()
promptElectronErrorPlot = promptElectronPlot.Clone()
promptMuonErrorPlot = promptMuonPlot.Clone()
tauToEErrorPlot = tauToEPlot.Clone()
tauToMuErrorPlot = tauToMuPlot.Clone()
genuineTauErrorPlot = genuineTauPlot.Clone()

unmatchedDecayErrorPlot.SetFillStyle(3001)
promptElectronErrorPlot.SetFillStyle(3001)
promptMuonErrorPlot.SetFillStyle(3001)
tauToEErrorPlot.SetFillStyle(3001)
tauToMuErrorPlot.SetFillStyle(3001)
genuineTauErrorPlot.SetFillStyle(3001)

unmatchedDecayErrorPlot.SetFillColor(15)
promptElectronErrorPlot.SetFillColor(15)
promptMuonErrorPlot.SetFillColor(15)
tauToEErrorPlot.SetFillColor(15)
tauToMuErrorPlot.SetFillColor(15)
genuineTauErrorPlot.SetFillColor(15)

unmatchedDecayPlot.SetFillColor(ROOT.kRed)
#unmatchedDecayPlot.SetLineColor(0)

promptElectronPlot.SetFillColor(ROOT.kBlue)
#promptElectronPlot.SetLineColor(0)

promptMuonPlot.SetFillColor(ROOT.kGreen)
#promptMuonPlot.SetLineColor(0)

tauToEPlot.SetFillColor(ROOT.kCyan)
#tauToEPlot.SetLineColor(0)

tauToMuPlot.SetFillColor(ROOT.kPink+9)
#tauToMuPlot.SetLineColor(0)

genuineTauPlot.SetFillColor(ROOT.kOrange)
#genuineTauPlot.SetLineColor(0)

theStack = ROOT.THStack()
theStack.Add(genuineTauPlot,'HIST')
theStack.Add(tauToEPlot,'HIST')
theStack.Add(tauToMuPlot,'HIST')
theStack.Add(promptElectronPlot,'HIST')
theStack.Add(promptMuonPlot,'HIST')
theStack.Add(unmatchedDecayPlot,'HIST')

theStack.SetMaximum(1.3)

theStack.Draw()

theStack.GetYaxis().SetTitle(args.yTitle)
theStack.GetXaxis().SetTitle(args.xTitle)
theStack.SetTitle(args.plotTitle)

errorStack = ROOT.THStack()
errorStack.Add(genuineTauErrorPlot,'e2')
errorStack.Add(tauToEErrorPlot,'e2')
errorStack.Add(tauToMuErrorPlot,'e2')
errorStack.Add(promptElectronErrorPlot,'e2')
errorStack.Add(promptMuonErrorPlot,'e2')
errorStack.Add(unmatchedDecayErrorPlot,'e2')

#errorStack.Draw('SAME e2')

theLegend=ROOT.TLegend(0.12,0.7,0.88,0.88)
theLegend.AddEntry(genuineTauPlot,"Real Hadronic Taus",'f')
theLegend.AddEntry(tauToEPlot,'Tau to e decay','f')
theLegend.AddEntry(tauToMuPlot,'Tau to mu decay','f')
theLegend.AddEntry(promptElectronPlot,'Prompt e decay','f')
theLegend.AddEntry(promptMuonPlot,'Prompt muon decay','f')
theLegend.AddEntry(unmatchedDecayPlot,'Other or unmatched decays','f')
theLegend.SetNColumns(3)
theLegend.SetLineColor(0)
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
theStack.Write()
resultsFile.Write()
resultsFile.Close()
