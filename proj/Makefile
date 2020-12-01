# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c utils.c periferals/kbc.c periferals/timer.c periferals/video_gr.c periferals/interrupt_handler.c game/character_movement.c game/magic_blast.c

# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic -D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>>

periferals/kbc.o: periferals/kbc.c periferals/kbc.h
		cc $(CFLAGS) -o periferals/kbc.o -c periferals/kbc.c 
periferals/timer.o: periferals/timer.c periferals/timer.h
		cc $(CFLAGS) -o periferals/timer.o -c periferals/timer.c 
periferals/video_gr.o: periferals/video_gr.c periferals/video_gr.h
		cc $(CFLAGS) -o periferals/video_gr.o -c periferals/video_gr.c 
periferals/interrupt_handler.o: periferals/interrupt_handler.c periferals/interrupt_handler.h
		cc $(CFLAGS) -o periferals/interrupt_handler.o -c periferals/interrupt_handler.c 
game/character_movement.o: game/character_movement.c game/character_movement.h 
		cc $(CFLAGS) -o game/character_movement.o -c game/character_movement.c
game/magic_blast.o: game/magic_blast.c game/magic_blast.h 
		cc $(CFLAGS) -o game/magic_blast.o -c game/magic_blast.c