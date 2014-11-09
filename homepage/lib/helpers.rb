include Nanoc::Helpers::LinkTo

def menu_link(titel, link)
	if (link == 'index.html' && @item_rep.path == '/') ||
		"/#{link}" == @item_rep.path then
		%Q{<li class="webgen-menu-item-selected"><span>#{titel}</span></li>}
	else
		%Q{<li><a href="#{link}">#{titel}</a></li>}
	end
end
