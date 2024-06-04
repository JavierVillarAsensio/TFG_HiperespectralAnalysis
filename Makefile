specs_folder = spectrums/

code = hiperespectral.cpp
exe = hiper
flags = `pkg-config opencv4 --cflags --libs`

run: prepare
	./$(exe)

prepare:	
	g++ $(code) -g -O0 -o $(exe) $(flags)

clean:
	rm -f $(exe)