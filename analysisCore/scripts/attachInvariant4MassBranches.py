import ROOT
import json
import argparse
from tqdm import tqdm
from array import array

def main (args):
    with open(args.sampleConfig) as jsonFile:
        samplesJson = json.load(jsonFile)

    listOfFiles = []
    listOfTrees = []
    #print(samplesJson)
    for sampleKey in samplesJson:
        theFile = ROOT.TFile(samplesJson[sampleKey]['file'], 'UPDATE')
        if theFile.IsZombie():
            print('Found zombie file: %s, continuing...' % samplesJson[sampleKey]['file'])
            continue
        theTree = theFile.Events
        listOfFiles.append(theFile)
        listOfTrees.append(theTree)
        
    print(len(listOfTrees))
    for treeIndex in tqdm(range(len(listOfTrees)), desc = 'Trees'):
        tree = listOfTrees[treeIndex]
        #let's make the branch first
        theFirstHiggsMassArray = array('f', [0.0])
        theSecondHiggsMassArray = array('f', [0.0])
        theInvariantFourMassArray = array('f', [0.0])

        theFirstHiggsMassBranch = tree.Branch('firstHiggsMass', theFirstHiggsMassArray, 'firstHiggsMass/F')
        theSecondHiggsMassBranch = tree.Branch('secondHiggsMass', theSecondHiggsMassArray, 'secondHiggsMass/F')
        theInvariantFourMassBranch = tree.Branch('inv4Mass', theInvariantFourMassArray, 'in4Mass')

        #loop on the tree
        for i in tqdm(range(tree.GetEntries()), desc = 'Adding masses', leave=False):
            tree.GetEntry(i)
            #okay. Now we get get the objects, and do the overall calculation
            leadingTauObject = ROOT.TLorentzVector()
            leadingTauObject.SetPtEtaPhiM(
                tree.allTau_pt[0],
                tree.allTau_eta[0],
                tree.allTau_phi[0],
                tree.allTau_mass[0]
            )
            #get the other Object
            secondLepton = ROOT.TLorentzVector()
            if args.channel == 'et':
                secondLepton.SetPtEtaPhiM(
                    tree.gElectron_pt[0],
                    tree.gElectron_eta[0],
                    tree.gElectron_phi[0],
                    tree.gElectron_mass[0]
                )
            if args.channel == 'mt':
                secondLepton.SetPtEtaPhiM(
                    tree.gMuon_pt[0],
                    tree.gMuon_eta[0],
                    tree.gMuon_phi[0],
                    tree.gMuon_mass[0]
                )
            if args.channel == 'tt':
                secondLepton.SetPtEtaPhiM(
                    tree.allTau_pt[1],
                    tree.allTau_eta[1],
                    tree.allTau_phi[1],
                    tree.AllTau_mass[1]
                )
            #get the fat jet
            fatJetObject = ROOT.TLorentzVector()
            fatJetObject.SetPtEtaPhiM(
                tree.gFatJet_pt[0],
                tree.gFatJet_eta[0],
                tree.gFatJet_phi[0],
                tree.gFatJet_mass[0]
            )
            
            theFirstHiggsMassArray[0] = (leadingTauObject + secondLepton).M()
            theSecondHiggsMassArray[0] = fatJetObject.M()
            theInvariantFourMassArray[0] = (leadingTauObject + secondLepton + fatJetObject).M()
            
            theFirstHiggsMassBranch.Fill()
            theSecondHiggsMassBranch.Fill()
            theInvariantFourMassBranch.Fill()
        listOfFiles[treeIndex].cd()
        tree.Write('Events', ROOT.TObject.kOverwrite | ROOT.TObject.kSingleKey)
        listOfFiles[treeIndex].Write()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate a branch to attach to files that will be the invariant 4 mass, and 2 higgs masses of the considered objects')
    parser.add_argument('--sampleConfig',
                        nargs='?',
                        help = 'JSON of the configuration that points out specific samples',
                        required=True)

    parser.add_argument('--channel',
                        nargs='?',
                        choices = ['et','mt','tt'],
                        help = 'channel to calculate',
                        required = True)

    args = parser.parse_args()
    
    main(args)
