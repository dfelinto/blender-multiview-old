# CVS
# $Author$
# $Date$
# $RCSfile$
# $Revision$

SCENE 		= test0000
RIBSCENE	= test.rib
POVRAY		= *.pov *.tga
RENDERMAN	= *.rib *.tif
RENDERFILES	= $(POVRAY) $(RENDERMAN)

default:	povray renderman

supersampled:	povray_ss renderman_ss

$(SCENE).pov: povanimdb.py
	python povanimdb.py

$(RIBSCENE): ribanimdb.py
	python ribanimdb.py

povray:		$(SCENE).pov
	povray +V -GA +W300 +H300 +I $(SCENE).pov
	xv $(SCENE).tga &

povray_ss:	$(SCENE).pov
	povray +A +AM2 +V -GA +W300 +H300 +I $(SCENE).pov
	xv $(SCENE).tga &

renderman:	$(RIBSCENE)
	rendrib $(RIBSCENE)
	xv $(SCENE).tif &

renderman_ss:	$(RIBSCENE)
	rendrib -samples 4 4 $(RIBSCENE)
	xv $(SCENE).tif &

clean:
	rm -f *~ *.pyc socket.tmp $(RENDERFILES)
