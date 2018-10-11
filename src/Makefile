TARGET=main
CC=arm-linux-gcc
CFLAGS=-I ../inc/ -I ../inc/freetype2/ -L ../lib/ -ljpeg -L ../lib/ -lfreetype -pthread -ldl
CRCS=display_jpeg.c freetype.c garage.c lcd.c license.c main.c rfid.c serial_init.c sqlite3.c v4l2.c

$(TARGET):$(OBJ)
	$(CC) $(CRCS) -o $(TARGET) $(CFLAGS)
	
