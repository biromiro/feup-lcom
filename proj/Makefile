# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c utils.c periferals/kbc.c periferals/timer.c periferals/video_gr.c periferals/interrupt_handler.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -O3
# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>

periferals/kbc.o: periferals/kbc.c periferals/kbc.h
		cc $(CFLAGS) -o periferals/kbc.o -c periferals/kbc.c 
periferals/timer.o: periferals/timer.c periferals/timer.h
		cc $(CFLAGS) -o periferals/timer.o -c periferals/timer.c 
periferals/video_gr.o: periferals/video_gr.c periferals/video_gr.h
		cc $(CFLAGS) -o periferals/video_gr.o -c periferals/video_gr.c 
periferals/interrupt_handler.o: periferals/interrupt_handler.c periferals/interrupt_handler.h
		cc $(CFLAGS) -o periferals/interrupt_handler.o -c periferals/interrupt_handler.c 