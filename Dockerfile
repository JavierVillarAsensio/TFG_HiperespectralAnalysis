FROM ubuntu

RUN apt-get update && apg-get install -y \
build-essential 

FROM gcc:latest

COPY . .

RUN g++ hiperespectral.cpp -o hiper

CMD ["./hiper"]