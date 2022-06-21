#!/usr/bin/env python

import os
import re
import sys
import glob
import argparse

def submitJob(samplePath):

    print "\t Resubmitting %s..." % samplePath
    #rescueDag = max(glob.glob('%s/*dag.rescue[0-9][0-9][0-9]' % samplePath))
    #resubmitCommand = 'farmoutAnalysisJobs --rescue-dag-file %s' % samplePath
    #os.system(resubmitCommand)
    

def generateSubmitDirs(jobs):
    dirs = []
    #recursive search everything under this for a dag submission/"dags" directory
    for job in jobs:
        dirs += recursivelySearchForDagDir(job)
    return dirs
    
#takes a string directory to search, and returns a list of all directories/subdirectories
#that have a "dags" dir
def recursivelySearchForDagDir(search):
    theSearch = glob.glob(search)
    results = []
    for item in theSearch:
        if re.search('.*/dags/dag', item):
            results.append(item)
        else:
            results+=recursivelySearchForDagDir(item+'/*')
    return results
        
    
def main(args):
    submitDirs = generateSubmitDirs(args.jobs)
    
    for directory in submitDirs:
        submitJob(directory)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = 'Resubmit failed jobs to re-nano-ify files')
    parser.add_argument('--jobs',nargs='+',required=True,help='Paths to jobs to resubmit')

    args = parser.parse_args()

    main(args)
