CCODE = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(CCODE))

GLSL = $(wildcard *.glsl)
SHADEROBJ = $(patsubst %.glsl, %.spv, $(GLSL))

ELF = first_try

dbg=
ifeq ($(dbg), 1)
CFLAGS += -DDEBUG
else
CFLAGS += -O2
endif

all: $(ELF) $(SHADEROBJ)
	@echo done

$(SHADEROBJ): %.spv : %.glsl
	glslangValidator -V $^ -o $@

$(ELF): $(OBJ)
	gcc $^ -o $@ ./libfilemap.so -lvulkan

$(OBJ): %.o : %.c
	gcc -c $^ $(CFLAGS)

clean:
	rm $(OBJ) $(ELF) $(SHADEROBJ)
