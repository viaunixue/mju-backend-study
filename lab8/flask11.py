from flask import Flask

app = Flask(__name__)

count = 0

@app.route('/increase')
def on_increase():
    global count
    count += 1
    return str(count)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)