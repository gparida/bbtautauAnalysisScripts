#this will be the running script for the datacard maker setup

import ROOT
import argparse
from bbtautauAnalysisScripts.dataCardMaker.analysis import analysis

def main(args):
    # we really don't want to be drawing all these histograms to screen, so this script assumes
    #batch mode. This could be changed later if it becomes interesting
    
    ROOT.gROOT.SetBatch(ROOT.kTRUE)

    #create the analysis
    theAnalysis = analysis(
        outputName = args.outputName,
        analysisConfig = args.analysisConfig,
        sampleConfig = args.sampleConfig
    )
    #let it run.
    theAnalysis.runAnalysis()
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'Generate datacard setups from yaml configuration files')
    parser.add_argument('--analysisConfig',
                        nargs='?',
                        help='YAML describing the categories to organize the analysis in',
                        required = True)
    parser.add_argument('--sampleConfig',
                        nargs='?',
                        help='YAML describing the samples and files to use for the analysis',
                        required = True)
    parser.add_argument('--outputName',
                        nargs='?',
                        help='Name/path of the output root file datacard',
                        default='Out.root')

    args = parser.parse_args()

    main(args)
