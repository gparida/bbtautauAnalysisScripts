#!/usr/bin/env python
#script for skimming large nano-aod ntuples on hdfs down to size.

import ROOT
import argparse
import glob
import re
import json
import math
import os
from tqdm import tqdm
from bbtautauAnalysisScripts.boostedTauNanoMaker.skimModules.skimManager import skimManager

def main(args):
    print('Setting up the skim...')
    #first things first, let's load the json
    jsonInputFile = open(args.skimFileConfiguration)
    jsonFileGlobs = json.load(jsonInputFile)
    jsonInputFile.close()
    
    try:
        datasetREs = [re.compile(x) for x in args.datasets]
    except Exception as err:
        print('Failed to make proper RE\'s for datasets:')
        print(err)
        exit(1)    


    branchCancelationREs = None
    if args.skimBranchCancelations != None:
        branchCancellationFile = open(args.skimBranchCancelations)
        branchCancelationJSON = json.load(branchCancellationFile)
        branchCancellationFile.close()

        try:
            branchCancelationREs = [re.compile(branchCancelationJSON[x]) for x in branchCancelationJSON]
        except Exception as err:
            print('Failed to make proper RE\'s for Branch cancellations')
            print(err)
            exit(1)

    print('Performing skim...')
    for datasetIndex in tqdm(range(len(jsonFileGlobs)), desc = 'Datasets'):
        #let's figure out the list of files we need to operate on
        listOfGlobs = []
        globKey = jsonFileGlobs.keys()[datasetIndex]
        for REIndex in tqdm(range(len(datasetREs)),leave = False, desc = 'RE Check: '+globKey):
            if datasetREs[REIndex].search(globKey):
                globsToAdd = glob.glob(jsonFileGlobs[globKey])
                if globsToAdd == []:
                    print('empty glob: '+globKey+', '+jsonFileGlobs[globKey])
                else:
                    listOfGlobs += globsToAdd
        if listOfGlobs == []: #we found no files to work with
            continue
            
        #load the files and get to work on them
        for loadFileIndex in tqdm(range(len(listOfGlobs)), desc = 'Process: '+globKey):
            outputFileName = globKey+'_'+('{:0'+str(int(math.floor(math.log10(len(jsonFileGlobs))))+1)+'}').format(loadFileIndex)+'.root'
            outputFileName = args.destination+'/'+outputFileName
            loadFileName = listOfGlobs[loadFileIndex]
            if args.prepareCondorSubmission: #prepare submission files for condor sub, and submit
                #first, we create the script
                runScriptName = globKey+'_Submit.sh'
                theRunScript = None
                if loadFileIndex == 0: #Only write this for the first thing
                    theRunScript = open(runScriptName,'w+')
                    theRunScript.write('#!/bin/sh\n\n')
                    currentLocation = os.environ['PWD']
                    theRunScript.write('cd '+currentLocation+'\n')
                    theRunScript.write('export X509_USER_PROXY=$2\n\n')
                else:
                    theRunScript = open(runScriptName,'a')
                theRunScript.write('if [ $1 -eq '+str(loadFileIndex)+' ]; then\n')
                theRunScript.write('\tpython boostedTauNanoMaker/scripts/singleFileSkimForSubmission.py ')
                theRunScript.write('--inputFile '+loadFileName+' ')
                theRunScript.write('--branchCancelations ')
                for reToWrite in branchCancelationJSON:
                    theRunScript.write(branchCancelationJSON[reToWrite])
                theRunScript.write(' ')
                theRunScript.write('--theCutFile '+args.skimCutConfiguration+' ')
                theRunScript.write('--outputFileName '+outputFileName+'\n')
                theRunScript.write('fi\n')
                theRunScript.close()
                #now we write the submission file itself
                #only if it's the first thing though, we just need one
                if loadFileIndex == 0:
                    theSubFile = open(globKey+'_Submit.sub','w+')
                    theSubFile.write('executable = '+runScriptName+'\n')
                    theSubFile.write('Proxy_path = '+args.x509Proxy+'\n')
                    theSubFile.write('arguments = $(ProcId) $(Proxy_path)\n')
                    theSubFile.write('output = '+args.destination+'/'+globKey+'.$(ClusterId).$(ProcId).out\n')
                    theSubFile.write('error = '+args.destination+'/'+globKey+'.$(ClusterId).$(ProcId).err\n')
                    theSubFile.write('log = '+args.destination+'/'+globKey+'.$(ClusterId).log\n\n')
                    theSubFile.write('#Make sure to perform this with our environment\n')
                    theSubFile.write('getenv = True\n\n')
                    theSubFile.write('# Send the job to Held state on failure.\n')
                    theSubFile.write('on_exit_hold = (ExitBySignal == True) || (ExitCode != 0)\n\n')
                    theSubFile.write('# Periodically retry the jobs every 10 minutes, up to a maximum of 5 retries.\n')
                    theSubFile.write('periodic_release =  (NumJobStarts < 3) && ((CurrentTime - EnteredCurrentStatus) > 600)\n\n')
                    theSubFile.write('+JobFlavour = "longlunch"\n')
                    theSubFile.write('queue '+str(len(listOfGlobs)))
                    theSubFile.close()

                    
            else: #attempt the skim locally
                theSkimManager = skimManager()
                theSkimManager.skimAFile(fileName = loadFileName,
                                         branchCancelations = branchCancelationREs,
                                         theCutFile = args.skimCutConfiguration,
                                         outputFileName = outputFileName)
        #submit the submission file
        if args.prepareCondorSubmission:
            os.system('condor_submit '+(globKey+'_Submit.sub'))
            
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'Skim HDFS nanoAOD ntuples down to size in a configurable way')
    parser.add_argument('--skimFileConfiguration',nargs='?',required=True,help='JSON file describing the paths/files to be skimmed',type=str)
    parser.add_argument('--datasets',nargs='+',default=['.*'],help='select datasets from the configuration file')
    parser.add_argument('--skimCutConfiguration',nargs='?',required=True,help='JSON file describing the cuts to be implemented in the files')
    parser.add_argument('--skimBranchCancelations',nargs='?',help='JSON file describing the branches that do not need to be ported around with the skimmed nanoAOD file')
    parser.add_argument('--destination',nargs='?',type=str,required=True,help='destination path for resut files')
    parser.add_argument('--prepareCondorSubmission',action='store_true',help='Instead of attempting the overall skimming on a local CPU, prepare a "Combine-style" submission to condor')
    parser.add_argument('--x509Proxy',nargs='?',required=True,help='Path to the x509 proxy to be used to open the files. REQUIRED TO BE IN AFS')

    args = parser.parse_args()

    main(args)
