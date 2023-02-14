import argparse
import os
import string
import shutil
import urllib.request


def parse_args(destination, name, type):
    print("destination:", destination)
    print("name:", name)
    print("type:", type)


parser = argparse.ArgumentParser(description="Used to generate various template files.")
parser.add_argument("-n", "--name", help="target project name. eg: EPDomain_okr", required=True)
parser.add_argument("-t", "--type", help="target project type", choices=["domain", "app", "plugin"], required=True)
parser.add_argument("-d", "--destination", help="target generated path", default=r'..\template_generate')
args = parser.parse_args()


def download_gitignore():
    url = "https://www.toptal.com/developers/gitignore/api/c++,qt,qml"
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) '
                      'Chrome/110.0.0.0 Safari/537.36',
    }

    request = urllib.request.Request(url, headers=headers)
    gitignore_text = urllib.request.urlopen(request).read()

    try:
        gitignore_file_path = os.path.join(args.destination, args.name, '.gitignore')
        with open(gitignore_file_path, 'w', encoding='utf-8') as gitignore_file:
            gitignore_file.write(gitignore_text.decode())
            gitignore_file.close()
    except IOError:
        shutil.rmtree(args.destination)
        exit(1)


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
        with open(file, 'r') as f:
            template = string.Template(f.read())
            f.close()

        file = file.replace(template_files_path, '')
        target_file_path = args.destination + '\\' + args.name + '\\' + file
        target_file_path_dirname = os.path.dirname(target_file_path)
        if not os.path.exists(target_file_path_dirname):
            os.makedirs(target_file_path_dirname)

        with open(os.path.realpath(target_file_path), 'x', encoding='utf-8') as f:
            f.write(template.safe_substitute(TM_TARGET_NAME=args.name))
            f.close()

    download_gitignore()
