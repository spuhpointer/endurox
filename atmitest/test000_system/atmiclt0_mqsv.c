/**
 * @brief Simple message queue server process
 *
 * @file atmiclt0_mqsv.c
 */
/* -----------------------------------------------------------------------------
 * Enduro/X Middleware Platform for Distributed Transaction Processing
 * Copyright (C) 2009-2016, ATR Baltic, Ltd. All Rights Reserved.
 * Copyright (C) 2017-2018, Mavimax, Ltd. All Rights Reserved.
 * This software is released under one of the following licenses:
 * GPL or Mavimax's license for commercial use.
 * -----------------------------------------------------------------------------
 * GPL license:
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * -----------------------------------------------------------------------------
 * A commercial use license is available from Mavimax, Ltd
 * contact@mavimax.com
 * -----------------------------------------------------------------------------
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys_mqueue.h>
#include "test000.h"

/*---------------------------Externs------------------------------------*/
/*---------------------------Macros-------------------------------------*/
/*---------------------------Enums--------------------------------------*/
/*---------------------------Typedefs-----------------------------------*/
/*---------------------------Globals------------------------------------*/
/*---------------------------Statics------------------------------------*/
/*---------------------------Prototypes---------------------------------*/

/**
 * This will open some message queue (say, "test000_server") and will provide responses
 * back to "test000_client".
 * We will use standard API here via sys_mqueue.h
 * @param argc
 * @param argv
 * @return 
 */
int main( int argc , char **argv )
{
    int ret = EXSUCCEED;
    mqd_t mq = (mqd_t)EXFAIL;
    struct mq_attr attr;
    char buffer[TEST_REPLY_SIZE];
    int must_stop = 0;
    struct   timespec tm;
    
    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = TEST_REPLY_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    if ((mqd_t)EXFAIL==(mq = ndrx_mq_open(SV_QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr)))
    {
        NDRX_LOG(log_error, "Failed to open queue: [%s]: %s", 
                SV_QUEUE_NAME, strerror(errno));
        EXFAIL_OUT(ret);
    }
    
    do 
    {
        ssize_t bytes_read;

        /* receive the message 
         * Maybe have some timed receive
         */
        clock_gettime(CLOCK_REALTIME, &tm);
        tm.tv_sec += 5;  /* Set for 20 seconds */

        NDRX_LOG(log_debug, "About to RCV!");
        
        if (EXFAIL==(bytes_read=ndrx_mq_timedreceive(mq, buffer, 
                TEST_REPLY_SIZE, NULL, &tm)))
        {
            NDRX_LOG(log_error, "Failed to get message: %s", strerror(errno));
            EXFAIL_OUT(ret);
        }
        
        NDRX_LOG(log_debug, "Read bytes: %d", bytes_read);
        
        if (100==buffer[15])
        {
            must_stop = EXTRUE;
        }
        
    } while (!must_stop);

out:
    /* cleanup */
    
    if ((mqd_t)EXFAIL!=mq && EXFAIL==ndrx_mq_close(mq))
    {
        NDRX_LOG(log_error, "Failed to close queue: %s", tpstrerror(errno));
    }

    return ret;
}


/* vim: set ts=4 sw=4 et smartindent: */