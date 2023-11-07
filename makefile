PATHB = build/
PATHO = build/objs/
PATHS = src/

BUILD_PATHS = $(PATHB) $(PATHO)

OUTFILE = dns

COMPILE = cc -c -Wall -Wpedantic -Werror
LINK = cc 

SRC = $(wildcard $(PATHS)*.c)
OBJS = $(patsubst $(PATHS)%.c, $(PATHO)%.o, $(SRC))

$(PATHB)$(OUTFILE) : $(BUILD_PATHS) $(OBJS)
	$(LINK) -o $@ $(OBJS)

$(PATHO)%.o : $(PATHS)%.c 
	$(COMPILE) $< -o $@

# create necessary directories
$(PATHB) :
	mkdir -p $@

$(PATHO) :
	mkdir -p $@

.PHONY : clean
clean :
	rm $(PATHB)$(OUTFILE) $(OBJS)
