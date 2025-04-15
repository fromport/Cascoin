#!/bin/bash

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

cascoinD=${cascoinD:-$SRCDIR/cascoind}
cascoinCLI=${cascoinCLI:-$SRCDIR/cascoin-cli}
cascoinTX=${cascoinTX:-$SRCDIR/cascoin-tx}
cascoinQT=${cascoinQT:-$SRCDIR/qt/cascoin-qt}

[ ! -x $cascoinD ] && echo "$cascoinD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
CASVER=($($cascoinCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for bitcoind if --version-string is not set,
# but has different outcomes for bitcoin-qt and bitcoin-cli.
echo "[COPYRIGHT]" > footer.h2m
$cascoinD --version | sed -n '1!p' >> footer.h2m

for cmd in $cascoinD $cascoinCLI $cascoinTX $cascoinQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${CASVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${CASVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
