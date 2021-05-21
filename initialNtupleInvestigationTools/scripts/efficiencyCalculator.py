#!/usr/bin/env python

#quick script to perform (boosted) tau reconstruction/identification efficiency checks

import ROOT
import argparse

ROOT.gStyle.SetOptStat(0)

parser = argparse.ArgumentParser(description = 'Generate reco efficiency plots for (boosted) tau')
parser.add_argument('--resultsFileName',help='name of the resulting file',default='results.root')
parser.add_argument('-b','--binningVariable',help='variable to bin the efficiency in',default = 'boostedTau_pt')
parser.add_argument('--bins',help='bins for the binning variable',default = '25,0,500')
parser.add_argument('-d','--denominatorConditions',help='Conditions to apply to denominator and numerator',default='1')
parser.add_argument('-n','--numeratorConditions',help='additional conditions to apply to the numerator',default='1')
parser.add_argument('--plotTitle',help='Title of the plot',default='Boosted Tau Efficiency')
parser.add_argument('--xTitle',help='Title of the x-axis',default='boostedTau Pt')
parser.add_argument('--yTitle',help='Title of the y-axis',default='Efficiency')

args=parser.parse_args()

fileLocation='/data/aloeliger/preliminarybbtautauNtuples/'

theChain = ROOT.TChain('Events')
theChain.Add(fileLocation+'NanoAOD_M1000.root')
theChain.Add(fileLocation+'NanoAOD_M2000.root')
theChain.Add(fileLocation+'NanoAOD_M4000.root')

resultsFile = ROOT.TFile(args.resultsFileName,'RECREATE')

theChain.Draw(args.binningVariable+'>>denominatorHist('+args.bins+')',
              '('+args.denominatorConditions+')')
denominatorHist = ROOT.gDirectory.Get('denominatorHist').Clone()

theChain.Draw(args.binningVariable+'>>numeratorHist('+args.bins+')',
              '('+args.denominatorConditions+')*('+args.numeratorConditions+')')
numeratorHist = ROOT.gDirectory.Get('numeratorHist').Clone()

denominatorHist.Sumw2()
numeratorHist.Sumw2()

numeratorHist.Divide(denominatorHist)

numeratorHist.SetMarkerStyle(20)
numeratorHist.SetMarkerColor(ROOT.kBlack)
numeratorHist.Draw('e1')

numeratorHist.SetTitle(args.plotTitle)
numeratorHist.GetYaxis().SetTitle(args.yTitle)
numeratorHist.GetXaxis().SetTitle(args.xTitle)
numeratorHist.GetYaxis().SetRangeUser(0.0,1.0)

cmsLatex = ROOT.TLatex()
cmsLatex.SetTextSize(0.04)
cmsLatex.SetNDC(True)
cmsLatex.SetTextFont(61)
cmsLatex.SetTextAlign(11)
cmsLatex.DrawLatex(0.1,0.92,"CMS")
cmsLatex.SetTextFont(52)
cmsLatex.DrawLatex(0.1+0.08,0.92,"Preliminary")

raw_input('Press Enter to continue...')

resultsFile.Write()
resultsFile.Close()
