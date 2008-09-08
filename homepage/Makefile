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
	scp $(ARCHIV) bhaak@shell.sourceforge.net:~
	echo "Archiv nach Sourceforge kopiert"

.PHONY: webgen archiv sourceforge
