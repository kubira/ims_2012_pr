NAME=dokumentace

$(NAME).pdf: $(NAME).dvi
	dvipdf $(NAME).dvi

$(NAME).dvi: $(NAME).tex title.tex
	latex $(NAME).tex
	latex $(NAME).tex

clean:
	-rm $(NAME).dvi $(NAME).aux $(NAME).toc $(NAME).log $(NAME).out
