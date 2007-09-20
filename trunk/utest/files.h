/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * $Id: files.h,v 1.1.2.7 2007/08/04 20:52:36 wojci Exp $
 */

// All files used for testing - that is all files that are actually
// created before running the tests.

#ifndef FILES_H
#define FILES_H

#define TESTFILE_BASE_DIR  "btg_utest"

#define TESTFILE_SHMEM  "btg_utest/shmem_file"
#define TESTFILE_MQ     "btg_utest/test.mque"

#define TESTFILE_DMN    "btg_utest/btgdaemon"

#define TESTFILE_CONFIG "btg_utest/test.config"
#define TESTFILE_WORK   "btg_utest"
#define TESTFILE_FINSH  "btg_utest/finished"

#define TESTFILE_LAST_0  "btg_utest/file0"
#define TESTFILE_LAST_1  "btg_utest/file1"
#define TESTFILE_LAST_2  "btg_utest/file2"
#define TESTFILE_LAST_3  "btg_utest/file3"
#define TESTFILE_LAST_4  "btg_utest/file4"
#define TESTFILE_LAST_5  "btg_utest/file5"
#define TESTFILE_LAST_6  "btg_utest/file6"
#define TESTFILE_LAST_7  "btg_utest/file7"
#define TESTFILE_LAST_8  "btg_utest/file8"
#define TESTFILE_LAST_9  "btg_utest/file9"
#define TESTFILE_LAST_10 "btg_utest/file10"
#define TESTFILE_LAST_11 "btg_utest/file11"

/* Daemon config. */
#define TESTFILE_DAEMON_CONFIG "btg_utest/test.daemon.config"

/* Daemon ip list. */
#define TESTFILE_DAEMON_IPLIST "btg_utest/test.daemon.iplist"

#if BTG_OPTION_SAVESESSIONS
#define TESTFILE_DAEMON_SESSIONSAVE "btg_utest/test.daemon.ss"
#endif // BTG_OPTION_SAVESESSIONS

#define OSFILEOPDIR  "btg_utest/dir1"
#define OSFILEOPFILE "btg_utest/file1"

#define TESTFILE_FILLED  "btg_utest/file.torrent"

#endif // FILES_H
