#This will just be some classes on how we setup the overall data card creation process
#An analysis is essentially comprised of two things
#A set of samples,
#and a set of categories

#the samples willhandle the tree side and histogram drawing

#The categories will keep their definitions, and the resulting histograms that belong to them.

import ROOT
from bbtautauAnalysisScripts.dataCardMaker.sampleSet import sampleSet
from bbtautauAnalysisScripts.dataCardMaker.categorySet import categorySet
from bbtautauAnalysisScripts.dataCardMaker.outputSet import outputSet

class analysis():
    def __init__(self, outputName, analysisConfig, sampleConfig):
        self.theOutputSet = outputSet( outputFileName = outputName  )#this needs to happen first to have a file open
        self.theSampleSet = sampleSet( sampleConfig )
        self.theCategorySet = categorySet( analysisConfig )
        
        #Now that we have our samples and categories all laid out, 
        #let's quickly scroll the categories and
        #make sure they are primed to have histograms drawn
        self.theCategorySet.initializeAllHistogramDictionaries(self.theSampleSet)
    
    #Okay. Let's make a quick function for drawing all the nominal histograms
    def createNominalHistograms(self):
        for theCategory in self.theCategorySet.categoryList:
            for theSample in self.theSampleSet.sampleList:
                theHistogram = theSample.createHistogram(weight = 'FinalWeighting', #TODO: we need an eloquent way of setting this
                                                         variable = theCategory.variableOfInterest,
                                                         cuts = theCategory.theCutString)
                theCategory.dictionaryOfHistograms[theSample.name].append(theHistogram)
                
    #this will be the overall function. call this and all the hsitograms and output will happen
    def runAnalysis(self):
        self.createNominalHistograms()

        #TODO: insert uncertainty creation here

        #write out all the histograms, and close out
        for theCategory in self.theCategorySet.categoryList:
            self.theOutputSet.logCategoryOutput(theCategory)
        
        self.theOutputSet.finalize()


