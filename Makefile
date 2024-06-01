specs = spectrums.txt
reflectances_folder = reflectances
specs_folder = spectrums/

code = hiperespectral.cpp
exe = hiper

run: prepare
	./$(exe)

prepare:	
	ls $(specs_folder) >> $(specs)
	mkdir -p $(reflectances_folder)
	g++ $(code) -o $(exe)

clean:
	rm -f $(exe)
	rm -f $(specs)
	rm -r -f $(reflectances_folder)
