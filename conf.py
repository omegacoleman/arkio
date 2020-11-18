# mcss doxygen config

DOXYFILE = 'Doxyfile-mcss'

STYLESHEETS = [
    '../css/m-dark+documentation.compiled.css'
]
THEME_COLOR = '#22272e'
FAVICON = 'favicon-dark.png'

LINKS_NAVBAR1 = [
    ("Modules", 'modules', []),
    ("Pages", 'pages', []),
    ("Examples", 'page_examples', [])
]

LINKS_NAVBAR2 = [
    ("<a href=\"https://github.com/omegacoleman/arkio\">GitHub</a>", [])
]

FINE_PRINT = ""

SHOW_UNDOCUMENTED = True

