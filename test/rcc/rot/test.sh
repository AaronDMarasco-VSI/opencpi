#! /bin/sh
# Copyright (c) 2009 Mercury Federal Systems.
# 
# This file is part of OpenCPI.
# 
# OpenCPI is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OpenCPI is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with OpenCPI.  If not, see <http://www.gnu.org/licenses/>.

# ----------------------------------------------------------------------
# Test script for the RCC container using the "rot" component.
#
# Revision History:
#
#     06/03/2009 - Frank Pilhofer
#                  Remove "--polled" from test harness command line.
#                  (Use its default.)
#
#     06/03/2009 - Frank Pilhofer
#                  Initial version.
#
# ----------------------------------------------------------------------
#

appFile="./${OUTDIR}/rot.zip"

if test ! -f "$appFile" ; then
    echo "$appFile not found."
    exit 2
fi

CPI_RCC_TEST="${CPIDIR}/tools/local/tester/${OUTDIR}/cpi-rcc-test"

if test ! -x "$CPI_RCC_TEST" ; then
    echo "$CPI_RCC_TEST not found."
    exit 2
fi

cleanup ()
{
    rm -f /tmp/crypt.txt /tmp/clear.txt /dev/shm/cpi-rcc-test
}

trap cleanup EXIT

"$CPI_RCC_TEST" \
    --workerFile=$appFile --entryPoint=rotWorker \
    --inputFile=DataIn=Makefile --outputFile=DataOut=/tmp/crypt.txt \
    --configure=key=13

origsize=`cat Makefile | wc -c`
cryptsize=`cat /tmp/crypt.txt | wc -c`

if test $origsize -ne $cryptsize ; then
    echo "File size differs: $origsize vs $cryptsize."
    exit 1
fi

origsum=`md5sum Makefile | cut -d " " -f 1`
cryptsum=`md5sum /tmp/crypt.txt | cut -d " " -f 1`

if test $origsum = $cryptsum ; then
    echo "Output file is copy of input file."
    exit 1
fi

"$CPI_RCC_TEST" \
    --workerFile=$appFile --entryPoint=rotWorker \
    --inputFile=DataIn=/tmp/crypt.txt --outputFile=DataOut=/tmp/clear.txt \
    --configure=key=13

clearsize=`cat /tmp/clear.txt | wc -c`

if test $origsize -ne $clearsize ; then
    echo "File size differs: $origsize vs $clearsize."
    exit 1
fi

clearsum=`md5sum /tmp/clear.txt | cut -d " " -f 1`

if test $origsum != $clearsum ; then
    echo "Contents differ."
    exit 1
fi
