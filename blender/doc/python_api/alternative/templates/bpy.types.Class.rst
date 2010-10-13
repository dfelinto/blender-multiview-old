<%namespace name="layout" file="Layout.rst"/>
<%namespace name="util" module="util"/>
<%
references = _class.seealsos
subclasses = util.get_subclasses(_class)
_class.seealsos = []
%>
${layout.class_header(_class)}
${layout.class_footer(_class)}

% if subclasses:
.. rubric:: Subclasses

% for subclass in subclasses:
* :class:`${subclass}`
% endfor

% endif
% if references:
.. rubric:: References

% for ref in references:
* :class:`${ref}`
% endfor

% endif
