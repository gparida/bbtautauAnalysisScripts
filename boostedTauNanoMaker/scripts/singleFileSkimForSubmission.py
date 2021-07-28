#!/usr/bin/env python
#script designed to do small sets of skims, with everything necessary fed into it as an argument
#so it can be suitable for 

import argparse
import re

from bbtautauAnalysisScripts.boostedTauNanoMaker.skimModules.skimManager import skimManager

def main(args):
    branchCancelationREs = None
    if args.branchCancelations != None:
        try:
            branchCancelationREs = [re.compile(x) for x in args.branchCancelations]
        except Exception as err:
            print('Failed to make proper RE\'s for Branch cancellations')
            print(err)
            exit(1)


    theSkimManager = skimManager()
    theSkimManager.skimAFile(fileName = args.inputFile,
                             branchCancelations = branchCancelationREs,
                             theCutFile = args.theCutFile,
                             outputFileName = args.outputFileName)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'Skim a single HFS nanoAOD ntuple down to size from arguments. For use on condor via scripts mostly')
    parser.add_argument('--inputFile',nargs='?',help='Input file to skim',required=True)
    parser.add_argument('--branchCancelations',nargs='+',help='List of branches to cancel')
    parser.add_argument('--theCutFile',nargs='?',help='File containing the JSON of cuts to use',required=True)
    parser.add_argument('--outputFileName',nargs='?',help='Location/File name of the output',required=True)

    args=parser.parse_args()
    main(args)
