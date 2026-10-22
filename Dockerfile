
FROM python:3.12-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc make flex bison \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN make

EXPOSE 5000

CMD ["python", "app.py"]
