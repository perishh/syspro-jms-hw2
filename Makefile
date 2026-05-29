CC := gcc
CFLAGS := -Wall -Wextra -Iinclude -g -pthread

SRC := src
INC := include

OBJ := build/obj
BIN := build/bin

SRC_COORD = $(wildcard $(SRC)/coord/*.c)
OBJ_COORD = $(patsubst $(SRC)/coord/%.c, $(OBJ)/coord/%.o, $(SRC_COORD))

SRC_CONSOLE = $(wildcard $(SRC)/console/*.c)
OBJ_CONSOLE = $(patsubst $(SRC)/console/%.c, $(OBJ)/console/%.o, $(SRC_CONSOLE))

SRC_GLOBAL = $(wildcard $(SRC)/*.c)
OBJ_GLOBAL = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRC_GLOBAL))

all: coord console

coord: $(OBJ_COORD) $(OBJ_GLOBAL)
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -I$(INC)/coord $^ -o $(BIN)/jms_coord

console: $(OBJ_CONSOLE) $(OBJ_GLOBAL)
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -I$(INC)/console $^ -o $(BIN)/jms_console

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC)/$(dir $*) -c $< -o $@

clean:
	rm -rf build/

.PHONY: all clean coord console
