#! /bin/sh -e
test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

autoreconf --force --install "$srcdir"
"$srcdir/configure" "$@"

echo "--"
echo "autoreconf --force --install"
