#!/bin/sh --noprofile
# This file is protected by Copyright. Please refer to the COPYRIGHT file
# distributed with this source distribution.
#
# This file is part of OpenCPI <http://www.opencpi.org>
#
# OpenCPI is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# OpenCPI is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.

# Note that this script runs on remote/embedded systems and thus may not have access to the full
# development host environment.
# This script is SOURCED at the start of a run script for a platform
#
# Its arguments are: <options> - <ports>
# Options are: run, verify, view, and remote
# Ports are the names of output ports

#echo PARMS: $*
source $OCPI_CDK_DIR/scripts/util.sh
ocpiGetToolPlatform
tput=tput
[ -z "$(which tput)" ] && tput=true 
spec=$1; shift
component=${spec##*.}
platform=$1; shift
while [ $1 != - ]; do
    [ $1 = run ] && run=run
    [ $1 = verify ] && verify=verify
    [ $1 = view ] && view=view
    [ $1 = remote ] && remote=remote
    shift
done
shift # get rid of the dash
# now the args are all ports
ports=($*)
[ -z "$remote" -a -x runremote.sh -a -n "$run" -a -z "$verify" -a -z "$view" ] && {
    # We are: only running, and running locally, and the platform is remote,
    # run all the cases remotely
    ./runremote.sh TestVerbose=$TestVerbose TestTimeout=$TestTimeout Cases="\"$Cases\"" \
		   ./run.sh run remote
    exit $?
}
# docase <model> <worker> <case> <subcase> <timeout> <duration>
function docase {
  [ -z "$Cases" ] || {
     local ok
     set -f
     for c in $Cases; do
       [[ ($c == *.* && $3.$4 == $c) || ($c != *.* && $3 == $c) ]] && ok=1
     done
     set +f
     [ -z "$ok" ] && return 0
  }
  [ -n "$header" ] || {
    header=1
    echo Performing test cases for $spec on platform $platform 1>&2
    echo '  'Functions performed in one pass are: $run $verify $view 1>&2
  }
  r=0
  [ -z "$run" ] || {
    local output outputs timearg
    for o in ${ports[@]}; do
      output=" -pfile_write"
      [ ${#ports[@]} != 1 ] && output+="_from_$o"
      outputs+="$output=fileName=$3.$4.$2.$1.$o.out"
    done
    echo '  'Executing $component test case: "$3.$4" on platform $platform using worker $2.$1... 1>&2
    if [ $5 != 0 ]; then
      timearg=--timeout=$5
    elif [ $6 != 0 ]; then
      timearg=--seconds=$6
    elif [ -n "$TestTimeout" ]; then
      timearg=--timeout=$TestTimeout
    fi
    cmd=('OCPI_LIBRARY_PATH=../../../lib/rcc:../../../lib/ocl:../../gen/assemblies:$OCPI_CDK_DIR/lib/components/rcc' \
         $OCPI_CDK_DIR/bin/$OCPI_TOOL_DIR/ocpirun -d -v -m$component=$1 -w$component=$2 -P$component=$platform \
	         --sim-dir=$3.$4.$2.$1.simulation $timearg \
		 --dump-file=$3.$4.$2.$1.props $outputs ../../gen/applications/$3.$4.xml)
    rm -f -r $3.$4.$2.$1.*
    set -o pipefail
    if [ "$TestVerbose" = 1 ]; then
	out=/dev/stdout
    else
	out=/dev/null
    fi
    setStartTime
    if [ -z "$remote" -a -x runremote.sh ]; then
	# We are local, running interleaved run/verify and platform is remote
	# Remote execution is simply ocpirun
	./runremote.sh "(export TestVerbose=$TestVerbose; echo ${cmd[@]}; time ${cmd[@]})" \
		       2>&1 | tee $3.$4.$2.$1.remote_log > $out
    elif [ -z "$remote" ]; then
 	(echo ${cmd[@]}; eval time env ${cmd[@]}) 2>&1 | tee $3.$4.$2.$1.log > $out
    elif [ "$TestVerbose" = 1 ]; then
 	(echo ${cmd[@]}; eval time env ${cmd[@]}) 2>&1 | tee $3.$4.$2.$1.log
    else
 	(echo ${cmd[@]}; eval time env ${cmd[@]}) > $3.$4.$2.$1.log 2>&1 
    fi
    r=$?
    set +o pipefail
    if [ $r = 0 ]; then
      $tput setaf 2
       echo '    'Execution succeeded, time was $(getElapsedTime).
      $tput sgr0
    else
      $tput setaf 1
      if (( r > 128 )); then
        # Fail immediately if execution stopped on a signal
        let s=r-128
        echo '    'Execution FAILED due to signal $s\; log is in run/$platform/$3.$4.$2.$1.log 1>&2
        $tput sgr0
        exit $r
      fi
      echo '    'Execution FAILED\($r\) - see log in run/$platform/$3.$4.$2.$1.log 1>&2
      $tput sgr0
      failed=1
      return 0
    fi
  }
  [ -z "$view" -a -z "$verify" ] || 
    if [ "$r" = 0 ]; then
      if [ -f $3.$4.$2.$1.props ]; then
        ../../gen/applications/verify_$3.sh $2.$1 $4 $view $verify
	r=$?
        [ -n "$verify" -a $r = 0 -a "$KeepSimulations" != 1 ] && rm -r -f $3.$4.$2.$1.simulation
	if (( r > 128 )); then
	  let s=r-128
	  echo Verification exited with signal $s. 1>&2 
	  exit $r
        fi
      else
        $tput setaf 1
        echo '    'Verification for $3.$4:  FAILED.  No execution using $2.$1 on platform $platform. 1>&2 
        $tput sgr0
      fi
    else
      echo Execution failed so verify or view not performed.
    fi
}
