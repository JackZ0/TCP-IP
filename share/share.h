#ifndef __SHARE_H__
#define __SHARE_H__
#define TEXT_SZ 2048 //申请共享内存大小

struct shared_use_st
{
 int written_by_you; //written_by_you为1时表示有数据写入，为0时表示数据已经被消费者提走
 char some_text[TEXT_SZ];
};

#endif
