1�����ݹ�ʦ���ṩ��Ӳ����Ϣ������ƫ�Ƶ�ַ��ʵ�ֵ����������ܹ�ʹ��WindowsΪ�ӿڿ��ṩ��Դ��
2��Ӧ�ó�����WDF�������������⡣

3����Ӧ�ó������ͳ�������������������������������мӷ������������

4��ʹ��IoDeviceControl ����ֵΪ0��Getlasterror = 1 ���ܴ���
5����������˵������δ��������֤��
6��handle�Ƿ��ȡ�ɹ���CreateFile��Ӧ�Ļص�����EvtDeviceFileCreate,CloseHandle��Ӧ�Ļص�����EvtFileCloseû�б�д
7������΢��������Ƿ�Ҳû��EvtDeviceFileCreate��
8����CreateFile�Ĳ����ĳɱ����һ�ԣ�
9���������Ƿ������⣿
//--------------------------------------------------------
Queue.c
status = WdfMemoryCopytoBuffer(memory, 0, pRAM, length);
�ĳ���status = WdfMemoryCopyFromBuffer(memory, 0, pRAM, length);
//--------------------------------------------------------
Driver.c
DriverEntry()���� WDFDRIVER  driver;
��Queue.c�еĹ���queue��ʼ�����������뵽��Device.c��Spw_PCIeCreateDevice�������ˡ�
//--------------------------------------------------------
Device.c
WdfDeviceInitSetIoType�ĳ���WdfDeviceIoBufferedģʽ

//--------------------------------------------------------
��д��ʽ�ĳ�buffer������memory���Ͳ�����������

 