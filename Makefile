code = hiperespectral.cpp
exe = hiper
flags = `pkg-config opencv4 --cflags --libs`
output = *.bin
output_folder = output/
file_path = spectrums/vegetation.tree.eucalyptus.maculata.vswir.jpl087.jpl.asd.spectrum.txt
log = log

run: prepare
	./$(exe) $(file_path) > $(output_folder)$(log)

prepare_cv:	
	g++ $(code) -g -O0 -o $(exe) $(flags)

prepare:	
	g++ $(code) -g -O0 -o $(exe) 

clean:
	rm -f $(exe)
	rm -f $(output_folder)$(output)
	rm -f $(output_folder)$(log)