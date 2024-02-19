# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html


import sys
import os

# include folder where PyMbs is located
sys.path.insert(0, os.path.abspath(os.path.join(__file__, '../../../')))

import pymbs

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

project = 'pymbs'
version = pymbs.__version__

copyright = '2024, Carsten Knoll, Christian Schubert, '\
            'Jens Frenkel, Sebastian Voigt'

extensions = ['sphinx.ext.autodoc',
              'sphinx.ext.doctest',
              'sphinx.ext.todo',
              'sphinx.ext.coverage',
              'sphinx.ext.imgmath',
              'sphinx.ext.ifconfig',
              'sphinx.ext.mathjax',
              'sphinx.ext.napoleon']

templates_path = ['_templates']
source_suffix = '.rst'
source_encoding = 'utf-8'
master_doc = 'index'
pygments_style = 'sphinx'


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output


html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
htmlhelp_basename = 'pymbs_doc'


# -- Options for LaTeX output --------------------------------------------------

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title, author, documentclass [howto/manual]).
latex_documents = [('index', 'PyMbs.tex', 'PyMbs Documentation',
                    'Carsten Knoll, Christian Schubert, '
                    'Jens Frenkel, Sebastian Voigt', 'manual'),]

# Additional stuff for the LaTeX preamble.
latex_preamble = r"\usepackage{enumitem}\n\setlistdepth{9}"
