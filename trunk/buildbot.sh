#! /bin/bash

#
# Buildbot.
#
#
# $Id$
#

#set -x

pidfile="buildbot.pid"
reportfile="buildbot.report"
workdir=".buildbot"
workgen_params="build_parallel $workdir"
[ -x ./.gen_all_builds.sh ] && \
	workgen="./.gen_all_builds.sh $workgen_params" || \
	workgen="./gen_all_builds.pl $workgen_params"
worklaunch="./gen_all_builds_launch.pl $workdir 8"
prelaunch="./buildbot.prelaunch"
subject="BUILD"
mailto="btg@romanr.info"

# setup config
config="$1"
[ -z "$config" ] && config="./buildbot.conf"

# read config
[ -r $config ] && . $config

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
	svn st | grep '^C' >>$reportfile
	report_exit "SVN rev. $oldver - conflicts"
fi

conflicts=`svn up | grep '^C'`
newver=`svnversion`
if [ -n "$conflicts" ] ; then
	echo "++> Conflicts after update. Need human support." >$reportfile
	svn st | grep '^C' >>$reportfile
	report_exit "Update SVN rev. $newver - conflicts"
fi

if [ "$oldver" != "$newver" ] ; then
	rotate $workdir
	$workgen
	[ -x "$prelaunch" ] && "$prelaunch"
	$worklaunch

	>$reportfile

	# we should carefully use parameters with wildcards ($workdir/*.fail)
	# because we can run out of max. cmdline length

	ok_cnt=0
	for ok in $workdir/*.ok ; do
		[ -e $ok ] || continue
		ok_cnt=$(($ok_cnt+1))
	done
	[ $ok_cnt -gt 0 ] && echo "==> Completed successfully: $ok_cnt" >>$reportfile

	>$reportfile.fails
	fail_cnt=0
	for fail in $workdir/*.fail ; do
		[ -e $fail ] || continue
		name=`basename $fail .fail`
		echo "($name)"
		cat $workdir/$name.log
		fail_cnt=$(($fail_cnt+1))
	done >>$reportfile.fails
	if [ $fail_cnt -gt 0 ] ; then
		echo "==> Failed: $fail_cnt"
		cat $reportfile.fails
	fi >>$reportfile
	
	>$reportfile.warnings
	>$reportfile.errors
	for err in $workdir/*/*.err $workdir/*/.*.err ; do
		[ -e $err ] || continue
		cat $err | grep 'warning' >>$reportfile.warnings
		cat $err | grep 'error' >>$reportfile.errors
	done
	sort --unique $reportfile.warnings >$reportfile.warnings.unique
	sort --unique $reportfile.errors >$reportfile.errors.unique
	warn_cnt=`cat $reportfile.warnings.unique | wc -l`
	err_cnt=`cat $reportfile.errors.unique | wc -l`

	if [ $warn_cnt -gt 0 ] ; then
		warn_cnt=$(($warn_cnt)) # strip space
		echo "==> Warnings: $warn_cnt"
		cat $reportfile.warnings.unique
	fi >>$reportfile
	
	if [ $err_cnt -gt 0 ] ; then
		err_cnt=$(($err_cnt)) # strip space
		echo "==> Errors: $err_cnt"
		cat $reportfile.errors.unique
	fi >>$reportfile
	
	rm -f $reportfile.*

	report_exit "Update SVN rev. $newver"
else
	rm -f $reportfile
fi

cleanup_exit
