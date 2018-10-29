SOURCE = $(wildcard *.cpp)
TARGET = $(patsubst %.cpp, %, $(SOURCE))

.PHONY: all
all: build

.PHONY: test
test: all
	python3 test.py

.PHONY: build
build: $(TARGET)

%: %.cpp
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	-rm $(TARGET)
