CXX := clang++
CXXFLAGS := -g -O3 -std=c++20 -Wall -Wextra -Werror -pedantic
LINKNCURSESFLAG := -lncurses 
TICKITLINK :=  -ltickit
DEPS := TextBuffer.h key_codes.h
# SRCDIR=./src/ 

# scanner: scanner.o
# 	$(CXX) -o $@ $^

elditor: elditor.o key_codes.o
	$(CXX) -o $@ $^ $(LINKNCURSESFLAG)

scanner: scanner.o key_codes.o
	$(CXX) -o $@ $^ $(LINKNCURSESFLAG) -fsanitize=address

# $@ is the left side of the :
# $< is the first dependency
# $^ is the right side of the :
%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -rf elditor *.o a.out