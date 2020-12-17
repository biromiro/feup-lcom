# name of the program (Minix service)
PROG=proj

.PATH: ${.CURDIR}
.PATH: ${.CURDIR}/game
.PATH: ${.CURDIR}/periferals

# source code files to be compiled
SRCS = proj.c utils.c kbc.c timer.c video_gr.c interrupt_handler.c rtc.c character_movement.c magic_blast.c enemies.c game_state.c hud.c

# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>>
