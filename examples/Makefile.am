@LIBCXX_AM@
@LIBCXXW_AM@

EXTRA_DIST=

noinst_PROGRAMS=\
	booklayoutmanager			\
	borderlayoutmanager			\
	busy					\
	checkradio				\
	combobox				\
	copycutpastemenu			\
	customcanvas				\
	customcontainer				\
	customelement				\
	customfontrenderer			\
	customfontrenderer2			\
	customkeyfocus				\
	dateinputfield				\
	filteredinput				\
	filteredinput2				\
	focusable				\
	fontcolorpickers			\
	gridlayoutmanager			\
	helloworld				\
	hierlist				\
	inputfieldsandbuttons			\
	itemlayoutmanager			\
	listlayoutmanager			\
	menu					\
	pagelayoutmanager			\
	panelayoutmanager			\
	passwordinput				\
	peepholelayoutmanager			\
	popupmenu1				\
	popupmenu2				\
	printdialog				\
	progressbar				\
	scrollbar				\
	searchinputfield			\
	singleton				\
	spininput				\
	splash					\
	statictooltip				\
	table					\
	table2				        \
	toolboxlayoutmanager			\
	uigenerator1				\
	uigenerator2				\
	uigenerator3				\
	uigenerator4				\
	uigenerator5				\
	uigenerator6				\
	validatedinput				\
	wordwraplabel

AM_CPPFLAGS=-Wall -Werror

booklayoutmanager_SOURCES=booklayoutmanager.C close_flag.H
booklayoutmanager_LDADD=-lcxxw -lcxx

borderlayoutmanager_SOURCES=borderlayoutmanager.C close_flag.H
borderlayoutmanager_LDADD=-lcxxw -lcxx

$(call THREADMSGDISPATCHER_GEN,busy.inc.H,busy.xml)
busy_SOURCES=busy.C
busy_LDADD=-lcxxw -lcxx

checkradio_SOURCES=checkradio.C close_flag.H
checkradio_LDADD=-lcxxw -lcxx

$(call OPTIONS_GEN,combobox.H,combobox.xml)
combobox_SOURCES=combobox.C close_flag.H
combobox_LDADD=-lcxxw -lcxx

copycutpastemenu_SOURCES=copycutpastemenu.C close_flag.H
copycutpastemenu_LDADD=-lcxxw -lcxx

customcanvas_SOURCES=customcanvas.C close_flag.H
customcanvas_LDADD=-lcxxw -lcxx

customcontainer_SOURCES=customcontainer.C close_flag.H
customcontainer_LDADD=-lcxxw -lcxx

customelement_SOURCES=customelement.C close_flag.H
customelement_LDADD=-lcxxw -lcxx

customfontrenderer_SOURCES=customfontrenderer.C close_flag.H
customfontrenderer_LDADD=-lcxxw -lcxx

customfontrenderer2_SOURCES=customfontrenderer2.C close_flag.H
customfontrenderer2_LDADD=-lcxxw -lcxx

$(call OPTIONS_GEN,customkeyfocus.H,customkeyfocus.xml)
customkeyfocus_SOURCES=customkeyfocus.C close_flag.H
customkeyfocus_LDADD=-lcxxw -lcxx

dateinputfield_SOURCES=dateinputfield.C close_flag.H
dateinputfield_LDADD=-lcxxw -lcxx

filteredinput_SOURCES=filteredinput.C close_flag.H
filteredinput_LDADD=-lcxxw -lcxx

filteredinput2_SOURCES=filteredinput2.C close_flag.H
filteredinput2_LDADD=-lcxxw -lcxx

focusable_SOURCES=focusable.C close_flag.H
focusable_LDADD=-lcxxw -lcxx

fontcolorpickers_SOURCES=fontcolorpickers.C close_flag.H
fontcolorpickers_LDADD=-lcxxw -lcxx

$(call REFCOLLECTION_GEN,gridlayoutmanager.inc.H,gridlayoutmanager.xml)

gridlayoutmanager_SOURCES=gridlayoutmanager.C close_flag.H
gridlayoutmanager_LDADD=-lcxxw -lcxx

helloworld_SOURCES=helloworld.C close_flag.H
helloworld_LDADD=-lcxxw -lcxx

hierlist_SOURCES=hierlist.C close_flag.H
hierlist_LDADD=-lcxxw -lcxx

$(call REFCOLLECTION_GEN,inputfields.inc.H,inputfields.xml)

inputfieldsandbuttons_SOURCES=inputfieldsandbuttons.C close_flag.H
inputfieldsandbuttons_LDADD=-lcxxw -lcxx

itemlayoutmanager_SOURCES=itemlayoutmanager.C close_flag.H
itemlayoutmanager_LDADD=-lcxxw -lcxx -lcourier-unicode

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

passwordinput_SOURCES=passwordinput.C close_flag.H
passwordinput_LDADD=-lcxxw -lcxx

peepholelayoutmanager_SOURCES=peepholelayoutmanager.C close_flag.H
peepholelayoutmanager_LDADD=-lcxxw -lcxx

popupmenu1_SOURCES=popupmenu1.C close_flag.H
popupmenu1_LDADD=-lcxxw -lcxx

popupmenu2_SOURCES=popupmenu2.C close_flag.H
popupmenu2_LDADD=-lcxxw -lcxx

printdialog_SOURCES=printdialog.C close_flag.H
printdialog_LDADD=-lcxxwcups -lcxxw -lcxxcups -lcxx

progressbar_SOURCES=progressbar.C close_flag.H
progressbar_LDADD=-lcxxw -lcxx

scrollbar_SOURCES=scrollbar.C close_flag.H
scrollbar_LDADD=-lcxxw -lcxx

searchinputfield_SOURCES=searchinputfield.C close_flag.H
searchinputfield_LDADD=-lcxxw -lcxx -lcourier-unicode

singleton_SOURCES=singleton.C close_flag.H
singleton_LDADD=-lcxxw -lcxx

$(call OPTIONS_GEN,spininput.H,spininput.xml)
spininput_SOURCES=spininput.C close_flag.H
spininput_LDADD=-lcxxw -lcxx

$(call OPTIONS_GEN,splash.H,splash.xml)
splash_SOURCES=splash.C
splash_LDADD=-lcxxw -lcxx

statictooltip_SOURCES=statictooltip.C close_flag.H
statictooltip_LDADD=-lcxxw -lcxx

table_SOURCES=table.C close_flag.H
table_LDADD=-lcxxw -lcxx

table2_SOURCES=table2.C close_flag.H
table2_LDADD=-lcxxw -lcxx

toolboxlayoutmanager_SOURCES=toolboxlayoutmanager.C close_flag.H
toolboxlayoutmanager_LDADD=-lcxxw -lcxx

uigenerator1_SOURCES=uigenerator1.C close_flag.H
uigenerator1_LDADD=-lcxxw -lcxx
EXTRA_DIST += uigenerator1.xml

uigenerator2_SOURCES=uigenerator2.C close_flag.H
uigenerator2_LDADD=-lcxxw -lcxx
EXTRA_DIST += uigenerator2.xml

uigenerator3_SOURCES=uigenerator3.C close_flag.H
uigenerator3_LDADD=-lcxxw -lcxx
EXTRA_DIST += uigenerator3.xml

uigenerator4_SOURCES=uigenerator4.C close_flag.H
uigenerator4_LDADD=-lcxxw -lcxx
EXTRA_DIST += uigenerator4.xml
$(call EXTRACT_THEME_TEXT_GEN,uigenerator4.i18n.C, uigenerator4.xml)

uigenerator5_SOURCES=uigenerator5.C close_flag.H
uigenerator5_LDADD=-lcxxw -lcxx
EXTRA_DIST += uigenerator5.xml

uigenerator6_SOURCES=uigenerator6.C close_flag.H
uigenerator6_LDADD=-lcxxw -lcxx
EXTRA_DIST += uigenerator6.xml

validatedinput_SOURCES=validatedinput.C close_flag.H
validatedinput_LDADD=-lcxxw -lcxx

wordwraplabel_SOURCES=wordwraplabel.C close_flag.H
wordwraplabel_LDADD=-lcxxw -lcxx
