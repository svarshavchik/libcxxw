@LIBCXX_AM@

noinst_PROGRAMS=\
	booklayoutmanager			\
	busy					\
	checkradio				\
	combobox				\
	customcanvas				\
	customelement				\
	customkeyfocus				\
	dateinputfield				\
	focusable				\
	gridlayoutmanager			\
	helloworld				\
	inputfieldsandbuttons			\
	listlayoutmanager			\
	menu					\
	pagelayoutmanager			\
	panelayoutmanager			\
	popupmenu1				\
	popupmenu2				\
	printdialog				\
	progressbar				\
	scrollbar				\
	singleton				\
	spininput				\
	validatedinput				\
	wordwraplabel

AM_CPPFLAGS=-Wall -Werror

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

customcanvas_SOURCES=customcanvas.C close_flag.H
customcanvas_LDADD=-lcxxw -lcxx

customelement_SOURCES=customelement.C close_flag.H
customelement_LDADD=-lcxxw -lcxx

$(call OPTIONS_GEN,customkeyfocus.H,customkeyfocus.xml)
customkeyfocus_SOURCES=customkeyfocus.C close_flag.H
customkeyfocus_LDADD=-lcxxw -lcxx

dateinputfield_SOURCES=dateinputfield.C close_flag.H
dateinputfield_LDADD=-lcxxw -lcxx

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

pagelayoutmanager_SOURCES=pagelayoutmanager.C close_flag.H
pagelayoutmanager_LDADD=-lcxxw -lcxx

$(call OPTIONS_GEN,panelayoutmanager.H,panelayoutmanager.xml)
panelayoutmanager_SOURCES=panelayoutmanager.C close_flag.H
panelayoutmanager_LDADD=-lcxxw -lcxx

popupmenu1_SOURCES=popupmenu1.C close_flag.H
popupmenu1_LDADD=-lcxxw -lcxx

popupmenu2_SOURCES=popupmenu2.C close_flag.H
popupmenu2_LDADD=-lcxxw -lcxx
popupmenu2_CXXFLAGS=@STDTHREAD_CXXFLAGS@

printdialog_SOURCES=printdialog.C close_flag.H
printdialog_LDADD=-lcxxwcups -lcxxw -lcxxcups -lcxx

progressbar_SOURCES=progressbar.C close_flag.H
progressbar_LDADD=-lcxxw -lcxx

scrollbar_SOURCES=scrollbar.C close_flag.H
scrollbar_LDADD=-lcxxw -lcxx

singleton_SOURCES=singleton.C close_flag.H
singleton_LDADD=-lcxxw -lcxx
singleton_CXXFLAGS=@STDTHREAD_CXXFLAGS@

$(call OPTIONS_GEN,spininput.H,spininput.xml)
spininput_SOURCES=spininput.C close_flag.H
spininput_LDADD=-lcxxw -lcxx

validatedinput_SOURCES=validatedinput.C close_flag.H
validatedinput_LDADD=-lcxxw -lcxx

wordwraplabel_SOURCES=wordwraplabel.C close_flag.H
wordwraplabel_LDADD=-lcxxw -lcxx
