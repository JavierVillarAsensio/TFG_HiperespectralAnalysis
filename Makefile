code_spec = spectrum.cpp
exe_spec = spec

flags = `pkg-config opencv4 --cflags --libs`

out = *.bin
output_folder = output/
distances_folder = distances/
logs_folder = logs/
log = *.log

code_master = master.cpp
include = ./include
exe_master = master

result = result.jpg
legend = legend.txt

code_compare = compare_result.cpp
exe_compare = compare
lib_compare = lmatio

all: run_spectrum run_master

run_master: prepare_master
	./$(exe_master)

prepare_master:
	g++ $(code_master) -o $(exe_master) -I $(include) -$(lib_compare)

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
	rm -f $(output_folder)$(result)
	rm -f $(output_folder)$(legend)