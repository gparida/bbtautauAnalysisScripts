#adds a branch to signal files
# 1 for gen level tt
# 2 for gen level mt
# 3 for gen level et
# 4 for gen level em
# 5 for other
import ROOT
import json
import re
import os
from array import array
import argparse
from tqdm import tqdm

def determineGenTauDecayMode(daughterPDGCodes):
    if 15 in daughterPDGCodes or -15 in daughterPDGCodes:
        raise RuntimeError('Received tau that decayed to tau! Please check it\'s daughters instead!')
    if 13 in daughterPDGCodes or -13 in daughterPDGCodes:
        return 'm'
    elif 12 in daughterPDGCodes or -12 in daughterPDGCodes:
        return 'e'
    else:
        return 't'

def determineTauDaughters(tauIndex,tree):
    tauDaughterParticles = []
    for genPartIndex in range(tree.nGenPart):
        if tree.GenPart_genPartIdxMother[genPartIndex] == tauIndex:
            if abs(tree.GenPart_pdgId[genPartIndex]) == 15: # we got a tau and had some intermediary radiation thing happen
                tauDaughterParticles += determineTauDaughters(genPartIndex, tree) #we now want the daughters of that tau,
            else:
                tauDaughterParticles.append(tree.GenPart_pdgId[genPartIndex])
    return tauDaughterParticles

def determineChannelDecayMode(tree):
    numberOfGenParticles = tree.nGenPart

    totalHiggsProducts = 0
    higgsProductIndices = []
    for genPartIndex in range(numberOfGenParticles):
        #look for a gen level tau
        if (abs(tree.GenPart_pdgId[genPartIndex]) == 15):
            #print("Found a gen tau, with status {0:b}".format(tree.GenPart_statusFlags[genPartIndex]))
            if (tree.GenPart_genPartIdxMother[genPartIndex] > 0):
                if(tree.GenPart_pdgId[tree.GenPart_genPartIdxMother[genPartIndex]] == 25):
                    totalHiggsProducts+=1
                    higgsProductIndices.append(genPartIndex)

    if(totalHiggsProducts != 2):
        raise RuntimeError("Event with != 2 taus to a higgs! %i" % i)

    #okay, now that we have the indices of the two taus, let's look for particles that
    #came from these taus.
    decayModes = []
    for tauIndex in higgsProductIndices:
        tauDaughterParticles = determineTauDaughters(tauIndex, tree)
        decayModes.append(determineGenTauDecayMode(tauDaughterParticles)) #figure out what the tau decayed to and attach it to the list

    decayModes.sort()
    decayChannel = ''.join(x for x in decayModes)

    return decayChannel
    

def main(args):
    with open (args.sampleConfig) as jsonFile:
        samplesJson = json.load(jsonFile)

    listOfFiles = []
    listOfTrees = []
    for sampleKey in samplesJson:
        if not re.search('Radion',sampleKey):
            continue
        theFile = ROOT.TFile(samplesJson[sampleKey]['file'],'UPDATE')
        if theFile.IsZombie():
            raise RuntimeError("Zombie File "+samplesJson[sampleKey]['file'])
        theTree = theFile.Events
        listOfFiles.append(theFile)
        listOfTrees.append(theTree)
    
    for treeIndex in tqdm(range(len(listOfTrees)), desc = 'Trees'):
        tree = listOfTrees[treeIndex]
        #we need to make the branch first
        theGenLevelDecayModeArray = array('I', [0])
        theGenLevelDecayModeBranch = tree.Branch('tauDecayModeCode', theGenLevelDecayModeArray, 'tauDecayModeCode/I')
        
        #okay, now we need to loop on the tree and determine the decay mode
        for i in tqdm(range(tree.GetEntries()), desc = 'Adding decay modes', leave=False):
            tree.GetEntry(i)
            theDecayChannel = determineChannelDecayMode(tree)
            #print('Event decay mode: %s' %theDecayChannel)
            #okay, now we map the decay mode onto an integer
            if theDecayChannel == 'tt':
                theGenLevelDecayModeArray[0] = 1
            elif theDecayChannel == 'mt':
                theGenLevelDecayModeArray[0] = 2
            elif theDecayChannel == 'et':
                theGenLevelDecayModeArray[0] = 3
            elif theDecayChannel == 'em':
                theGenLevelDecayModeArray[0] = 4
            else:
                theGenLevelDecayModeArray[0] = 5
            theGenLevelDecayModeBranch.Fill()
        #make sure we are in the appropriate place, and write things
        listOfFiles[treeIndex].cd()
        tree.Write('Events',ROOT.TObject.kOverwrite | ROOT.TObject.kSingleKey)
        listOfFiles[treeIndex].Write()
        
            
            
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate a branch to attach to files that will code the specific gen-level tau decay mode present in MC')
    parser.add_argument('--sampleConfig',
                        nargs='?',
                        help = 'JSON of the configuration that points out specific samples',
                        required=True)

    args = parser.parse_args()
    
    main(args)
