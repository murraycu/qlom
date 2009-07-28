#! /bin/sh -e
test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

autoreconf --force --install "$srcdir"
"$srcdir/configure" "$@"

echo "--"
echo "autoreconf --force --install"
echo "--"
echo "Remember it well, because one day, autogen.sh may be replaced with an"
echo "even smaller shell script."
