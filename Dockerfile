FROM ubuntu

RUN apt-get update && apg-get install -y \
build-essential 

FROM gcc:latest

COPY . .

RUN g++ hiperespectral.cpp -o hiper

CMD ["./hiper", "spectrums/vegetation.tree.eucalyptus.maculata.vswir.jpl087.jpl.asd.spectrum.txt", "> output/log"]