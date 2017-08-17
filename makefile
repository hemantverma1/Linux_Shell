CC = g++
CFLAGS = -Wall
DEPS = parser.h cd.h pwd.h echo.h exit_s.h export.h history.h
OBJ = parser.o cd.o pwd.o echo.o exit_s.o export.o history.o shell.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

shell: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

