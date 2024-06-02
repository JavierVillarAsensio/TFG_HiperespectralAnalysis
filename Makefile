specs_folder = spectrums/

code = hiperespectral.cpp
exe = hiper

run: prepare
	./$(exe)

prepare:	
	g++ $(code) -g -O0 -o $(exe)

clean:
	rm -f $(exe)