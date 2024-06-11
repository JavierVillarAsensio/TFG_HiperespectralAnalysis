code = spectrum.cpp
exe = spec
flags = `pkg-config opencv4 --cflags --libs`
output = *.bin
output_folder = output/
distances_folder = distances/
logs_folder = logs/
log = *.log

run: clean prepare
	./$(exe)

prepare_cv:	
	g++ $(code) -o $(exe) $(flags)

prepare:	
	g++ $(code) -o $(exe) 

clean:
	rm -f $(exe)
	rm -f $(output_folder)$(distances_folder)$(output)
	rm -f $(output_folder)$(logs_folder)$(log)