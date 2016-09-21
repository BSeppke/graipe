CXX = g++
CFLAGS = -O3
INCLUDES = -I/opt/local/include 
LIBS = -L/opt/local/lib -lboost_date_time-mt

TMP_DIR = ../../../tmp/
BIN_DIR = ../../../bin/tools/

all:	bsh_reader_v2 bsh_reader_v3

bsh_reader_v2: 	$(TMP_DIR)bsh_reader_v2.o
	$(CXX) -o $(BIN_DIR)bsh_reader_v2 $(CFLAGS) $(LIBS) $(TMP_DIR)bsh_reader_v2.o 

bsh_reader_v3: 	$(TMP_DIR)bsh_reader_v3.o
	$(CXX) -o $(BIN_DIR)bsh_reader_v3 $(LIBS) $(TMP_DIR)bsh_reader_v3.o 
	
$(TMP_DIR)%.o: 	%.cxx  
	$(CXX) -c $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm $(BIN_DIR)bsh_reader_v2 $(BIN_DIR)bsh_reader_v3 $(TMP_DIR)bsh_reader_v2.o $(TMP_DIR)bsh_reader_v3.o