#script for checking the S/B and S/sqrt(S+B) of various interesting variables
import argparse
import ROOT
import json
import re
import os
import traceback
import math
from bbtautauAnalysisScripts.analysisControlPlots.controlPlotUtilities import processCutJsonToString as processCutJsonToString
from bbtautauAnalysisScripts.analysisControlPlots.controlPlotUtilities import createFileAndSampleDictionaries as createFileAndSampleDictionaries

def main(args):
    ROOT.gStyle.SetOptStat(0)

    #load the samples json
    with open (args.sampleConfig) as jsonFile:
        samplesJson = json.load(jsonFile)

    #load the cut configuration json
    cutJson = None
    if args.cutConfig != None:
        with open(args.cutConfig) as jsonFile:
            cutJson = json.load(jsonFile)

    #create the cuts to filter the histogram down to size
    finalCutString = processCutJsonToString(cutJson, args.additionalSelections)

    #get the configuration for the variables
    with open(args.variableConfig) as jsonFile:
        variableJson = json.load(jsonFile)

    #if we have an RE to specify variables, let's compile it
    theVariableRE = None
    if args.variableSpecification != None:
        theVariableRE = re.compile(args.variableSpecification)

    sampleDict,fileDict = createFileAndSampleDictionaries(samplesJson,args.signalMassPoint)

    #loop over our variables and make histograms
    for variable in variableJson:
        if theVariableRE != None:
            if not theVariableRE.search(variable):
                continue
        print(variable)
        #if we're doing multiple variables, let's try to power through individual errors
        try:
            #draw the distributions
            histoDict = {'MC':{},'Data':{}}
            if finalCutString != '':
                finalCutString = '*('+finalCutString+')'
            for MCSample in sampleDict['MC']:
                try:
                    histoName = MCSample+'_'+variableJson[variable]['name']
                except KeyError:
                    histoName = MCSample+'_'+variable
                sampleDict['MC'][MCSample].Draw(variable+'>>'+histoName+'('+variableJson[variable]['bins']+')',
                                                args.weightingFormula+finalCutString)
                theHisto = ROOT.gDirectory.Get(histoName).Clone()
                histoDict['MC'][MCSample] = theHisto

            #no data histogram needed in this set-up
            #okay, now let's assemble a collection of background and signal histograms
            backgroundHisto = None
            signalHisto = None
            combinedHisto = None
            for histogramName in histoDict['MC']:
                #everything that is a radion histogram is signal
                #everything that is not a radion histogram is a background histogram
                if 'Radion' in histogramName:
                    if signalHisto == None:
                        signalHisto = histoDict['MC'][histogramName].Clone()
                    else:
                        signalHisto.Add(histoDict['MC'][histogramName].Clone())
                else:
                    if backgroundHisto == None:
                        backgroundHisto = histoDict['MC'][histogramName].Clone()
                    else:
                        backgroundHisto.Add(histoDict['MC'][histogramName].Clone())
                #everything gets added to the combined histogram
                if combinedHisto == None:
                    combinedHisto = histoDict['MC'][histogramName].Clone()
                else:
                    combinedHisto.Add(histoDict['MC'][histogramName].Clone())
            #let's go through and create the square root combination histogram
            squareRootCombinedHisto = combinedHisto.Clone()
            for binIndex in range(1,squareRootCombinedHisto.GetNbinsX()+1):
                squareRootCombinedHisto.SetBinContent(binIndex, math.sqrt(squareRootCombinedHisto.GetBinContent(binIndex)))

            #now let's create some derivative histograms
            signalOverBackgroundHisto = signalHisto.Clone()
            signalOverBackgroundHisto.Divide(backgroundHisto)
            
            signalOverSqrtSPlusBHisto = signalHisto.Clone()
            signalOverSqrtSPlusBHisto.Divide(squareRootCombinedHisto)

            #style
            signalOverBackgroundHisto.SetLineColor(ROOT.kRed)
            signalOverBackgroundHisto.SetLineWidth(2)

            signalOverSqrtSPlusBHisto.SetLineColor(ROOT.kBlue)
            signalOverSqrtSPlusBHisto.SetLineWidth(2)

            #draw some stuff
            theCanvas = ROOT.TCanvas("theCanvas","theCanvas")
            theCanvas.SetTickx()
            theCanvas.SetTicky()
            theCanvas.SetLogy()

            signalOverBackgroundHisto.SetMaximum(max(signalOverBackgroundHisto.GetMaximum(),signalOverSqrtSPlusBHisto.GetMaximum())*5)
            #signalOverBackgroundHisto.SetTitle(variable)
            signalOverBackgroundHisto.SetTitle('')
            signalOverBackgroundHisto.Draw("HIST")
            signalOverSqrtSPlusBHisto.Draw("HIST SAME")
            signalOverBackgroundHisto.GetXaxis().SetTitle(variable)

            theLegend = ROOT.TLegend(0.4,0.92,0.6,0.98)
            theLegend.AddEntry(signalOverBackgroundHisto,'S/B','l')
            theLegend.AddEntry(signalOverSqrtSPlusBHisto,'S/#sqrt{S+B}','l')
            theLegend.SetNColumns(2)

            theLegend.Draw()

            #draw the preliminary text
            cmsLatex = ROOT.TLatex()
            cmsLatex.SetTextSize(0.06)
            cmsLatex.SetNDC(True)
            cmsLatex.SetTextFont(61)
            cmsLatex.SetTextAlign(11)
            cmsLatex.DrawLatex(0.1,0.92,"CMS")
            cmsLatex.SetTextFont(52)
            cmsLatex.DrawLatex(0.1+0.08,0.92,"Preliminary")

            cmsLatex.SetTextAlign(31)
            cmsLatex.SetTextFont(42)

            if args.year == '2016':
                lumiText = '16.81 fb^{-1}, 13 TeV'
            elif args.year == '2016APV':
                lumiText = '19.52 fb^{-1}, 13 TeV'
            elif args.year == '2017':
                lumiText = '41.48 fb^{-1}, 13 TeV'
            elif args.year == '2018':
                lumiText = '59.83 fb^{-1}, 13 TeV'
            cmsLatex.DrawLatex(0.9,0.92,lumiText)

            #let's figure out where we're going to keep all of this
            if not os.path.isdir(args.destination):
                os.mkdir(args.destination)
            try:
                outFileName = variableJson[variable]['name']
            except:
                outFileName = variable
            print('')
            print('--------------------')
            theCanvas.SaveAs(args.destination+'/'+outFileName+'.png')
            theCanvas.SaveAs(args.destination+'/'+outFileName+'.pdf')
            print('--------------------')
            print('')

            if args.pause:
                raw_input('Press Enter to Continue....')

            del theCanvas
            del histoDict
        except Exception:
            print("There was a problem!")
            traceback.print_exc()
            continue
            
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate the predicted signal extraction powers of various bins of control variables')
    parser.add_argument('-v',
                        '--variableConfig',
                        nargs='?',
                        help='JSON with variable information to be drawn',
                        required=True)
    parser.add_argument('--variableSpecification',
                        nargs='?',
                        help='RE to specify variables to be drawn from the list')
    parser.add_argument('--sampleConfig',
                        nargs='?',
                        help='location of the configuration that points out specific samples',
                        required=True)
    parser.add_argument('--batchMode',
                        action='store_true',
                        help='run in batch mode (no attempt to draw to the screen)')
    parser.add_argument('--cutConfig',
                        nargs='?',
                        help = 'configuration to take standard cutting setups from')
    parser.add_argument('--additionalSelections',
                        nargs='+',
                        help = 'additional selections on top of standard ones')
    parser.add_argument('--year',
                        nargs='?',
                        choices = ['2016','2016APV','2017','2018'],
                        default = '2016',
                        help = 'Year to base lumi calculation off of')
    parser.add_argument('--destination',
                        nargs='?',
                        default='signalControlPlots',
                        help='directory to store plots in')
    parser.add_argument('--pause',
                        action = 'store_true',
                        help='pause after each plot drawn')
    parser.add_argument('--signalMassPoint',
                        nargs='?',
                        choices = ['1000','1200','1400','1600','1800','2000','2500','3000','3500','4000','4500'],
                        default = '1000',
                        help='Signal mass point to include in the control plots')
    parser.add_argument('--weightingFormula',
                        nargs='?',
                        help='Formula to weight the final histograms by',
                        default = 'FinalWeighting')
    args = parser.parse_args()
    
    if args.batchMode:
        ROOT.gROOT.SetBatch(ROOT.kTRUE)
    
    main(args)
