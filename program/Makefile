OBJS1	=	server_main.o server_net.o server_command.o server_sys.o
OBJS2	=	client_main.o client_net.o client_command.o client_win.o
TARGET1	=	server
TARGET2	=	client
CFLAGS	=	-c

.c.o:
	gcc $(CFLAGS) $<

all: $(TARGET1) $(TARGET2)

$(TARGET1):	$(OBJS1)
	gcc -o $(TARGET1) $(OBJS1) -lm -lSDL -lSDL_image -lcwiimote -lSDL_ttf -lSDL_gfx -lopencv_core -lopencv_imgproc -lopencv_highgui -D_ENABLE_TILT -D_ENABLE_FORCE -L/usr/lib $(LDFLAGS)
$(TARGET2):	$(OBJS2)
	gcc -o $(TARGET2) $(OBJS2) -lm -lSDL -lSDL_image -lSDL_mixer -lcwiimote -lSDL_ttf -lSDL_gfx -lopencv_core -lopencv_imgproc -lopencv_highgui -D_ENABLE_TILT -D_ENABLE_FORCE -L/usr/lib $(LDFLAGS)

clean:
	rm *.o $(TARGET1) $(TARGET2)

server_main.o: server_main.c server_common.h server_func.h common.h
server_net.o: server_net.c server_common.h server_func.h common.h
server_command.o: server_command.c server_common.h server_func.h common.h
server_sys.o: server_sys.c server_common.h server_func.h common.h
client_main.o: client_main.c client_func.h common.h
client_net.o: client_net.c client_func.h common.h
client_command.o: client_command.c client_func.h common.h
client_win.o: client_win.c client_func.h common.h
