#ifndef MPU_H
#define MPU_H

struct s_mympu {
	float ypr[3];
	float gyro[3];
};

struct l_mympu {  //long型でquatanionの姿勢データを保存する
  long lq[4];
};

extern struct s_mympu mympu;
extern struct l_mympu lmympu;//long型でquatanionの姿勢データを保存する

int mympu_open(unsigned int rate);
int mympu_update();
int lmympu_update();//lmympuをアップデート

#endif

