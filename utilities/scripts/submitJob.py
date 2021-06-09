#a utility for submitting large batches of jobs to condor based on the file list JSONs
#the overall goal of this script will be to have a few things happen
#1.) the script should take care of creating a space to be used on /hdfs/store/user/
#2.) it should interact with the condor dag system to make rescue of jobs possible
#3.) Output should get written straight to the created /hdfs/store/user/ area
#4.) Should create a script that creates a farmout job. The script should be modifiable so that parameters can be tuned before a final submission is made

import argparse

def main():
    pass

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'Create submission file to re-nano-ify files')

    args = parser.parse_args()

    main(args)
