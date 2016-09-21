CXX = g++
CFLAGS = -O3
INCLUDES = -I/opt/local/include 

TMP_DIR = ../../../tmp/
BIN_DIR = ../../../bin/tools/

all:	sar_reader_ceos sar_reader_envisat image_extractor

image_extractor:  	$(TMP_DIR)image_extractor.o
	$(CXX) -o $(BIN_DIR)image_extractor `vigra-config --libs` $(TMP_DIR)image_extractor.o 

sar_reader_ceos: 	$(TMP_DIR)sar_reader_ceos.o
	$(CXX) -o $(BIN_DIR)sar_reader_ceos $(TMP_DIR)sar_reader_ceos.o 
	
sar_reader_envisat: 	$(TMP_DIR)sar_reader_envisat.o
	$(CXX) -o $(BIN_DIR)sar_reader_envisat $(TMP_DIR)sar_reader_envisat.o 


$(TMP_DIR)%.o: 	%.cxx  
	$(CXX) -c $(CFLAGS) $(INCLUDES) $< -o $@
	
clean:
	rm $(BIN_DIR)sar_reader_ceos $(BIN_DIR)sar_reader_envisat  $(TMP_DIR)sar_reader_ceos.o $(TMP_DIR)sar_reader_envisat.o