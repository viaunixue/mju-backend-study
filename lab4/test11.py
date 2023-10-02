import json
import sys

def main(argv):
    obj1 = {
        'name' : 'JongHyun Jung',
        'id' : 12345678,
        'work' : {
            'name' : 'Myongji University',
            'address' : '116 Myongji-ro'
        }
    }

    s = json.dumps(obj1)
    obj2 = json.loads(s)
    print(obj2['name'], obj2['id'], obj2['work']['address'])
    print(obj1 == obj2)

if __name__ == '__main__':
    main(sys.argv)