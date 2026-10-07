#!/usr/bin/env sh

exec pandoc README.md \
    --pdf-engine=xelatex \
	-o SPEC.pdf \
	-V monofont="JetBrainsMonoNL Nerd Font"
