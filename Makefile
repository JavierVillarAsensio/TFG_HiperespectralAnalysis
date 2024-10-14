code_spec = spectrum_replica.cpp
spectrum_funcs = spectrum_common_functions.cpp
exe_spec = spec

flags = `pkg-config opencv4 --cflags --libs`

output_folder = output/
code_folder = code/

include_folder = code/include
lib_compare = lmatio

code_master = master.cpp
master_funcs = master_common_functions.cpp
exe_master = master

result = result.jpg
legend = legend.txt

code_compare = compare_result.cpp
exe_compare = compare

test_folder = testing/black_box/code_test/
code_test = spectrum_test.cpp
exe_test = test

all: run_spectrum run_master

compile_master:
	g++ $(code_folder)$(code_master) -o $(exe_master) -I $(include_folder) -$(lib_compare)

run_master: compile_master
	./$(exe_master)

compile_spec:	
	g++ $(code_folder)$(code_spec) -o $(exe_spec) -I $(include_folder)

run_spectrum: clean compile_spec
	./$(exe_spec)

compile_cv:	
	g++ $(code_folder)$(code_spec) -o $(exe_spec) $(flags)

compile_test:
	g++ -I./$(include_folder) $(include_folder)/$(spectrum_funcs) $(include_folder)/$(master_funcs) $(test_folder)$(code_test) -o $(exe_test) -$(lib_compare)

test: clean compile_test
	./$(exe_test)

clean:
	rm -f $(exe_spec)
	rm -f $(exe_master)
	rm -f $(exe_test)
	rm -rf $(output_folder)*