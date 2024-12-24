#GRUP NO: 37
#Metin Hakan Yılmaz / B221210075
#Elşat Mirdesoğlu / B221210579
#Osman Yıldız / B221210021
#Abdulsamed Kurubal / B221210009
#Ferdi Kaynar / G201210311 

CC = gcc
CFLAGS = -Wall -g

OBJS = main.o
TARGET = myshell

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)