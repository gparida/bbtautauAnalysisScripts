#just a quick class to handle sets of categories and reading the yaml into the various categories

import yaml
from bbtautauAnalysisScripts.dataCardMaker.category import category

class categorySet():
    #initialize using the yaml
    def __init__(self, theCategoryYaml):
        with open(theCategoryYaml) as theFile:
            self.yamlContents = list(yaml.load_all(theFile, Loader=yaml.SafeLoader))
        self.categoryList = []
        for document in self.yamlContents:
            self.categoryList.append( category(document) )

    def initializeAllHistogramDictionaries(self, theSampleSet):
        for theCategory in self.categoryList:
            theCategory.initializeHistogramDictionary(theSampleSet)
