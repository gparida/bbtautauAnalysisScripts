#Class to define what a sample means inside of our datacard creator

import ROOT

class sample():
    #perform sample initiailization
    #in the future, we may want to offload some of this work, to a configuration style
    #yaml or json. Something like that.
    def __init__(self, setupDict):
        #Start by loading the files, constructing them from the path,
        self.setupDict = setupDict
        if 'path' in self.setupDict:
            self.listOfPaths = [self.setupDict['path']+'/'+theFileName for theFileName in self.setupDict['files']]
        else:
            self.listOfPaths = [theFileName for theFileName in self.setupDict['files']]
        
        self.theChain = ROOT.TChain('Events')
        for filePath in self.listOfPaths:
            self.theChain.Add(filePath)
        
        self.name = setupDict['sampleName']

        if 'definition' in self.setupDict:
            self.theChain = self.theChain.CopyTree( self.setupDict['definition'] )

        #We also need a way to store all the histograms. We'll just make a dictionary for it
        #We'll key it to nominal and the various uncertainties later
        #self.histogramDictionary = {}
        #actually, given the way that categories are going to work. We'll want 

        #TODO: we need a way of handling the various uncertainties.
        
    #Okay, we need a method for creating histograms
    #needs to be flexible enough that we can use it for nominal or uncertainty histograms
    def createHistogram(self,
                        weight,
                        variable,
                        cuts,
                        histoName = ''):
        overallName = self.name
        if histoName != '':
            overallName += '_'+histoName
        self.theChain.Draw(variable+'>>'+overallName,
                           weight+'*('+cuts+')')
        
        #retrieve the histogram we have just made
        theHistogram = ROOT.gDirectory.Get(overallName).Clone()
        return theHistogram
