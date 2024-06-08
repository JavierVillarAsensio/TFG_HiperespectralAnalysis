FROM ubuntu

RUN apt-get update && apg-get install -y \
build-essential 

FROM gcc:latest

WORKDIR /hiperespectral

COPY hiperespectral.cpp /hiperespectral/
COPY jasperRidge2_R198 /hiperespectral//jasperRidge2_R198 

RUN mkdir spectrums
RUN mkdir output

RUN g++ hiperespectral.cpp -o hiper

CMD ["./hiper"]