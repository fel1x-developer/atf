#! /bin/sh
#
# Automated Testing Framework (atf)
#
# Copyright (c) 2007, 2008, 2009 The NetBSD Foundation, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
# CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#
# Generates a header file with information about the revision used to
# build ATF.
#

set -e

Prog_Name=${0##*/}

MTN=
ROOT=

#
# err message
#
err() {
    echo "${Prog_Name}: ${@}" 1>&2
    exit 1
}

#
# call_mtn args
#
call_mtn() {
    ${MTN} --root=${ROOT} "${@}"
}

#
# generate_revision revfile
#
# Creates the revision file 'revfile'.
#
generate_revision() {
    revfile=${1}

    >${revfile}

    base_revision_id=$(call_mtn automate get_base_revision_id)
    echo "#define PACKAGE_REVISION_BASE \"${base_revision_id}\"" >>${revfile}

    if call_mtn status | grep "no changes" >/dev/null; then
        :
    else
        echo "#define PACKAGE_REVISION_MODIFIED 1" >>${revfile}
    fi
}

#
# main
#
# Entry point.
#
main() {
    outfile=
    while getopts :m:r:o: arg; do
        case ${arg} in
            m)
                MTN=${OPTARG}
                ;;
            o)
                outfile=${OPTARG}
                ;;
            r)
                ROOT=${OPTARG}
                ;;
            *)
                err "Unknown option ${arg}"
                ;;
        esac
    done
    [ -n "${ROOT}" ] || \
        err "Must specify the top-level source directory with -r"
    [ -n "${outfile}" ] || \
        err "Must specify an output file with -o"

    if [ -n "${MTN}" -a -d ${ROOT}/_MTN ]; then
        generate_revision ${outfile}
    else
        rm -f ${outfile}
    fi
}

main "${@}"

# vim: syntax=sh:expandtab:shiftwidth=4:softtabstop=4
