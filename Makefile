NAME_1=projekt_bezny_provoz
NAME_2=projekt_vanoce
NAME_3=projekt_vanoce_bez_navyseni

all:
	g++ $(NAME_1).cc -o run_$(NAME_1) -lsimlib
	g++ $(NAME_2).cc -o run_$(NAME_2) -lsimlib
	g++ $(NAME_3).cc -o run_$(NAME_3) -lsimlib

run:
	./run_$(NAME_1)
	./run_$(NAME_2)
	./run_$(NAME_3)
	@echo "****************************************"
	@echo "Vystupy jednotlivych experimentu jsou"
	@echo "ulozeny v souborech s priponou  *.dat"
	@echo

clean:
	rm run_* output_*
