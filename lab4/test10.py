import json
import sys

def main(argv):
    obj1 = {
        'name': 'JongHyun Jung',
        'id': 12345678,
        'work': {
            'name': 'Myongji University',
            'address': '116 Myongji-ro'
        },
        'scores': [100, 90.5, 80.0]
    }

    s = json.dumps(obj1, indent = 2)
    print(s)

if __name__ == '__main__' :
    main(sys.argv)