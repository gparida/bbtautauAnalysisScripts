#!/user/bin/env python
#A class that will act as a master manager for the file skimming process
#should abstract the act of skimming a single file, and this should allow for easier submission

import ROOT
import json
from tqdm import tqdm
from cutManager import cutManager

class skimManager():
    def __init__(self):
        pass
    def skimAFile(self,
                  fileName,
                  branchCancelations,
                  theCutFile,
                  outputFileName):

        try:
            theLoadFile = ROOT.TFile(fileName)
            theInputTree = theLoadFile.Events
        except: #we failed to open the file properly, so let's try it the other way
            try:
                theLoadFile = ROOT.TFile.Open(fileName)
                theInputTree = theLoadFile.Events
            except: #we have failed again to find the file. Let's try to open it this way
                hdfsFileName = fileName.replace('/hdfs','root://cmsxrootd.hep.wisc.edu/')
                theLoadFile = ROOT.TFile.Open(hdfsFileName)
                theInputTree = theLoadFile.Events

        theCutManager = cutManager(theInputTree,theCutFile)
        
        nBranches = theInputTree.GetNbranches()
        listOfBranches = theInputTree.GetListOfBranches()
        #now we loop over branches, and the branch cancellation REs
        #if one of the RE's matches, disable the branch.
        if branchCancelations != None:
            for branchIndex in range(nBranches):
                for REIndex in range(len(branchCancelations)):
                    theRE = branchCancelations[REIndex]
                    theBranchName = listOfBranches[branchIndex].GetName()
                    if theRE.search(theBranchName):
                        theInputTree.GetBranch(theBranchName).SetStatus(0) # close out the branch and don't copy it
        #all branches should be canceled now
        #now let's set up a new file/tree
        theOutputFile = ROOT.TFile(outputFileName,'RECREATE') #this has to happen last to keep things associated with it

        theCutFlow = theCutManager.createCutFlowHistogram()
        theCutFlow.Write('cutflow', ROOT.TFile.kOverwrite)

        #now, let's do the magic copy
        finalCut = theCutManager.createAllCuts()
        theOutputTree = theInputTree.CopyTree(finalCut)

        theOutputTree.Write('Events', ROOT.TFile.kOverwrite)

        #okay, now let's get the remaining object in the nano file and

        theOutputFile.Write()
        theOutputFile.Close()
        theLoadFile.Close()        
