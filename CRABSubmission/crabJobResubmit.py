#!/usr/bin/env python3
from pathlib import Path
import subprocess
import argparse


parser = argparse.ArgumentParser(description='A script hadding hadding different background and signal samples')
parser.add_argument('--basePath',help="Enter the dir path inside which there are  crab- directories",required=True)
args = parser.parse_args()

base_path = Path(args.basePath)

#searchPhrase = "_01March22_0950_skim_2016_Data_29February-singleFileSkimForSubmission"
#searchPhrase = "_TuneCP5"
#searchPhrase = args.Search

for obj in base_path.iterdir():
	if not obj.is_dir():
		continue

	if "crab_" not in obj.name:
	#if searchPhrase in obj.name:
		#print(f"{obj.name} doesn't have "+searchPhrase+" in it")
		continue
	if "crab_" in obj.name:
		print (f"{obj.name} has  "+"crab_"+" in it")
		print("Comamnd to Execute (reduce the required memory to 3500 from original submission) = ",f'crab resubmit {base_path/obj.name} --maxmemory 3500')
		#success1=subprocess.call(f'crab resubmit {base_path/obj.name} --maxmemory 2500', shell=True)
		#success1=subprocess.call(f'crab resubmit {base_path/obj.name}', shell=True)
		success1=subprocess.call(f'crab resubmit {base_path/obj.name} --maxmemory 3500', shell=True)




