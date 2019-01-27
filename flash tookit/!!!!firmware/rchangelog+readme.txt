更新记录：

v0.0.0.7s
	功能和编译方式同0.0.0.6s。
	修复webconfig配置Station的问题。
	
v0.0.0.6s
	功能同0.0.0.6，编译连接方式为1024+1024map，版本号带s，区别于512+512map的固件。
	该版本固件要求flash>=2MByte.
	上一版固件无法升级到s版本固件。
	AT+CLDOTA & AT+CIUPDATE开启对1024+1024map固件的升级服务。

v0.0.0.6
    增加webconfig，删除+CHELLO,+CIOADC,+CWWPSSTART,+CWWPSSTOP
	增加IO0复位操作(上电后1s内，拉低IO0，保持3s，模组恢复出产设置AT+RESTORE)
	对于1MByte flash, 带AiCloud2.0的固件在user1下任然可以正常运行，不能升级到user2运行。
	!!!不带s的为512+512map方式固件。!!!
	!!!CIUPDATE更新路径停止512+512map版本升级服务。!!!
	
v0.0.0.5
    内置 AiCloud 2.0 v0.0.5 版本
    修复断网后重连问题

v0.0.0.4
    修复OTA问题
    修复局域网TCP有时无法通讯的问题
    
如何下载：
    1. 该固件已合并 boot_v1.6.bin, user1.bin, esp_init_data_default.bin, blank.bin；
    2. 请使用ESP_DOWNLOAD_TOOL_V2.4 + 以上版本工具下载，下载地址为0x00000；
    3. 如果不知道如何配置Flash选项，请勾选"DoNotChgBin"，这样下载时会按照默认设置下载；
    4. 下载配置可参考 http://wiki.ai-thinker.com/esp_download。
    
    更多信息请访问  http://www.ai-thinker.com/ 或 http://wiki.ai-thinker.com/
    有任何疑问请发送邮件至 support@aithinker.com
    
    安信可科技
    2017-06-02
===================================================================================
Change List：

v0.0.0.7s
	Fix webconfig bugs.
	
v0.0.0.6s
	Modify flash map 1024+1024, flash size>=2Mbyte
	
v0.0.0.6
	Add webconfig, add IO0 restore.
	
v0.0.0.5
    Fix reconnection problem after disconnected

v0.0.0.4
    Fix OTA bug
    Fix LAN TCP communicate bug when on LAN

How to download:

    1. This firmware have already combined boot_v1.6.bin, user1.bin, esp_init_data_default.bin, blank.bin；
    2. Please select the firmware in ESP_DOWNLOAD_TOOL_V2.4 +, The address should write 0x00000;
    3. If you don't know how to config your download panel, please checked the "DoNotChgBin". That will be download the bin as default setting;
    4. Please refer to http://wiki.ai-thinker.com/esp_download if you don't know how to download.
    
    More infomation please visit http://www.ai-thinker.com/ or http://wiki.ai-thinker.com/
    If you have any question, please send your mail to support@aithinker.com 
    
    Ai-Thinker
    2017-03-24