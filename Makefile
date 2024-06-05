code = hiperespectral.cpp
exe = hiper
flags = `pkg-config opencv4 --cflags --libs`
output = *.bin
output_folder = output/
log = *.log

run: clean prepare
	./$(exe)

prepare_cv:	
	g++ $(code) -g -O0 -o $(exe) $(flags)

prepare:	
	g++ -std=c++17 $(code) -g -O0 -o $(exe) 

clean:
	rm -f $(exe)
	rm -f $(output_folder)$(output)
	rm -f $(output_folder)$(log)