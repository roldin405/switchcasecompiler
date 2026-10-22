CC = gcc
CFLAGS = -Wall -Wextra -g -I. -Ibuild
FLEX = flex
BISON = bison

BUILD_DIR = build
TARGET = $(BUILD_DIR)/switch_compiler

SRCS = main.c ast.c semantic.c
PARSER_C = $(BUILD_DIR)/parser.tab.c
PARSER_H = $(BUILD_DIR)/parser.tab.h
LEXER_C = $(BUILD_DIR)/lex.yy.c

OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/ast.o $(BUILD_DIR)/semantic.o $(BUILD_DIR)/parser.tab.o $(BUILD_DIR)/lex.yy.o

.PHONY: all clean run test web

all: $(TARGET)

$(TARGET): $(BUILD_DIR) $(PARSER_C) $(LEXER_C) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(PARSER_C) $(PARSER_H): parser.y | $(BUILD_DIR)
	$(BISON) -d -o $(PARSER_C) parser.y

$(LEXER_C): lexer.l $(PARSER_H) | $(BUILD_DIR)
	$(FLEX) -o $(LEXER_C) lexer.l

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/parser.tab.o: $(PARSER_C) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lex.yy.o: $(LEXER_C) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) input.txt

test: $(TARGET)
	@bash run_tests.sh

web: $(TARGET)
	@python3 app.py

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.c $(BUILD_DIR)/*.h $(TARGET)
	@echo "Clean complete."
