.PHONY: all install clean

all: html

LYX_SOURCE:=$(wildcard *.lyx)
LYX_HTML_DIR:=$(patsubst %.lyx,%.html.LyXconv,$(LYX_SOURCE))
HTML_DIR:=$(patsubst %.lyx,%Doc,$(LYX_SOURCE))

html: $(LYX_SOURCE)
	-lyx -e html $^
	
install:
	-cp -r $(LYX_HTML_DIR) $(KVROOT)/KaliVedaDoc/$(HTML_DIR)
	-cp -r $(LYX_HTML_DIR) $(KVROOT)/KaliVedaDoc/$(VERSION_NUMBER)/$(HTML_DIR)

clean:
	-rm -r $(LYX_HTML_DIR)
