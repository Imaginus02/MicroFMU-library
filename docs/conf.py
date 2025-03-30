import os
import sys

project = 'MicroFMU Documentation'
author = 'Tom Reynaud'
release = '1.0'
language = 'en'
copyright = '%Y, Tom Reynaud'

# Paths setup
sys.path.insert(0, os.path.abspath('../'))

# Exclude patterns
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

html_theme = 'sphinx_rtd_theme'
