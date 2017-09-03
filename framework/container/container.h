/*
 * container.h
 *
 *  Created on: 2017Äê8ÔÂ23ÈÕ
 *      Author: Gift
 */

#ifndef CONTAINER_CONTAINER_H_
#define CONTAINER_CONTAINER_H_

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({			\
	const typeof(((type *)0)->member) * __mptr = (ptr);	\
	(type *)((char *)__mptr - offsetof(type, member)); })

#define __must_be_array(a) (0)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

#endif /* CONTAINER_CONTAINER_H_ */
