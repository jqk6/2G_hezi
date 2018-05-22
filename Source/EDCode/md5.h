//
// Created by YangYongbao on 2017/3/27.
//

#ifndef FIRMWARE_MD5_H
#define FIRMWARE_MD5_H

/**
 * 获取32位MD5校验值
 * @param encrypt 要校验的数据
 * @param encrypt_len 要校验的数据长度
 * @param result 生成的MD5校验值
 */
extern	void get_md5_32_str(unsigned char* encrypt, unsigned int encrypt_len, unsigned char* result);
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
void MD5Transform(unsigned int state[4],unsigned char block[64]);
#endif //FIRMWARE_MD5_H
