#Category definition

import ROOT

class category():
    def __init__(self, setupDict):
        self.setupDict = setupDict
        self.categoryName = self.setupDict['categoryName']
        self.definition = self.setupDict['definition']
        self.variableOfInterest = self.setupDict['variableOfInterest']
        #we are going to need a dictionary that keeps track ofthe histograms.
        #how do we structure this dictionary?
        """
        {
        
        {"sampleName": [list, of, histograms]},
        ...

        }
        """
        self.dictionaryOfHistograms = {}
        self.theCutString = self.createCategoryCutString()
        
    #we need a quick function for making the overall cut string associated with this category.
    def createCategoryCutString(self):
        theCutString = '('
        for definitionString in self.definition:
            theCutString += ' ( ' + definitionString + ' ) &&'
        #trim the last " &&" off
        theCutString = theCutString[:len(theCutString)-2]
        theCutString += ')'
        
        return theCutString
    
    def initializeHistogramDictionary(self, theSampleSet):
        for theSample in theSampleSet.sampleList:
            self.dictionaryOfHistograms[theSample.name] = []
            
        
