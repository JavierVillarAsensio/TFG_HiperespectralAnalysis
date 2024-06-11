code_spec = spectrum.cpp
exe_spec = spec

flags = `pkg-config opencv4 --cflags --libs`

out = *.bin
output_folder = output/
distances_folder = distances/
logs_folder = logs/
log = *.log

code_master = master.cpp
exe_master = master

all: clean prepare_master prepare_spec run_spectrum run_master

run_master: prepare_master
	./$(exe_master)

prepare_master:
	g++ $(code_master) -o $(exe_master)

run_spectrum: clean prepare_spec
	./$(exe_spec)

prepare_cv:	
	g++ $(code_spec) -o $(exe_spec) $(flags)

prepare_spec:	
	g++ $(code_spec) -o $(exe_spec) 

clean:
	rm -f $(exe_spec)
	rm -f $(exe_master)
	rm -f $(output_folder)$(distances_folder)$(out)
	rm -f $(output_folder)$(logs_folder)$(log)