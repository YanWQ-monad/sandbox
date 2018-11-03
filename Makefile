CXXFLAGS := -I. -std=c++14 -Wall -Wextra
LDFLAGS :=
LDLIBS := -lseccomp -lpthread

TARGET = sandbox
SOURCES =\
	$(filter-out ./python_module.cpp,\
	$(shell find $(SOURCEDIR) -name '*.cpp' | grep -v 'test/.*'))
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

.PHONY: lint
lint: cpplint.py
	-@./cpplint.py --filter=-legal/copyright --root=. --verbose=0 $(SOURCES) $(HEADERS)
	-@./cpplint.py --filter=-whitespace/line_length,-legal/copyright --root=. ./python_module.cpp

.PHONY: check_py_module
check_py_module:
	python3 -c 'import sandbox'

.PHONY: test
test: check_py_module
	$(MAKE) -C test test

.PHONY: py_module
py_module:
	python3 setup.py build
	python3 setup.py bdist_wheel --universal

.PHONY: install
install: py_module
	python3 setup.py install

.PHONY: clean
clean:
	-rm *.o *.d src/*.o src/*.d
	-rm -r dist build sandbox.egg-info
	$(MAKE) -C test clean
