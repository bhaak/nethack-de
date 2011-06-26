ARCHIV = archiv/nethack-hp-`date +%Y%m%d`.tar.gz

webgen:
	webgen -V 2
	cp nethack-de.ico output/favicon.ico
	rm -f output/nethack-de-updates.rss
	./rss_news.rb --rss src/index.page > output/nethack-de-updates.rss
	find output -type f | xargs chmod 644
	find output -type d | xargs chmod 755

archiv:
	tar cvfz $(ARCHIV) -C output .
	echo "Archiv erstellt"

sourceforge: webgen archiv
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

.PHONY: webgen archiv sourceforge bestiarium kurzhilfe
