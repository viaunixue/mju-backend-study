FROM python:3.8

WORKDIR /app

COPY . /app

RUN apt-get update && \
    apt-get install -y redis-server && \
    pip install --no-cache-dir -r requirements.txt

CMD ["bash", "-c", "service redis-server start && python3 memo.py"]
