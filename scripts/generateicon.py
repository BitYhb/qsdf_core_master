import os
import sys
from PIL import Image


def separate_str(symbol):
    temp_str = ''
    arr = []
    for c in symbol:
        if c != 'x':
            temp_str += c
        else:
            arr.append(temp_str)
            temp_str = ''
    if len(temp_str) > 0:
        arr.append(temp_str)
    return arr


def main(icon_from, icon_to):
    android_dic = {
        'drawable-ldpi': '36x36',
        'drawable-mdpi': '48x48',
        'drawable-hdpi': '72x72',
        'drawable-xhdpi': '96x96',
        'drawable-xxhdpi': '144x144'
    }

    try:
        img = Image.open(icon_from)
        for key in android_dic:
            size_arr = separate_str(android_dic[key])
            crop_img = img.resize((eval(size_arr[0]), eval(size_arr[1])), Image.LANCZOS)
            crop_img_path = os.path.join(icon_to, key, 'icon.png')
            if not os.path.exists(os.path.dirname(crop_img_path)):
                os.mkdir(os.path.dirname(crop_img_path))
            crop_img.save(crop_img_path)
            print("generateicon.py: save %s successfully" % crop_img_path)
    except IOError as e:
        print(e)
        exit(1)


if __name__ == '__main__':
    print("generateicon.py: run. arguments: %s, %s" % (sys.argv[1], sys.argv[2]))
    main(icon_from=sys.argv[1], icon_to=sys.argv[2])
