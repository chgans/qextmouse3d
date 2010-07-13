#!/bin/sh
export QTDIR=/opt/Qt
export PATH=$QTDIR/bin:/usr/local/bin:/usr/local/sbin:$PATH
export LD_LIBRARY_PATH=$QTDIR/lib
exec $*
