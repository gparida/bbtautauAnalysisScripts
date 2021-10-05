#utility functions and objects for various histogram scripts
import ROOT

def compileCutString(listOfCuts):
    theFinalCutString = ''
    for cut in listOfCuts:
        theFinalCutString += '(' + cut + ')&&'
    #cut trailing ampersands from final cutconfig
    if len(listOfCuts) > 0:
        theFinalCutString = theFinalCutString[:len(theFinalCutString)-2]
    return theFinalCutString

def processCutJsonToString(cutJson,additionalSelections = None):
    cuts = []
    if cutJson != None:
        for cutKey in cutJson:
            cuts.append(cutJson[cutKey])
    if additionalSelections != None:
        cuts = cuts+additionalSelections
    return compileCutString(cuts)

def createFileAndSampleDictionaries(samplesJson,signalMassPoint):
    sampleDict = {'Data':{},'MC':{}} # we should not lose track of what is data and what isn't
    fileDict = {'Data':{},'MC':{}}
    for sampleKey in samplesJson:
        theFile = ROOT.TFile(samplesJson[sampleKey]['file'])
        if theFile.IsZombie():
            raise RuntimeError("Zombie File "+samplesJson[sampleKey]['file'])
        theTree = theFile.Events
        #can't close these files, or we lose the trees
        if 'Data' in sampleKey:
            fileDict['Data']['Data'] = theFile
            sampleDict['Data'][sampleKey] = theTree
            #only load the radion signal we want
            #not real fond of coding it like this
        elif ('Radion' in sampleKey and 'M'+signalMassPoint in sampleKey):
            fileDict['MC'][sampleKey] = theFile
            sampleDict['MC'][sampleKey] = theTree
        elif not 'Radion' in sampleKey and not 'Data' in sampleKey:
            fileDict['MC'][sampleKey] = theFile
            sampleDict['MC'][sampleKey] = theTree
    return sampleDict,fileDict

