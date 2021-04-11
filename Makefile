CXX=g++
CXXFLAGS=-g -c
OBJDIR=build

all: output game

output:
	mkdir -p ${OBJDIR}

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

game: ${OBJDIR}/asset.o ${OBJDIR}/rotmg.o ${OBJDIR}/nanotime.o
	${CXX} $^ -ldoge -lGL -lglfw -lGLU -lX11 -lGLEW -o ${OBJDIR}/game