<%namespace name="util" module="util"/>
project = 'Blender'
copyright = u'Blender Foundation'
version = '${util.get_version_string()} - UNSTABLE API'
release = '${util.get_version_string()} - UNSTABLE API'
html_theme = 'blender-org'
html_theme_path = ['../']
html_favicon = 'favicon.ico'
html_copy_source = False

latex_documents = [ ('contents', 'contents.tex', 'Blender Index', 'Blender Foundation', 'manual'), ]
latex_paper_size = 'a4paper'
