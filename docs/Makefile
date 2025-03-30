# Minimal Makefile for Sphinx documentation

SPHINXBUILD   = sphinx-build
SPHINXPROJ    = MicroFMU
SOURCEDIR     = .
BUILDDIR      = _build

.PHONY: help clean html

help:
	@echo "Available targets:"
	@echo "  make html       - Build HTML documentation"
	@echo "  make clean      - Remove all build artifacts"

clean:
	rm -rf $(BUILDDIR)

html:
	$(SPHINXBUILD) -b html $(SOURCEDIR) $(BUILDDIR)/html
