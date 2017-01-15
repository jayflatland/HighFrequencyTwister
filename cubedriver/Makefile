CXX=g++
CXXFLAGS=-std=c++11 -g -O3 -Wl,--no-as-needed -I cpp_kociemba -I /opt/boost/include/boost-1_56

.PHONY: all
all: cubedriver test

obj/%.o: %.cpp 
	$(CXX) -c -o $@ $<  $(CXXFLAGS)

obj/%.o: cpp_kociemba/%.cpp 
	$(CXX) -c -o $@ $< $(CXXFLAGS) 

obj/capv4l2.o: capv4l2.cpp capv4l2.h util.h

obj/serial.o: serial.cpp serial.h util.h

obj/util.o: util.cpp util.h

obj/config.o: config.cpp config.h

obj/translate_moves.o: translate_moves.h

obj/threaded_solver2.cpp: threaded_solver2.h translate_moves.h

obj/cubedriver.o: cubedriver.cpp cubedriver.h config.h capv4l2.h serial.h util.h translate_moves.h threaded_solver2.h cpp_kociemba/*.h time_for_move_sequence.h

obj/coordcube.o: cpp_kociemba/coordcube.cpp cpp_kociemba/*.h

obj/cubiecube.o: cpp_kociemba/cubiecube.cpp cpp_kociemba/*.h 

obj/facecube.o:cpp_kociemba/facecube.cpp cpp_kociemba/*.h

obj/search.o: cpp_kociemba/search.cpp cpp_kociemba/*.h

obj/test.o: test.cpp cpp_kociemba/*.h time_for_move_sequence.h

cubedriver: obj/capv4l2.o obj/serial.o obj/util.o obj/config.o obj/translate_moves.o obj/threaded_solver2.o obj/cubedriver.o obj/coordcube.o obj/cubiecube.o obj/cubiecube.o obj/facecube.o obj/search.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) -lSDL2 -lSDL2_ttf

test: obj/test.o obj/coordcube.o obj/cubiecube.o obj/cubiecube.o obj/facecube.o obj/search.o obj/threaded_solver2.o obj/translate_moves.o
	$(CXX) -o $@ $^ $(CXXFLAGS) -lpthread 

.PHONY: clean
clean:
	rm -f obj/*.o
