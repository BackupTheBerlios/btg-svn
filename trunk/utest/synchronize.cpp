/*
 * btg Copyright (C) 2008 Roman Rybalko.
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
 * $Id$
 */

#include "synchronize.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdio.h> // for perror, printf
#include <unistd.h> // for sleep

// synchronizes launch with other processes
// doesn't allow to launch several processes simultaneously
int synchronize()
{
	int id;
	union semun {
		int     val;    /* Value for SETVAL */
        	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
		unsigned short  *array;  /* Array for GETALL, SETALL */
		struct seminfo  *__buf;  /* Buffer for IPC_INFO
		                            (Linux specific) */
        } u;
	//union semun u;
	
	id = semget(IPC_KEY, 1, IPC_CREAT|IPC_EXCL|0660);
	if (id == -1 && errno == EEXIST)
	{
		id = semget(IPC_KEY, 1, 0);
		if (id == -1)
		{
			perror("semget");
			return 51;
		}
		
		// checking whether it inited
		struct semid_ds s;
		do
		{
			// waiting for init
			sleep(1);
			u.buf = &s;
			if (semctl(id, 0, IPC_STAT, u) == -1)
			{
				perror("semctl");
				return 52;
			}
		}
		while(s.sem_otime == 0);
	}
	else
	{
		// initializing
		u.val = 0;
		if(semctl(id, 0, SETVAL, u) == -1)
		{
			perror("semctl");
			return 53;
		}
		// initialized
	}
	printf("waiting for other testrunner processes ..."); fflush(stdout);
	struct sembuf op[2];
	op[0].sem_num = 0;
	op[0].sem_op = 0;
	op[0].sem_flg = 0;
	op[1].sem_num = 0;
	op[1].sem_op = 1;
	op[1].sem_flg = SEM_UNDO; // will decrease when app terminates
	struct timespec tout;
	tout.tv_nsec = 0;
	tout.tv_sec = 1;
   for(;;)
   {
      // performing operation
      if (semtimedop(id, op, 2, &tout) == -1)
      {
         if (errno == EAGAIN)
         {
            printf("."); fflush(stdout);
            continue;
         }
         perror("semop");
         return 54;
      }
      printf("\n");
      break;
   }
	// when application terminates, the semaphore will be decreased and other application will be unblocked
	return 0;
}
