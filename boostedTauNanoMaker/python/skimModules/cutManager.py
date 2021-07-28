#a quick object for creating cut strings estimating cutflows from a tree and dictionary full of cuts

import json
from ROOT import TTree,TH1F
from tqdm import tqdm

class cutManager():
    def __init__(self,theTree,theCutFile):
        self.theTree = theTree

        cuttingConfigurationFile = open(theCutFile)
        self.theCutDictionary = json.load(cuttingConfigurationFile)
        cuttingConfigurationFile.close()

        #do we want a complete cut variable?
    #let's start with the easy one, creating the cutflow
    def createCutFlowHistogram(self):
        numberOfCuts = len(self.theCutDictionary.keys())
        numberOfBins = numberOfCuts+1
        theCutFlow = TH1F('cutflow','cutflow',numberOfBins,0,numberOfBins)
        
        theCutFlow.SetBinContent(1, self.theTree.GetEntries())
        theCutFlow.GetXaxis().SetBinLabel(1,"No Cuts")
        
        #okay, loop now over the number of bins,
        for i in range(numberOfCuts):
            #for each bin we look at, first figure out what cut applies
            theCut = self.createCuts(i+1)
            #now that we have the cut, figure out how many events we're talking about
            numberOfEvents = self.theTree.GetEntries(theCut)
            theCutFlow.SetBinContent(i+2,numberOfEvents)
            theCutFlow.GetXaxis().SetBinLabel(i+2,self.theCutDictionary.keys()[i])
        #We should now have a comlete cutflow, return it
        return theCutFlow

    #create a proper cut string from the dictionary
    #specify the number of cuts to be used
    #and in order, it will take them, and make the TTree string for it
    def createCuts(self,numberOfCuts):
        keyList = self.theCutDictionary.keys()
        if numberOfCuts == 1:
            return self.theCutDictionary[keyList[0]]
        elif numberOfCuts > 1:
            if numberOfCuts > len(keyList):
                numberOfCuts = len(keyList)
            theCut = ''
            for i in range(numberOfCuts):
                theCut+='('+self.theCutDictionary[keyList[i]]+')&&'
            #we ended up with two extra ampersands on the back end
            #ditch the last two characters
            theCut = theCut[:len(theCut)-2]
            return theCut
        else:
            return ''
        return ''

    #let's just create a version of getting the cuts that just gets the final cut version
    def createAllCuts(self):
        nCuts = len(self.theCutDictionary.keys())
        return self.createCuts(nCuts)
