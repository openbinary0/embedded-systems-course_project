#!/usr/bin/env sh

exec pandoc README.md \
    --pdf-engine=xelatex \
	-o SPEC.pdf \
	-V colorlinks=true \
	-V linkcolor=blue \
	-V urlcolor=blue \
	-V monofont="JetBrainsMonoNL Nerd Font"
