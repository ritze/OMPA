CXX := clang++
AR := ar
LLVMCOMPONENTS := bitreader mcparser transformutils option
RTTIFLAG := -fno-rtti
RUNDIRFLAG := -Wl,-R,'$$ORIGIN/../lib'
LLVMCONFIG := llvm-config

BIN = bin
LIB = lib
SRC = src
TEST = test

LIBSOURCES = $(SRC)/Analyse.cpp \
			 $(SRC)/Array.cpp \
			 $(SRC)/ArrayElement.cpp \
			 $(SRC)/ArrayTimestamp.cpp \
			 $(SRC)/DeclInfo.cpp \
			 $(SRC)/FunctionPattern.cpp \
			 $(SRC)/KernelASTVisitor.cpp \
			 $(SRC)/KernelBuiltinFunctions.cpp \
			 $(SRC)/KernelFunction.cpp \
			 $(SRC)/KernelInfo.cpp \
			 $(SRC)/Operation.cpp \
			 $(SRC)/Warnings.cpp

TESTSOURCES = $(TEST)/ArrayTest.cpp \
			  $(TEST)/DeAndIncrementTest.cpp \
			  $(TEST)/Debug.cpp \
			  $(TEST)/DependenceTest.cpp \
			  $(TEST)/MathTest.cpp \
			  $(TEST)/Test.cpp \
			  $(TEST)/main.cpp

OMPATEST = $(BIN)/ompa-test
OMPALIB = $(LIB)/libompa.a

LIBOBJECTS = $(LIBSOURCES:.cpp=.o)
TESTOBJECTS = $(TESTSOURCES:.cpp=.o)

CLANGLIBS = -lclang \
			-lclangFrontend \
			-lclangDriver \
			-lclangParse \
			-lclangSema \
			-lclangSerialization \
			-lclangAST \
			-lclangAnalysis \
			-lclangBasic \
			-lclangEdit \
			-lclangLex

CXXFLAGS := $(shell $(LLVMCONFIG) --cxxflags) $(RTTIFLAG) -I$(shell pwd)
LLVMLDFLAGS := $(shell $(LLVMCONFIG) --ldflags --libs $(LLVMCOMPONENTS))
LIBS = $(CLANGLIBS) $(LLVMLDFLAGS)

DEPENDFILE = .depends
-include $(DEPENDFILE)

all: lib
	
lib: $(LIBOBJECTS) $(OMPALIB)

dep: $(SOURCES)
	$(CXX) -MM $(SOURCES) > $(DEPENDFILE)

%: %.o
	$(CXX) -o $@ $< $(LIBS)

$(OMPALIB): $(LIBOBJECTS)
	@mkdir -p $(LIB)
	$(AR) -o -rs $(OMPALIB) $(LIBOBJECTS)

check: CXXFLAGS += -DDEBUG
check: lib $(TESTOBJECTS) $(OMPALIB)
	@mkdir -p $(BIN)
	$(CXX) -o $(OMPATEST) $(TESTOBJECTS) $(RUNDIRFLAG) $(OMPALIB) $(LIBS)
	$(BIN)/ompa-test

release: clean lib

clean: 
	@rm -rf $(OMPATEST) $(OMPALIB) $(LIBOBJECTS) $(TESTOBJECTS) $(DEPENDFILE)
	@rm -df $(BIN) $(LIB)

.PHONY: check clean release
