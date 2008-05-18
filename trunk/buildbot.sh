#! /bin/sh

#
# Buildbot.
#
#
# $Id$
#

set -x

pidfile="buildbot.pid"
reportfile="buildbot.report"
workdir=".buildbot"
workgen_params="build_parallel $workdir"
[ -x ./.gen_all_builds.sh ] && \
	workgen="./.gen_all_builds.sh $workgen_params" || \
	workgen="./gen_all_builds.pl $workgen_params"
worklaunch="./gen_all_builds_launch.pl $workdir 8"
subject="BUILD"
mailto="btg@romanr.info"

[ -r ./buildbot.conf ] && . ./buildbot.conf

# check pid (ugly)
if [ -e "$pidfile" ] ; then
	read PID <"$pidfile"
	if [ -n "$PID" ] ; then
		if kill -0 $PID ; then
			echo "Already running"
			exit 0
		fi
	fi
fi
echo -n $$ >"$pidfile"

function cleanup_exit()
{
	rm -f $pidfile
	exit 0
}

function report()
{
	subject_add=$1
	for rcpt in $mailto ; do
		cat $reportfile | mutt \
			-s "$subject $subject_add" \
			"$rcpt"
			# -a attachment
	done
}

function report_exit()
{
	report "$@"
	cleanup_exit
}

function rotate()
{
	obj=$1
	rm -Rf $obj.7
	mv -f $obj.6 $obj.7
	mv -f $obj.5 $obj.6
	mv -f $obj.4 $obj.5
	mv -f $obj.3 $obj.4
	mv -f $obj.2 $obj.3
	mv -f $obj.1 $obj.2
	mv -f $obj $obj.1
}

trap cleanup_exit SIGINT
trap cleanup_exit SIGTERM

oldver=`svnversion`
conflicts=`svn st | grep '^C'`
if [ -n "$conflicts" ] ; then
	echo "++> Conflicts. Need human support." >$reportfile
	svn st >>$reportfile
	report_exit "SVN rev. $oldver - conflicts"
fi

conflicts=`svn up | grep '^C'`
newver=`svnversion`
if [ -n "$conflicts" ] ; then
	echo "++> Conflicts after update. Need human support." >$reportfile
	svn st >>$reportfile
	report_exit "Update SVN rev. $newver - conflicts"
fi

if [ "$oldver" != "$newver" ] ; then
	rotate $workdir
	$workgen
	$worklaunch
	echo "==> Completed. ($?)" >$reportfile
	echo "==> Failed:" >>$reportfile
	# we shouldn't use constructions like ls $workdir/*.fail here
	# because we can run out of max. cmdline length
	for fail in $workdir/*.fail ; do
		[ -e $fail ] || continue
		name=`basename $fail .fail`
		echo "($name)" >>$reportfile
		cat $workdir/$name.log >>$reportfile
	done
	echo "==> Warnings:" >>$reportfile
	>$reportfile.warnings
	for err in $workdir/*/*.err ; do
		[ -e $err ] || continue
		cat $err | grep 'warning' >>$reportfile.warnings
	done
	cat $reportfile.warnings | sort --unique >>$reportfile
	rm -f $reportfile.warnings
	report_exit "Update SVN rev. $newver"
else
	rm -f $reportfile
fi

cleanup_exit
