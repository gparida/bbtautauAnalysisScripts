import CRABClient
import argparse

from CRABClient.UserUtilities import config #, getUsernameFromSiteDB
config = config()
#from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
#from httplib import HTTPException
config.section_('General')
config.General.workArea =  'signalTest' #Will be rewritten
config.General.requestName = 'Radion_4500_GeV' #will br rewritten
config.General.transferLogs=True


config.section_('JobType')
#config.JobType.psetName = '../bbtautauAnalysisScripts/boostedTauNanoMaker/python/boostedTauReNano_2016_MC_cff.py'
#config.JobType.sendPythonFolder = True
config.JobType.maxMemoryMB = 2500


config.section_('Data')
config.Data.inputDataset = '/RadionTohhTohtatahbb_narrow_M-4500_TuneCP5_13TeV-madgraph-pythia8/RunIISummer20UL16MiniAODv2-106X_mcRun2_asymptotic_v17-v1/MINIAODSIM'
config.Data.inputDBS            = 'global'
config.Data.splitting           = 'FileBased'
config.Data.unitsPerJob         = 1
#config.Data.outLFNDirBase       = '/store/user/%s/%s' % ('parida','CRABtest_Radion_2016')
config.Data.outLFNDirBase       = '/store/user/gparida/CRABtest_Radion_2016'
config.Data.outputDatasetTag    = '%s' % ('reNano') # will be rewritten
config.Data.publication         = False


config.section_('Site')
#config.Site.storageSite         = '%s' % ('T2_US_Wisconsin')
config.Site.storageSite         = 'T2_US_Wisconsin' 


if __name__ == '__main__':
    from CRABAPI.RawCommand import crabCommand

    parser = argparse.ArgumentParser(description = 'Generate file list JSONs from a list of samples')
    #parser.add_argument('--dataTier',nargs = '?', choices=['MINIAODSIM','NANOAODSIM','MINIAOD','NANOAOD'],help='Datatier to form the list of files for',default='MINIAODSIM')
    parser.add_argument('-f','--datasetListFile',nargs='?',help='The text file containing list of the datasets',required=True)
    parser.add_argument('-c','--configFile',help='the file that you would cmsRun on - relative to this file',required=True)
    parser.add_argument('-w','--workArea', help='CRAB work area HHbbtt/year_Data/MC',required=True)
    parser.add_argument('-o','--outputDir',help='file path to be created in the hdfs to store files HHbbtt/Year_Data/MC',required=True)
    parser.add_argument('-t','--type',choices=['Data','MC'],help='Whether these are Data or MC. This will decide the request Name',required=True)
    parser.add_argument('-n','--username',help="Wisconsin T2 user name. so that the files are output in the right area in hdfs",required=True)
    args = parser.parse_args()

    with open(args.datasetListFile, 'r') as readFile:
        jobsLines = readFile.readlines()
        req_names = []
        for job in jobsLines :
            myJob = job.rstrip()
            if (len(myJob)==0 or myJob[0][0]=='#'): continue
            print (myJob)
            config.Data.inputDataset = myJob
            if args.type == "MC":
                if any(myJob.split('/')[1] in i for i in req_names):
                    config.General.requestName = myJob.split('/')[1] + "_" + (myJob.split('/')[2]).split('_')[-1] + "_otherPart"
                    config.Data.outputDatasetTag = myJob.split('/')[1] + "_" + (myJob.split('/')[2]).split('_')[-1] + "_otherPart"
                    req_names.append(config.General.requestName)
                else:
                    config.General.requestName = myJob.split('/')[1]
                    config.Data.outputDatasetTag = myJob.split('/')[1]
                    req_names.append(myJob.split('/')[1])
            elif args.type == "Data":
                config.General.requestName = myJob.split('/')[2]
                config.Data.outputDatasetTag = myJob.split('/')[2]
                #config.Data.inputBlocks = ['/MET/Run2016G-UL2016_MiniAODv2-v2/MINIAOD#605911b3-0c18-421a-9131-6ad9299bfa19']

                       
            print ("creating a request name = " + config.General.requestName + '\n')
            #config.Data.outLFNDirBase = '/store/user/gparida/' + args.outputDir
            config.Data.outLFNDirBase = '/store/user/'+args.username+'/' + args.outputDir            
            config.JobType.psetName = args.configFile
            config.General.workArea = args.workArea
            crabCommand('submit', config = config)
            




