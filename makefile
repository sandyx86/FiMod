CC=gcc

main:
	$(CC) src/main.c -DRAYGUI_IMPLEMENTATION -o main -lraylib -lm

clean:
	rm main