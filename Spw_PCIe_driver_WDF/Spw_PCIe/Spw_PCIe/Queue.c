#include "driver.h"
#include "queue.tmh"

#pragma warning(disable:4013)  // assuming extern returning int

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, Spw_PCIeEvtIoDeviceControl)

#endif
/*
��һ��Ĭ��I/O���к͵�һ������������EvtIoDefault��KMDF���Ὣ�豸���е������͵�Ĭ��I/O���У�
Ȼ������������������EvtIoDefault����ÿһ������ݽ�����������

*��һ��Ĭ��I/O���кͶ����������������EvtIoRead��EvtIoWrite��EvtIoDeviceControl��KMDF�Ὣ�豸���е������͵�Ĭ��I/O���С�
Ȼ���������������EvtIoRead���������ݽ������󡢵���EvtIoWrite���������ݽ�д���󡢵���EvtIoDeviceControl���������ݽ��豸I/O��������
*/


VOID
Spw_PCIeEvtIoDeviceControl(
    IN WDFQUEUE Queue,
    IN WDFREQUEST Request,
    IN size_t OutputBufferLength,
    IN size_t InputBufferLength,
    IN ULONG IoControlCode
    )
{
	WDFDEVICE device;
	PDEVICE_CONTEXT pDevContext;

	NTSTATUS  status;

	PVOID	  inBuffer;
	PVOID     outBuffer;
	ULONG	  AddressOffset;

	//PAGED_CODE(); do not uncomment this sentence
	device = WdfIoQueueGetDevice(Queue);
	pDevContext = GetDeviceContext(device);

	switch (IoControlCode) {
//����CTL_CODE����������Ӧ�Ĵ���
	case Spw_PCIe_IOCTL_WRITE_OFFSETADDRESS:
		status = WdfRequestRetrieveInputBuffer(
			Request,
			sizeof(ULONG),
			&inBuffer,
			NULL
			);
		pDevContext->OffsetAddressFromApp = *(ULONG*)inBuffer;
		WdfRequestCompleteWithInformation(Request, status, sizeof(ULONG));
		if (!NT_SUCCESS(status)){
			goto Exit;
		}
		break;

	case Spw_PCIe_IOCTL_IN_BUFFERED:
			status = WdfRequestRetrieveInputBuffer(
				Request,
				sizeof(ULONG),
				&inBuffer,
				NULL
				);
			AddressOffset = PCIE_WRITE_MEMORY_OFFSET + pDevContext->OffsetAddressFromApp;
			*(ULONG*)WDF_PTR_ADD_OFFSET(pDevContext->BAR0_VirtualAddress, AddressOffset) = *(ULONG*)inBuffer;
			WdfRequestCompleteWithInformation(Request, status, sizeof(ULONG));
			if (!NT_SUCCESS(status)){
				goto Exit;
			}
		break;
		
	case Spw_PCIe_IOCTL_OUT_BUFFERED:
		status = WdfRequestRetrieveOutputBuffer(
			Request,
			sizeof(ULONG),
			&outBuffer,
			NULL
			);
		AddressOffset = PCIE_WRITE_MEMORY_OFFSET + pDevContext->OffsetAddressFromApp;
		//--------------------------------------------------------------------------
		*(ULONG*)outBuffer = *(ULONG*)WDF_PTR_ADD_OFFSET(pDevContext->BAR0_VirtualAddress, AddressOffset);
		//--------------------------------------------------------------------------
		//*(ULONG*)outBuffer = pDevContext->Counter_i;
		//--------------------------------------------------------------------------
		WdfRequestCompleteWithInformation(Request, status, sizeof(ULONG));
		if (!NT_SUCCESS(status)){
			goto Exit;
		}
		break;
	case Spw_PCIe_IOCTL_READ_PADDRESS:
			//Just think about the size of the data when you are choosing the METHOD.  
		    //METHOD_BUFFERED is typically the fastest for small (less the 16KB) buffers, 
		    //and METHOD_IN_DIRECT and METHOD_OUT_DIRECT should be used for larger buffers than that.
			//METHOD_BUFFERED��METHOD_OUT_DIRECT��METHOD_IN_DIRECT���ַ�ʽ��
			//���뻺������ַ��ͨ������WdfRequestRetrieveInputBuffer�������
			//�����������ַ��ͨ������WdfRequestRetrieveOutputBuffer�������
	
			status = WdfRequestRetrieveOutputBuffer(
			Request,
			sizeof(ULONG),
			&outBuffer,
			NULL
			);

			*(ULONG*)outBuffer = pDevContext->PhysicalAddressRegister;//read BAR0 pysical address

			WdfRequestCompleteWithInformation(Request, status, sizeof(ULONG));
			if (!NT_SUCCESS(status)){
				goto Exit;
			}
		break;

	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		WdfRequestCompleteWithInformation(Request, status, 0);
		break;
	}

Exit:
	if (!NT_SUCCESS(status)) {
		WdfRequestCompleteWithInformation(
			Request,
			status,
			0
			);
	}
    return;
}
