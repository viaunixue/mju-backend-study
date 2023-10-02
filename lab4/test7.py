import json
import sys

def main(argv):
    obj1 = {
        'name' : 'JongHyun Jung',
        'id' : 12345678,
    }

    s = json.dumps(obj1, indent=2)
    print(s)
    print('Type', type(s).__name__)

if __name__ == '__main__':
    main(sys.argv)