#! /bin/sh

home_app=~/app

home_track=${home_app}/track

./configure --prefix=${home_track}

if test -d ${home_track}; then
    rm -rf ${home_track}
fi

make install

make distclean
