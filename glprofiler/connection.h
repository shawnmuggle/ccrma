/*
 *  connection.h
 *  GLProfiler
 *
 *  Created by handley on 7/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CONNECTION_H
#define CONNECTION_H

int InitProfilerConnection();
void CloseProfilerConnection();

uint32_t SendData(const void *data, unsigned int size, unsigned int msgID);
uint32_t SendDataWithExtra(const void *data, unsigned int size, unsigned int msgID, const void *extraData, uint32_t extraDataSize);

void AddDeleteBuffer(uint32_t count, const GLuint *buffers);

extern int shouldDelayDeletions;

#endif
