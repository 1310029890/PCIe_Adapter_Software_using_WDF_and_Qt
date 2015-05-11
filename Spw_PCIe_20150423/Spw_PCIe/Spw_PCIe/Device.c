#include "driver.h"
#include "device.tmh"

#pragma warning(disable:4013)  // assuming extern returning int
#ifdef ALLOC_PRAGMA
//#pragma alloc_text(PAGE, Spw_PCIeCreateDevice)
//#pragma alloc_text(PAGE, Spw_PCIeEvtDriverContextCleanup)

#pragma alloc_text(PAGE, Spw_PCIeEvtDevicePrepareHardware)
#pragma alloc_text(PAGE, Spw_PCIeEvtDeviceReleaseHardware)
#pragma alloc_text(PAGE, Spw_PCIeEvtDeviceD0Entry)
#pragma alloc_text(PAGE, Spw_PCIeEvtDeviceD0Exit)
//#pragma alloc_text(PAGE, Spw_PCIeEvtIoWrite)

#endif

NTSTATUS
Spw_PCIeEvtDevicePrepareHardware(
IN WDFDEVICE Device,
IN WDFCMRESLIST ResourceList,
IN WDFCMRESLIST ResourceListTranslated
)
{
	ULONG		    i;
	NTSTATUS        status = STATUS_SUCCESS;
	PDEVICE_CONTEXT pDeviceContext = NULL;
	/*
	BOOLEAN                             foundPort      = FALSE;
    PHYSICAL_ADDRESS                    portBasePA     = {0};
    ULONG                               portCount      = 0;
    WDF_DMA_ENABLER_CONFIG              dmaConfig;
	*/
	PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor;//��¼��ΪPCI�豸�����Ӳ����Դ


	/*
	��Windows���������У�PCM_PARTIAL_RESOURCE_DESCRIPTOR��¼��ΪPCI�豸�����Ӳ����Դ��
	������CmResourceTypePort, CmResourceTypeMemory�ȣ�
	���߱�ʾһ��memory��ַ�ռ䣬����˼�壬��ͨ��memory space���ʵģ�
	ǰ�߱�ʾһ��I/O��ַ�ռ䣬����flag��CM_RESOURCE_PORT_MEMORY��CM_RESOURCE_PORT_IO���֣�
	�ֱ��ʾͨ��memory space�����Լ�ͨ��I/O space���ʣ������PCI������ʵ�ʷ���Ĳ��죬
	��x86�£�CmResourceTypePort��flag����CM_RESOURCE_PORT_IO��������PCI�豸�������I/O��ַ�ռ䣬�����Ҳ��I/O��ַ�ռ䣬
	����ARM��Alpha���£�flag��CM_RESOURCE_PORT_MEMORY��������ʹPCI�����I/O��ַ�ռ䣬����������memory space��
	������Ҫͨ��memory space����I/O�豸(ͨ��MmMapIoSpaceӳ�������ַ�ռ䵽�����ַ�ռ䣬��Ȼ�����ں˵������ַ�ռ䣬���������Ϳ������������豸��)��
	*/
	PAGED_CODE();

//	UNREFERENCED_PARAMETER(Resources);//���߱�������Ҫ����Resourcesû�б����õľ���

//	DbgPrint("EvtDevicePrepareHardware - begins\n");

	pDeviceContext = GetDeviceContext(Device);
	pDeviceContext->MemBaseAddress = NULL;

	//ͨ�����·�ʽ��ȡ�ڴ�ռ�
	for (i = 0; i < WdfCmResourceListGetCount(ResourceListTranslated); i++) {

		descriptor = WdfCmResourceListGetDescriptor(ResourceListTranslated, i);
		//���ʧ�ܣ�
		if (!descriptor) {
			return STATUS_DEVICE_CONFIGURATION_ERROR;
		}

		switch (descriptor->Type) {

		case CmResourceTypeMemory:
			//MmMapIoSpace�������ַת����ϵͳ�ں�ģʽ��ַ
			pDeviceContext->MemBaseAddress = MmMapIoSpace(
				descriptor->u.Memory.Start,
				descriptor->u.Memory.Length,
				MmNonCached);
			pDeviceContext->MemLength = descriptor->u.Memory.Length;

			break;

		default:
			break;
		}
		if (!pDeviceContext->MemBaseAddress){
			return STATUS_INSUFFICIENT_RESOURCES;
		}
	}

	DbgPrint("EvtDevicePrepareHardware - ends\n");

	return STATUS_SUCCESS;
}

NTSTATUS
Spw_PCIeEvtDeviceReleaseHardware(
IN WDFDEVICE Device,
IN WDFCMRESLIST ResourceListTranslated
)
{
	PDEVICE_CONTEXT	pDeviceContext = NULL;

	PAGED_CODE();

	DbgPrint("EvtDeviceReleaseHardware - begins\n");

	pDeviceContext = GetDeviceContext(Device);

	if (pDeviceContext->MemBaseAddress) {
		//MmUnmapIoSpace��������ַ��ϵͳ�ں�ģʽ��ַ�Ĺ���
		MmUnmapIoSpace(pDeviceContext->MemBaseAddress, pDeviceContext->MemLength);
		pDeviceContext->MemBaseAddress = NULL;
	}

	DbgPrint("EvtDeviceReleaseHardware - ends\n");

	return STATUS_SUCCESS;
}

NTSTATUS
Spw_PCIeEvtDeviceD0Entry(
_In_ IN  WDFDEVICE Device,
_In_ IN  WDF_POWER_DEVICE_STATE PreviousState
)
{
	UNREFERENCED_PARAMETER(Device);
	UNREFERENCED_PARAMETER(PreviousState);

	return STATUS_SUCCESS;
}


NTSTATUS
Spw_PCIeEvtDeviceD0Exit(
_In_ IN  WDFDEVICE Device,
_In_ IN  WDF_POWER_DEVICE_STATE TargetState
)
{
	UNREFERENCED_PARAMETER(Device);
	UNREFERENCED_PARAMETER(TargetState);

	PAGED_CODE();

	return STATUS_SUCCESS;
}

/*+++++++DMA
NTSTATUS
InitializeDMA(
IN WDFDEVICE Device
)
{
NTSTATUS				status;
PDEVICE_CONTEXT			pDeviceContext;
WDF_DMA_ENABLER_CONFIG	dmaConfig;

PAGED_CODE();

pDeviceContext = GetDeviceContext(Device);

//
// DMA_TRANSFER_ELEMENTS must be 16-byte aligned
//
//����DMA���ݻ�������ַ�߽磺16�ֽڶ���
WdfDeviceSetAlignmentRequirement( Device, FILE_OCTA_ALIGNMENT );

//
// Create a new DMA Enabler instance.
//

//����һ��DMA������
WDF_DMA_ENABLER_CONFIG_INIT( &dmaConfig,
WdfDmaProfilePacket,
MAXNLEN );

status = WdfDmaEnablerCreate( Device,
&dmaConfig,
WDF_NO_OBJECT_ATTRIBUTES,
&pDeviceContext->DmaEnabler );

if (!NT_SUCCESS (status)) {
DbgPrint("WdfDmaEnablerCreate failed: %!STATUS!\n", status);
return status;
}

//
// Create a new DmaTransaction.
//
//����һ��DMA����
status = WdfDmaTransactionCreate( pDeviceContext->DmaEnabler,
WDF_NO_OBJECT_ATTRIBUTES,
&pDeviceContext->DmaTransaction );

if(!NT_SUCCESS(status)) {
DbgPrint("WdfDmaTransactionCreate failed: %!STATUS!\n", status);
}

return status;
}
------*/
