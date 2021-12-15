#This will just be a quick class for handling the output of the datacard maker

import ROOT

class outputSet():
    def __init__(self, outputFileName):
        self.outputFileName = outputFileName
        self.theFile = ROOT.TFile(self.outputFileName, 'RECREATE')
        self.listOfCategoryDirectories = []

    #Take a category, make a directory for it, and write all the histograms in the category
    def logCategoryOutput(self, theCategory):
        theDirectory = self.theFile.mkdir(theCategory.categoryName)
        #keep a log of what we're up to
        self.listOfCategoryDirectories.append(theDirectory)
        theDirectory.cd()
        for sampleName in theCategory.dictionaryOfHistograms:
            listOfHistograms = theCategory.dictionaryOfHistograms[sampleName]
            for histogram in listOfHistograms:
                histogram.Write()
        theDirectory.Write()
        self.theFile.cd()
        
    def finalize(self):
        self.theFile.Write()
        self.theFile.Close()
        
