OBJS = trace.o triangle_list.o ppm.o ray_trace.o read_nff.o mem_alloc.o \
       graphics_math.o light_list.o sphere_list.o __RVC_Instrument.o __RVC_Malloc_Free_Monitor.o

all: trace

trace: $(OBJS)
	gcc -o c-ray-tracer $(OBJS) -lm

trace.o: trace.c
	gcc -Wall -ansi -O3 -c trace.c 

triangle_list.o: triangle_list.c triangle_list.c
	gcc -Wall -ansi -O3 -c triangle_list.c 

sphere_list.o: sphere_list.c sphere_list.c
	gcc -Wall -ansi -O3 -c sphere_list.c 

light_list.o: light_list.c light_list.c
	gcc -Wall -ansi -O3 -c light_list.c 

ppm.o: ppm.c ppm.h 
	gcc -Wall -ansi -O3 -c ppm.c

ray_trace.o: ray_trace.c ray_trace.h
	gcc -Wall -ansi -O3 -c ray_trace.c

read_nff.o: read_nff.c read_nff.h
	gcc -Wall -ansi -O3 -c read_nff.c 

mem_alloc.o: mem_alloc.c mem_alloc.h
	gcc -Wall -ansi -O3 -c mem_alloc.c

graphics_math.o: graphics_math.c graphics_math.h
	gcc -Wall -ansi -O3 -c graphics_math.c

clean:
	rm -f trace $(OBJS) 

