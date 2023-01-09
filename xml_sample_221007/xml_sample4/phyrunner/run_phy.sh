#!/bin/sh

rm -f core.*

export LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH}

./wls_sample_phy

