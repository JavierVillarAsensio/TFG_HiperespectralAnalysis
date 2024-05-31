specs = spectrums.txt
specs_folder = spectrums/

code = hiperespectral.cpp
exe = hiper

run: prepare
	./$(exe)

prepare:	
	ls $(specs_folder) >> $(specs)
	g++ $(code) -o $(exe)

clean:
	rm -f $(exe)
	rm -f $(specs)
