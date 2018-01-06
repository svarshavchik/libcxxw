@LIBCXX_AM@

noinst_PROGRAMS=\
	booklayoutmanager			\
	busy					\
	checkradio				\
	combobox				\
	focusable				\
	gridlayoutmanager			\
	helloworld				\
	inputfieldsandbuttons			\
	listlayoutmanager			\
	menu					\
	panelayoutmanager			\
	progressbar				\
	scrollbar				\
	pagelayoutmanager			\
	wordwraplabel

booklayoutmanager_SOURCES=booklayoutmanager.C close_flag.H
booklayoutmanager_LDADD=-lcxxw -lcxx

$(call THREADMSGDISPATCHER_GEN,busy.inc.H,busy.xml)
busy_SOURCES=busy.C
busy_LDADD=-lcxxw -lcxx
busy_CXXFLAGS=@STDTHREAD_CXXFLAGS@

checkradio_SOURCES=checkradio.C close_flag.H
checkradio_LDADD=-lcxxw -lcxx

$(call OPTIONS_GEN,combobox.H,combobox.xml)
combobox_SOURCES=combobox.C close_flag.H
combobox_LDADD=-lcxxw -lcxx

focusable_SOURCES=focusable.C close_flag.H
focusable_LDADD=-lcxxw -lcxx

$(call REFCOLLECTION_GEN,gridlayoutmanager.inc.H,gridlayoutmanager.xml)

gridlayoutmanager_SOURCES=gridlayoutmanager.C close_flag.H
gridlayoutmanager_LDADD=-lcxxw -lcxx

helloworld_SOURCES=helloworld.C close_flag.H
helloworld_LDADD=-lcxxw -lcxx

$(call REFCOLLECTION_GEN,inputfields.inc.H,inputfields.xml)

inputfieldsandbuttons_SOURCES=inputfieldsandbuttons.C close_flag.H
inputfieldsandbuttons_LDADD=-lcxxw -lcxx

$(call OPTIONS_GEN,listlayoutmanager.H,listlayoutmanager.xml)
listlayoutmanager_SOURCES=listlayoutmanager.C close_flag.H
listlayoutmanager_LDADD=-lcxxw -lcxx

menu_SOURCES=menu.C close_flag.H
menu_LDADD=-lcxxw -lcxx

$(call OPTIONS_GEN,panelayoutmanager.H,panelayoutmanager.xml)
panelayoutmanager_SOURCES=panelayoutmanager.C close_flag.H
panelayoutmanager_LDADD=-lcxxw -lcxx

pagelayoutmanager_SOURCES=pagelayoutmanager.C close_flag.H
pagelayoutmanager_LDADD=-lcxxw -lcxx

progressbar_SOURCES=progressbar.C close_flag.H
progressbar_LDADD=-lcxxw -lcxx

scrollbar_SOURCES=scrollbar.C close_flag.H
scrollbar_LDADD=-lcxxw -lcxx

wordwraplabel_SOURCES=wordwraplabel.C close_flag.H
wordwraplabel_LDADD=-lcxxw -lcxx
