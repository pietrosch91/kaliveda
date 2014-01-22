MKDWN_EXT = .txt
GLOBAL_PANDOC_OPTIONS = --mathjax --latex-engine=pdflatex -S -s --toc -c ../rootdoc.css --section-divs
MKDWN_SOURCES := $(wildcard *$(MKDWN_EXT))

HTML_OUTPUT := $(patsubst %$(MKDWN_EXT),html/%.html,$(MKDWN_SOURCES))
PDF_OUTPUT  := $(patsubst %$(MKDWN_EXT),pdf/%.pdf,$(MKDWN_SOURCES))
TEX_OUTPUT  := $(patsubst %$(MKDWN_EXT),tex/%.tex,$(MKDWN_SOURCES))

OUTPUT = $(HTML_OUTPUT)

.PHONY: all clean

all: $(OUTPUT)

clean:
	@rm -f $(OUTPUT)
	
.SUFFIXES: $(MKDWN_EXT) .html .pdf .tex

html/%.html: %$(MKDWN_EXT)
	@echo "Generating $@ ..."
	@pandoc $(GLOBAL_PANDOC_OPTIONS) -f markdown -t html -s -o $@ $< 

pdf/%.pdf: %$(MKDWN_EXT)
	@echo "Generating $@ ..."
	@pandoc $(GLOBAL_PANDOC_OPTIONS) -f markdown -t pdf -s -o $@ $< 

tex/%.tex: %$(MKDWN_EXT)
	@echo "Generating $@ ..."
	@pandoc $(GLOBAL_PANDOC_OPTIONS) -f markdown -t latex -s -o $@ $< 
