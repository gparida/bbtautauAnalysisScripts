#!/usr/bin/env python
#script for taking our radion files tagged with the gen level decay mode, 
#and splitting it up into separate files based on the gen level tau decay mode tag
import ROOT
import argparse
import json
import re
from tqdm import tqdm

def main(args):
    with open (args.inputJson) as jsonFile:
        samplesJson = json.load(jsonFile)
    
    listOfFiles = []
    listOfTrees = []
    listOfOldFileNames = [] # we'll use these to determine new file names
    for sampleKey in samplesJson:
        if not re.search('Radion', sampleKey):
            continue
        oldFilePath = samplesJson[sampleKey]['file']
        theFile = ROOT.TFile(oldFilePath)
        oldFileName = oldFilePath.split('/')[oldFilePath.count('/')]
        if theFile.IsZombie():
            raise RuntimeError('Zombie File '+samplesJson[sampleKey]['file'])
        theTree = theFile.Events
        listOfFiles.append(theFile)
        listOfTrees.append(theTree)
        listOfOldFileNames.append(oldFileName)

    for treeIndex in tqdm(range(len(listOfTrees)), desc='Original files'):
        tree = listOfTrees[treeIndex]

        #do this for each of the relevant codes
        for decayCode in tqdm([1,2,3,4], desc='Decay modes', leave = False):
            #figure out what we're going to call the file
            replaceString = ''
            if decayCode == 1:
                replaceString = '_GenTT.'
            elif decayCode == 2:
                replaceString = '_GenMT.'
            elif decayCode == 3:
                replaceString = '_GenET.'
            elif decayCode == 4:
                replaceString = '_GenEM.'
            newFileName = listOfOldFileNames[treeIndex].replace('.',replaceString)
            if args.destination != '':
                newFilePath = args.destination + '/' + newFileName
            else:
                newFilePath = newFileName
                
            theFile = ROOT.TFile(newFilePath, 'RECREATE')
            theFile.cd()

            newTree = tree.CopyTree('tauDecayModeCode == %i' % decayCode,'fast') # here's where the magic happens again
            theFile.cd() #just to make sure
            newTree.Write('Events', ROOT.TObject.kOverwrite|ROOT.TObject.kSingleKey)
            theFile.Write()
            theFile.Close()
            
            
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for splitting up radio signal based on gen levelt au decay mode tag')
    parser.add_argument('--inputJson',nargs='?',required=True,help='JSON configuration that points out the samples')
    parser.add_argument('--destination',nargs='?',required=True,help='Location to put resulting files')

    args = parser.parse_args()

    main(args)
