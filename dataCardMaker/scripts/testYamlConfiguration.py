#Just a quick script to make sure yaml configurations are reading right, and are structured how we want

import yaml
import argparse

def main(args):

    with open(args.theConfig) as theFile:
        print('Before yaml load')
        yamlData = list(yaml.load_all(theFile, Loader=yaml.SafeLoader))
        print(yamlData)
    print('Pretty Print:')

    print(yaml.dump(yamlData, indent=4, default_flow_style=False))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Pick up and print out yaml configurations to make sure it is structured properly')
    parser.add_argument('theConfig',
                       nargs='?',
                       help = 'yaml config'
    )
    
    args = parser.parse_args()

    main(args)
