CC=gcc
FLAGS=
BIN=bin
CPU_OUT=$(BIN)/libcpu_6502.a

.PHONY: clean archive

all: release

debug: FLAGS += -DCPU_DEBUG
debug: archive

test_%: archive
test_%: $(CPU_OUT) test_%.o
	mkdir -p $(BIN)
	$(CC) $(FLAGS) -o $(BIN)/$@ $^ -L$(BIN) -lcpu_6502
	make --no-print-directory clean
	clear
	./$(BIN)/$@

release: FLAGS += -O2
release: archive

archive: cpu.o alu.o opcodes.o
	mkdir -p $(BIN)
	ar rcs $(CPU_OUT) $^
	make --no-print-directory clean

test_%.o: FLAGS += -Isrc/
test_%.o: test/%.c
	$(CC) $(FLAGS) -c $< -o $@

%.o: src/%.c
	$(CC) $(FLAGS) -c $<

opcodes.o: src/control/opcodes.c
	$(CC) $(FLAGS) -c $<

clean:
	rm *.o