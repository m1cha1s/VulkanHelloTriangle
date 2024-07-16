CC := gcc
CFLAGS := -std=c99 -O0 -g
LDFLAGS := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SRC := main.c
OBJ := $(SRC:.c=.o)

TARGET := vht

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS)

.PHONY: clean run

clean:
	rm $(TARGET) $(OBJ)

run: $(TARGET)
	./$(TARGET)
