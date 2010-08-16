This module holds key constants for the SCA_KeyboardSensor.

.. module:: bge.events

.. function:: EventToString(event)

   Return the string name of a key event. Will raise a ValueError error if its invalid.

   :arg event: key event from bge.keys or the keyboard sensor.
   :type event: int
   :rtype: string
   
.. function:: EventToCharacter(event, shift)

   Return the string name of a key event. Returns an empty string if the event cant be represented as a character.
   
   :type event: int
   :arg event: key event from :mod:`bge.keys` or the keyboard sensor.
   :type shift: bool
   :arg shift: set to true if shift is held.
   :rtype: string

.. data:: LEFTMOUSE
.. data:: MIDDLEMOUSE
.. data:: RIGHTMOUSE
.. data:: WHEELUPMOUSE
.. data:: WHEELDOWNMOUSE
.. data:: MOUSEX
.. data:: MOUSEY

.. data:: AKEY
.. data:: BKEY
.. data:: CKEY
.. data:: DKEY
.. data:: EKEY
.. data:: FKEY
.. data:: GKEY
.. data:: HKEY
.. data:: IKEY
.. data:: JKEY
.. data:: KKEY
.. data:: LKEY
.. data:: MKEY
.. data:: NKEY
.. data:: OKEY
.. data:: PKEY
.. data:: QKEY
.. data:: RKEY
.. data:: SKEY
.. data:: TKEY
.. data:: UKEY
.. data:: VKEY
.. data:: WKEY
.. data:: XKEY
.. data:: YKEY
.. data:: ZKEY


.. data:: ZEROKEY
.. data:: ONEKEY
.. data:: TWOKEY
.. data:: THREEKEY
.. data:: FOURKEY
.. data:: FIVEKEY
.. data:: SIXKEY
.. data:: SEVENKEY
.. data:: EIGHTKEY
.. data:: NINEKEY

.. data:: CAPSLOCKKEY
.. data:: LEFTCTRLKEY
.. data:: LEFTALTKEY
.. data:: RIGHTALTKEY
.. data:: RIGHTCTRLKEY
.. data:: RIGHTSHIFTKEY
.. data:: LEFTSHIFTKEY


.. data:: LEFTARROWKEY
.. data:: DOWNARROWKEY
.. data:: RIGHTARROWKEY
.. data:: UPARROWKEY


.. data:: PAD0
.. data:: PAD1
.. data:: PAD2
.. data:: PAD3
.. data:: PAD4
.. data:: PAD5
.. data:: PAD6
.. data:: PAD7
.. data:: PAD8
.. data:: PAD9
.. data:: PADPERIOD
.. data:: PADSLASHKEY
.. data:: PADASTERKEY
.. data:: PADMINUS
.. data:: PADENTER
.. data:: PADPLUSKEY

.. data:: F1KEY
.. data:: F2KEY
.. data:: F3KEY
.. data:: F4KEY
.. data:: F5KEY
.. data:: F6KEY
.. data:: F7KEY
.. data:: F8KEY
.. data:: F9KEY
.. data:: F10KEY
.. data:: F11KEY
.. data:: F12KEY
.. data:: F13KEY
.. data:: F14KEY
.. data:: F15KEY
.. data:: F16KEY
.. data:: F17KEY
.. data:: F18KEY
.. data:: F19KEY

.. data:: ACCENTGRAVEKEY
.. data:: BACKSLASHKEY
.. data:: BACKSPACEKEY
.. data:: COMMAKEY
.. data:: DELKEY
.. data:: ENDKEY
.. data:: EQUALKEY
.. data:: ESCKEY
.. data:: HOMEKEY
.. data:: INSERTKEY
.. data:: LEFTBRACKETKEY
.. data:: LINEFEEDKEY
.. data:: MINUSKEY
.. data:: PAGEDOWNKEY
.. data:: PAGEUPKEY
.. data:: PAUSEKEY
.. data:: PERIODKEY
.. data:: QUOTEKEY
.. data:: RIGHTBRACKETKEY
.. data:: RETKEY
.. data:: SEMICOLONKEY
.. data:: SLASHKEY
.. data:: SPACEKEY
.. data:: TABKEY
