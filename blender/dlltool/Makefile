include nan_definitions.mk

all: ${NAN_GETTEXT}/lib/freegettext.a  ${NAN_ICONV}/lib/freeiconv.a \
		${NAN_PYTHON}/lib/freepy.a

debug: all

${NAN_GETTEXT}/lib/freegettext.a: gettext.def \
			${NAN_GETTEXT}/lib/gnu_gettext.dll
	cp ${NAN_GETTEXT}/lib/gnu_gettext.dll .
	dlltool.exe -v --output-lib ${NAN_GETTEXT}/lib/freegettext.a \
		--input-def gettext.def \
		--dllname gnu_gettext.dll
	ranlib ${NAN_GETTEXT}/lib/freegettext.a
	rm ./gnu_gettext.dll

${NAN_ICONV}/lib/freeiconv.a: iconv.def ${NAN_ICONV}/lib/iconv.dll
	cp ${NAN_ICONV}/lib/iconv.dll .
	dlltool.exe -v --output-lib ${NAN_ICONV}/lib/freeiconv.a \
		--input-def iconv.def \
		--dllname iconv.dll
	ranlib ${NAN_ICONV}/lib/freeiconv.a
	rm ./iconv.dll

${NAN_PYTHON}/lib/freepy.a: python.def ${NAN_PYTHON}/lib/python22.dll
	cp ${NAN_PYTHON}/lib/python22.dll .
	dlltool.exe -v --output-lib ${NAN_PYTHON}/lib/freepy.a \
		--input-def python.def \
		--dllname python22.dll
	ranlib ${NAN_PYTHON}/lib/freepy.a
	rm ./python22.dll

clean:
	$(RM) ${NAN_PYTHON}/lib/freepy.a ${NAN_GETTEXT}/lib/freegettext.a \
		${NAN_ICONV}/lib/freeiconv.a
