#!/usr/bin/env sh

exec openocd \
    -f /board/st_nucleo_l4.cfg \
    -c "program $(printf '%q' "$1") preverify verify reset exit"
