SRCDIR = SRC

SRCS := $(wildcard $(SRCDIR)/*.cpp)

r: b
	./Src/app.exe

b:
	g++ ${SRCS} -o Src/app


dos:
	D:/Programs/DOSBox-0.74-3/DOSBox.exe

one: