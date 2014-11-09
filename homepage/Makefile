ARCHIV = archiv/nethack-hp-`date +%Y%m%d`.tar.gz

nanoc:
	nanoc
	xmllint --noout output/*.html
	nanoc check ilinks
	rm -f output/nethack-de-updates.rss
	./rss_news.rb --rss content/index.textile > output/nethack-de-updates.rss
	find output -type f | xargs chmod 644
	find output -type d | xargs chmod 755

archiv:
	tar cvfz $(ARCHIV) -C output .
	echo "Archiv erstellt"

sourceforge: nanoc archiv
	#scp $(ARCHIV) bhaak@shell.sourceforge.net:~
	scp -r output/* bhaak,nethack-de@web.sourceforge.net:htdocs/
	echo "Homepage nach Sourceforge kopiert"

bestiarium:
	(cd ../nhtohtml ; ./nhtohtml.pl)
	ruby bestiarium.rb

kurzhilfe:
	(cd ../doc ; make Kurzhilfe.pdf Kurzhilfe.html)
	cp ../doc/Kurzhilfe.pdf output/nethack-de-kurzhilfe.pdf
	cp ../doc/Kurzhilfe.html output/nethack-de-kurzhilfe.html
	cp ../doc/Kurzhilfe.css output/

.PHONY: nanoc archiv sourceforge bestiarium kurzhilfe
