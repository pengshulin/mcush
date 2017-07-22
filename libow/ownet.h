
#ifndef OWNET_H
#define OWNET_H

#define SMALLINT char
#define uchar unsigned char

SMALLINT owFirst(int portnum, SMALLINT do_reset, SMALLINT alarm_only);
SMALLINT owNext(int portnum, SMALLINT do_reset, SMALLINT alarm_only);
void owSerialNum(int portnum, uchar *serialnum_buf, SMALLINT do_read);

#endif
