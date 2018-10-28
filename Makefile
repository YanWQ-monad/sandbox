CXXFLAGS := -I. -std=c++14 -Wall -Wextra
LDFLAGS :=
LDLIBS := -lseccomp -lpthread

TARGET = sandbox
SOURCES =\
	$(filter-out ./python_module.cpp,\
	$(shell find $(SOURCEDIR) -name '*.cpp'))
HEADERS = $(shell find $(SOURCEDIR) -name '*.h')
OBJS = $(addsuffix .o, $(basename $(SOURCES)))

.PHONY: all
all: CXXFLAGS += -DNDEBUG -O3
all: $(TARGET)

.PHONY: debug
debug: CXXFLAGS += -g
debug: $(TARGET)

include $(addsuffix .d, $(basename $(SOURCES)))

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

%.d: %.c
	$(CC) $< -MM -MT'$*.o' -MT'$@' -MG -MP -MF'$@' $(CFLAGS)

%.d: %.cpp
	$(CXX) $< -MM -MT'$*.o' -MT'$@' -MG -MP -MF'$@' $(CXXFLAGS) $(CFLAGS)

.PHONY: py_module
py_module:
	python3 setup.py build

.PHONY: install
install: py_module
	python3 setup.py install

.PHONY: clean
clean:
	-rm *.o *.d src/*.o src/*.d
