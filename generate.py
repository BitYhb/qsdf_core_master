import os
import string
import argparse


def parse_args(destination, name, type):
    print("destination:", destination)
    print("name:", name)
    print("type:", type)


parser = argparse.ArgumentParser(description="Used to generate various template files.")
parser.add_argument("-n", "--name", help="target project name. eg: EPDomain_okr", required=True)
parser.add_argument("-t", "--type", help="target project type", choices=["domain", "app", "plugin"])
parser.add_argument("-d", "--destination", help="target generated path", default=r'..\template_generate')
args = parser.parse_args()

if __name__ == '__main__':
    try:
        parse_args(args.destination, args.name, args.type)
    except Exception as e:
        print(e)

    template_files_path = r'template\{type}'.format(type=args.type)
    path_walk = os.walk(template_files_path)
    files = []
    for dirpath, dirnames, filenames in path_walk:
        for filename in filenames:
            filename = dirpath + '\\' + filename
            files.append(filename)

    for file in files:
        template_file = open(file, 'r')
        template = string.Template(template_file.read())

        file = file.replace(template_files_path, '')
        target_file_path = args.destination + '\\' + args.name + '\\' + file
        target_file_path_dirname = os.path.dirname(target_file_path)
        if not os.path.exists(target_file_path_dirname):
            os.makedirs(target_file_path_dirname)

        target_file = open(os.path.realpath(target_file_path), 'x', encoding='utf-8')
        target_file.write(template.safe_substitute(TM_TARGET_NAME=args.name))
