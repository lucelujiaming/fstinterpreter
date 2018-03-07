all: interpreter 
.PHONY : all
interpreter:
	g++ -o interpreter forsight_inter_control.cpp \
        forsight_interpreter_shm.cpp\
        forsight_xml_reader.cpp\
        forsight_innercmd.cpp forsight_innerfunc.cpp \
        forsight_basint.cpp forsight_int_main.cpp \
        -lrt -lpthread  -I/usr/include/libxml2 \
        -I/usr/local/include/libxml2 \
        -L/usr/local/lib -lxml2  -std=c++11 -fpermissive

clean:
	rm interpreter 
