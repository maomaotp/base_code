#ifndef _GetConfigInfo_H__
#define _GetConfigInfo_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
using namespace std;
#define ErrSize 2*256
#define LocalBuffSize 1024

/*FIXME
 *支持的配置文件格式：
 *'-> 	NET_IP=172.16.91.25 	
 *'->		NET_IP 172.16.91.25
 *'-> 	NET_IP     172.16.91.25
 *'->		NET_IP =     172.16.91.25
 .(注2:能够处理NET_IP,=，192.168.91.25中间含有多个空格)
 *'->		NET_IP=    (IP地址没写的处理后显示为空)
 其中=可替换为空格
 如果含有多个相同字段,默认取第一个
 *'->等等
 */

/*
 * 数据结构	： 返回的数据结构
 * autodyne	:  数据容器
 * err:		    出错信息
 * FIXME 注意：autodyne或者err均在GetConfigInfo中分配内存空间,所以需要在调用完GetConfigInfo后手动释放autodyne或者err
 */
union ConfigInfo
{
		char *autodyne;
		char *err;
};

/*
 * 函数功能： 	
 * 		获取配置文件中某一值
 * 参数含义：
 * 		ConfigPath:		配置文件路径
 * 		key		:	要查找信息对应的关键字			
 * 		separtor	: 	分割符
 * 		BackInfo:		存放获取到的值或出错信息 
 * 返回值：
 * 		true			:		信息获取成功，err为空
 * 		false			:		信息获取失败，详情见err
 */
bool GetTipsConfigInfo(char *ConfigPath,char *key,char *separtor,union ConfigInfo *BackInfo);

/*
 * 函数功能： 	
 * 		释放用于存放返回信息或出错信息的内存
 * 参数含义：
 * 		BackInfo:		存放获取到的值或出错信息 
 * 返回值：
 * 		true			:		释放成功
 * 		false			:		释放失败
 */
bool ReleaseInfoSpace(union ConfigInfo *BackInfo);

#endif
