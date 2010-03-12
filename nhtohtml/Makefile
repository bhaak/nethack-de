### Makefile --- 

## Author: rob@simra.net
## Version: $Id: Makefile,v 1.2 2004/05/23 00:46:22 simra Exp $
## Keywords: 
## X-URL: 

VERSION = $(shell grep Revision nhtohtml.pl | head -1 | cut -d ' ' -f 5)
DISTDIR = nhtohtml-$(VERSION)


html: html/index.html

html/index.html: nhtohtml.pl
	./nhtohtml.pl

dist: html
	mkdir -p $(DISTDIR)
	cp -a html nhtohtml.pl Makefile README.txt LICENSE.txt TODO nhtohtml-$(VERSION)
	tar czvf $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

clean:
	rm -f html/*.html

.PHONY: html dist


### Makefile ends here
