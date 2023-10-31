PATHB = build/
PATHO = build/objs/
PATHS = src/

OUTFILE = dns

COMPILE = cc -c
LINK = cc

SRC = $(wildcard $(PATHS)*.c)
OBJS = $(patsubst $(PATHS)%.c, $(PATHO)%.o, $(SRC))

$(PATHB)$(OUTFILE) : $(OBJS)
	$(LINK) -o $@ $^

$(PATHO)%.o : $(PATHS)%.c 
	$(COMPILE) $< -o $@

.PHONY : clean
clean :
	rm $(PATHB)$(OUTFILE) $(OBJS)
