#Just a quick class to handle the yaml and holding all of the sample objects we need

import yaml
from bbtautauAnalysisScripts.dataCardMaker.sample import sample

class sampleSet():
    #initialize using the yaml, and make a set of samples to be used 
    def __init__(self, theSampleYaml):
        with open(theSampleYaml) as theFile:
            self.yamlContents = list(yaml.load_all(theFile, Loader=yaml.SafeLoader))
        self.sampleList = []
        for document in self.yamlContents:
            self.sampleList.append( sample(document) )
