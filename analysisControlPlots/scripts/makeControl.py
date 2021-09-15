#Script for bbtautau analysis control plot making
import argparse
import ROOT
import json
import re
import os
import traceback
from bbtautauAnalysisScripts.analysisCore.histogramUtilities import MakeStackErrors as MakeStackErrors
from bbtautauAnalysisScripts.analysisCore.histogramUtilities import MakeRatioHistograms as MakeRatioHistograms
from bbtautauAnalysisScripts.analysisControlPlots.controlPlotUtilities import processCutJsonToString as processCutJsonToString
from bbtautauAnalysisScripts.analysisControlPlots.controlPlotUtilities import createFileAndSampleDictionaries as createFileAndSampleDictionaries

def checkAndAdd(sample, handles, dictionaryKey, dictionaryOfHistograms, finalHistogramDictionary):
    for handleToCheckAgainst in handles:
        if handleToCheckAgainst in sample:
            if finalHistogramDictionary[dictionaryKey] == None:
                finalHistogramDictionary[dictionaryKey] = dictionaryOfHistograms['MC'][sample].Clone()
            else:
                finalHistogramDictionary[dictionaryKey].Add(dictionaryOfHistograms['MC'][sample].Clone())
    
#takes a dictionary of histgorams plotted in the variable
#And returns a dictionary of histgrams added together in a style we want to use
#these histograms are:
#DY
#W+Jets
#QCD
#TT
#Other (Includes signal)
#Signal (for line drawing later)
def produceFinalHistograms(dictionaryOfHistograms):
    finalHistogramDictionary = {}
    finalHistogramDictionary['QCD'] = None
    finalHistogramDictionary['WJets'] = None
    finalHistogramDictionary['TT'] = None
    finalHistogramDictionary['DY'] = None
    finalHistogramDictionary['Other'] = None
    finalHistogramDictionary['Signal'] = None
    finalHistogramDictionary['Data'] = None
    for sample in dictionaryOfHistograms['MC']:
        checkAndAdd(sample, ['TTTo'], 'TT', dictionaryOfHistograms, finalHistogramDictionary)
        checkAndAdd(sample, ['QCD'], 'QCD', dictionaryOfHistograms, finalHistogramDictionary)
        checkAndAdd(sample, ['WJets'], 'WJets', dictionaryOfHistograms, finalHistogramDictionary)
        checkAndAdd(sample, ['DY'], 'DY', dictionaryOfHistograms, finalHistogramDictionary)
        checkAndAdd(sample, ['WW', 'WZ', 'ZZ', 'ST_', 'Radion'], 'Other', dictionaryOfHistograms, finalHistogramDictionary)
        checkAndAdd(sample, ['Radion'], 'Signal', dictionaryOfHistograms, finalHistogramDictionary)
    finalHistogramDictionary['Data'] = dictionaryOfHistograms['Data']['Data']

    finalHistogramDictionary['QCD'].SetFillColor(ROOT.kCyan-5)
    finalHistogramDictionary['WJets'].SetFillColor(ROOT.kCyan)
    finalHistogramDictionary['TT'].SetFillColor(ROOT.TColor.GetColor('#9999cc'))
    finalHistogramDictionary['DY'].SetFillColor(ROOT.TColor.GetColor('#4496c8'))
    finalHistogramDictionary['Other'].SetFillColor(ROOT.TColor.GetColor('#12cadd'))

    finalHistogramDictionary['QCD'].SetLineWidth(0)
    finalHistogramDictionary['WJets'].SetLineWidth(0)
    finalHistogramDictionary['TT'].SetLineWidth(0)
    finalHistogramDictionary['DY'].SetLineWidth(0)
    finalHistogramDictionary['Other'].SetLineWidth(0)

    finalHistogramDictionary['Signal'].SetLineColor(ROOT.kRed)
    finalHistogramDictionary['Signal'].Scale(50)

    finalHistogramDictionary['Data'].SetMarkerStyle(20)
    finalHistogramDictionary['Data'].SetLineColor(ROOT.kBlack)
    finalHistogramDictionary['Data'].Sumw2()

    return finalHistogramDictionary

def main(args):
    ROOT.gStyle.SetOptStat(0)
    
    #Okay, first thing to do, grab some files so we can find our samples
    with open (args.sampleConfig) as jsonFile:
        samplesJson = json.load(jsonFile)
    
    #load the cut configuration json
    cutJson = None
    if args.cutConfig != None:
        with open(args.cutConfig) as jsonFile:
            cutJson = json.load(jsonFile)
    #let's create the cut we need to filter the histogram down to size
    finalCutString = processCutJsonToString(cutJson)

    #let's get the configuration for our varibles
    with open (args.variableConfig) as jsonFile:
        variableJson = json.load(jsonFile)

    #if we have an RE to specify variables, let's compile it
    theVariableRE = None
    if args.variableSpecification != None:
        theVariableRE = re.compile(args.variableSpecification)

    #let's get a list of samples and trees, so we don't open them for every variable later
    sampleDict,fileDict = createFileAndSampleDictionaries(samplesJson, args.signalMassPoint)
    
    #now let's loop over variables and make some histograms
    for variable in variableJson:
        if theVariableRE != None:
            if not theVariableRE.search(variable):
                continue
        print(variable)
        #let's try to power through any errors here and move on to the next variable, since we know the general approach works
        try:
            #draw the distributions
            histoDict = {'MC':{},'Data':{}}
            for MCSample in sampleDict['MC']:
                try:
                    histoName = MCSample+'_'+variableJson[variable]['name']
                except KeyError:
                    histoName = MCSample+'_'+variable
                sampleDict['MC'][MCSample].Draw(variable+'>>'+histoName+'('+variableJson[variable]['bins']+')',
                                                args.weightingFormula+'*('+finalCutString+')')
                theHisto = ROOT.gDirectory.Get(histoName).Clone()
                histoDict['MC'][MCSample] = theHisto
            #get the data histo
            try:
                dataHistoName = 'Data_'+variableJson[variable]['name']
            except KeyError:
                dataHistoName = 'Data_'+variable
            sampleDict['Data']['Data'].Draw(variable+'>>'+dataHistoName+'('+variableJson[variable]['bins']+')',
                                            finalCutString)
            dataHisto = ROOT.gDirectory.Get(dataHistoName).Clone()

            histoDict['Data']['Data'] = dataHisto
            finalHistoDict = produceFinalHistograms(histoDict)
            if args.scaleMC != None:
                for histDictKey in finalHistoDict:
                    if histDictKey != 'Data':
                        finalHistoDict[histDictKey].Scale(args.scaleMC)
            #now, let's prepare the stack

            backgroundStack = ROOT.THStack('backgroundStack','backgroundStack')
            backgroundStack.Add(finalHistoDict['QCD'], 'HIST')
            backgroundStack.Add(finalHistoDict['WJets'], 'HIST')
            backgroundStack.Add(finalHistoDict['TT'], 'HIST')
            backgroundStack.Add(finalHistoDict['DY'], 'HIST')
            backgroundStack.Add(finalHistoDict['Other'], 'HIST')

            backgroundStack_Errors = MakeStackErrors(backgroundStack)

            theCanvas = ROOT.TCanvas("theCanvas","theCanvas")
            theCanvas.Divide(1,2)

            plotPad = ROOT.gPad.GetPrimitive('theCanvas_1')
            ratioPad = ROOT.gPad.GetPrimitive('theCanvas_2')

            plotPad.SetPad("pad1","plot",0.0,0.2,1.0,1.0,0)
            ratioPad.SetPad("pad2","ratio",0.0,0.0,1.0,0.25,0)

            ratioPad.SetTopMargin(0.05)
            ratioPad.SetBottomMargin(0.27)
            plotPad.SetBottomMargin(0.08)
            ratioPad.SetGridy()

            ratioHist, ratioError = MakeRatioHistograms(finalHistoDict['Data'],backgroundStack,variable)

            ratioPad.cd()
            ratioHist.Draw('ex0')
            ratioError.Draw('SAME e2')
            ratioHist.Draw('SAME ex0')


            plotPad.cd()
            plotPad.SetTickx()
            plotPad.SetTicky()

            backgroundStack.SetMaximum(max(backgroundStack.GetMaximum(),dataHisto.GetMaximum()))

            backgroundStack.Draw()
            backgroundStack_Errors.Draw('SAME e2')
            backgroundStack.SetTitle(variable)
            finalHistoDict['Data'].Draw('SAME e1')
            finalHistoDict['Signal'].Draw('SAME HIST')
            backgroundStack.GetYaxis().SetTitle('Events')
            backgroundStack.GetYaxis().SetTitleOffset(1.58)
            backgroundStack.GetXaxis().SetLabelSize(0.0)

            theLegend = ROOT.TLegend(0.61,0.61,0.88,0.88)
            theLegend.AddEntry(finalHistoDict['Data'],'Observed','pe')
            theLegend.AddEntry(finalHistoDict['QCD'], 'QCD', 'f')
            theLegend.AddEntry(finalHistoDict['WJets'],'W+Jets','f')
            theLegend.AddEntry(finalHistoDict['TT'],'t#bar{t}','f')
            theLegend.AddEntry(finalHistoDict['DY'],'DY#rightarrow ll', 'f')
            theLegend.AddEntry(finalHistoDict['Other'],'Other','f')
            theLegend.AddEntry(finalHistoDict['Signal'],'Signal ('+args.signalMassPoint+'GeV, 1 pb #times 50)','l')
            theLegend.SetNColumns(2)

            theLegend.Draw()

            #also draw the preliminary warnings
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

            #just do some integral comparisons
            if args.printIntegrals:
                print('')
                print('-----------------')
                print('Integrals:')
                print('Data:' + str(finalHistoDict['Data'].Integral()))
                print('MC: '+str(backgroundStack_Errors.Integral()))
                print('')
                print('QCD: '+str(finalHistoDict['QCD'].Integral()))
                print('WJets: '+str(finalHistoDict['WJets'].Integral()))
                print('TT: '+str(finalHistoDict['TT'].Integral()))
                print('DY: '+str(finalHistoDict['DY'].Integral()))
                print('Other: '+str(finalHistoDict['Other'].Integral()))
                print('-----------------')
                print('')

            #let's figure out where we are going to _keep_ all of this
            if not os.path.isdir(args.destination):
                os.mkdir(args.destination)
            try:
                outFileName = variableJson[variable]['name']
            except KeyError:
                outFileName = variable
            print('')
            print('-----------------')
            theCanvas.SaveAs(args.destination+'/'+outFileName+'.png')
            theCanvas.SaveAs(args.destination+'/'+outFileName+'.pdf')
            print('-----------------')
            print('')


            if args.pause:
                raw_input('Press Enter to Continue...')
            del theCanvas
            del finalHistoDict
            del histoDict
        except Exception:
            print("There was a problem!")
            traceback.print_exc()
            continue

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate control plots for the bbtautau analysis fast')
    parser.add_argument('-v',
                        '--variableConfig',
                        nargs='?',
                        help = 'JSON with variable information to be drawn',
                        required=True)
    parser.add_argument('--variableSpecification',
                        nargs='?',
                        help='RE tospecify variables to be drawn from the list')
    parser.add_argument('--sampleConfig',
                        nargs='?',
                        help='location of the configuration that points out specific samples',
                        required=True)
    parser.add_argument('--batchMode',
                        action='store_true',
                       help='run in batch mode (no attempt to draw to screen)')
    parser.add_argument('--cutConfig',
                        nargs='?',
                        help = 'configuration to take standard cutting setups from')
    parser.add_argument('--additionalSelections',
                        nargs='+',
                        help = 'additional seelctions on top of the standard ones')
    parser.add_argument('--year',
                        nargs='?',
                        choices = ['2016','2016APV','2017','2018'],
                        default = '2016',
                        help='Year to base lumi calculation off of')
    parser.add_argument('--destination',
                       nargs='?',
                       default='controlPlots',
                       help='directory to store plots in')
    parser.add_argument('--pause',
                        action='store_true',
                        help='pause after each plot drawn')
    parser.add_argument('--printIntegrals',
                        action='store_true',
                        help='Print integral information about data and MC after each variable')
    parser.add_argument('--scaleMC',
                        nargs='?',
                        type=float,
                        help='Scale the MC by a dedicated factor to see the effects')
    parser.add_argument('--signalMassPoint',
                        nargs='?',
                        choices=['1000','1200','1400','1600','1800','2000','2500','3000','3500','4000','4500'],
                        default='1000',
                        help='Signal mass point to include in the control plots')
    parser.add_argument('--weightingFormula',
                        nargs='?',
                        help='Formula to weight the final histograms by',
                        default = 'FinalWeighting')
    
    args = parser.parse_args()

    if args.batchMode:
        ROOT.gROOT.SetBatch(ROOT.kTRUE)

    main(args)
