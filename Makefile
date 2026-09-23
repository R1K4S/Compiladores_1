# =========================================================
# Makefile - Compilador (Flex + Bison)
# =========================================================

CC       := gcc
CFLAGS   := -Wall -g
FLEX     := flex
BISON    := bison

LEXER_DIR  := lexer
PARSER_DIR := parser
SRC_DIR    := src
BUILD_DIR  := build

LEXER_SRC   := $(LEXER_DIR)/lexel.l
PARSER_SRC  := $(PARSER_DIR)/parser.y
MAIN_SRC    := $(SRC_DIR)/main.c

LEXER_C     := $(BUILD_DIR)/lex.yy.c
PARSER_C    := $(BUILD_DIR)/parser.tab.c
PARSER_H    := $(BUILD_DIR)/parser.tab.h
MAIN_O      := $(BUILD_DIR)/main.o

TARGET      := $(BUILD_DIR)/compilador

.PHONY: all clean run

all: $(TARGET)

# Gera o parser (.c e .h) a partir do arquivo .y
$(PARSER_C) $(PARSER_H): $(PARSER_SRC) | $(BUILD_DIR)
	$(BISON) -d -o $(PARSER_C) $(PARSER_SRC)

# Gera o lexer (.c) a partir do arquivo .l, dependendo do header do parser
$(LEXER_C): $(LEXER_SRC) $(PARSER_H) | $(BUILD_DIR)
	$(FLEX) -o $(LEXER_C) $(LEXER_SRC)

# Compila o main.c
$(MAIN_O): $(MAIN_SRC) $(PARSER_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -c -o $(MAIN_O) $(MAIN_SRC)

# Compila e linka lexer + parser + main no executável final
$(TARGET): $(LEXER_C) $(PARSER_C) $(MAIN_O)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -o $(TARGET) $(LEXER_C) $(PARSER_C) $(MAIN_O)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)