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

////----------------------------------------------
//VOID Spw_PCIeEvtIoDefault(IN WDFQUEUE Queue, IN WDFREQUEST Request)//ִ��I/O����
//{
//	PDEVICE_CONTEXT    deviceContext;
//	PULONG			pRAM;//ʲô���ͣ�����
//	//REQUEST_CONTEXT         * transfer;//�ṹ��ָ�� 
//	/*����ṹ������� WDFREQUEST          Request;
//	WDFDMATRANSACTION   DmaTransaction;*/
//	NTSTATUS                  status = STATUS_SUCCESS;
//	WDFMEMORY				  memory;
//	size_t                    length;
//	//WDF_DMA_DIRECTION         direction;
//	//WDFDMATRANSACTION         dmaTransaction;
//	WDF_REQUEST_PARAMETERS    params;
//
//	WDF_REQUEST_PARAMETERS_INIT(&params);
//
//	WdfRequestGetParameters(
//		Request,
//		&params
//		);
//	//params is out
//	//
//	// Get the device extension.
//	//
//	deviceContext = GetDeviceContext(WdfIoQueueGetDevice(Queue));
//	//GetDeviceContext is to change device to context
//	//
//	// Validate and gather parameters.
//	//
//	switch (params.Type) {//Create,Read,Write,IoControl
//
//	case WdfRequestTypeRead:
//		length = params.Parameters.Read.Length;
//		//direction = WdfDmaDirectionReadFromDevice;//FALSE
//		break;
//
//	case WdfRequestTypeWrite:
//		length = params.Parameters.Write.Length;
//		// ��ȡ���뻺����������Ӧ���ڴ����
//		status = WdfRequestRetrieveInputMemory(Request, &memory);
//		if (!NT_SUCCESS(status)) {
//			WdfRequestComplete(Request, status);
//			return ;
//		}
//		pRAM = deviceContext->MemBaseAddress;
//		pRAM += PCIE_WRITE_MEMORY_BASE;
//		//direction = WdfDmaDirectionWriteToDevice;//TRUE
//		if (length > MAXNLEN) length = MAXNLEN;
//		//��Ӧ�ó�������ݿ�����FPGA��PCIE_WRITE_MEMORY_BASE��
//		status = WdfMemoryCopyToBuffer(memory, 0, pRAM, length);//Read is  to use WdfMemoryCopyToBuffer
//		//WRITE_REGISTER_ULONG(pRAM, 0xffff);
//		if (status != STATUS_SUCCESS) {
//			WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
//			return;
//		}
//		WdfRequestCompleteWithInformation(Request, status, length);
//		return;
//		break;
//
//	default:
//		WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
//		return;
//	}
//
//	//
//	// The length must be non-zero.
//	//
//	if (length == 0) {
//		WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
//		return;
//	}
//	return ;
//}
////----------------------------------------------
//VOID
//Spw_PCIeEvtIoWrite(
//IN WDFQUEUE		Queue,
//IN WDFREQUEST	Request,
//IN size_t		Length
//)
//{
//	NTSTATUS		status;
//	WDFMEMORY		memory;
//	WDFDEVICE		device;
//	PDEVICE_CONTEXT pDeviceContext;
//	PULONG			pRAM;//ʲô���ͣ�����
//	ULONG			length;
//
//	// ��ȡ���뻺����������Ӧ���ڴ����
//	status = WdfRequestRetrieveInputMemory(Request, &memory);
//	if (!NT_SUCCESS(status)) {
//		WdfRequestComplete(Request, status);
//		return status;
//	}
//
//	device = WdfIoQueueGetDevice(Queue);
//	pDeviceContext = GetDeviceContext(device);
//
//	pRAM = pDeviceContext->MemBaseAddress;
//	pRAM += PCIE_WRITE_MEMORY_BASE;
//
//	length = Length;
//	if (length > MAXNLEN) length = MAXNLEN;
//
//	//��Ӧ�ó�������ݿ�����FPGA��PCIE_WRITE_MEMORY_BASE��
//	status = WdfMemoryCopyToBuffer(memory, 0, pRAM, length);//Read is  to use WdfMemoryCopyToBuffer
//	//WRITE_REGISTER_ULONG(pRAM, 0xffff);
//	/*
//	SourceMemory [in]
//	A handle to a framework memory object that represents the source buffer.
//	SourceOffset [in]
//	An offset, in bytes, from the beginning of the source buffer. The copy operation begins at the specified offset in the source buffer.
//	Buffer [out]
//	A pointer to a destination buffer.
//	NumBytesToCopyTo [in]
//	The number of bytes to copy from the source buffer to the destination buffer. This value must not be greater than the size of the source buffer
//	*/
//	if (status != STATUS_SUCCESS) {
//		WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
//		return ;
//	}
//	WdfRequestCompleteWithInformation(Request, status, length);
//
//	return;
//}
/*
VOID
Spw_PCIeEvtIoRead(
IN WDFQUEUE		Queue,
IN WDFREQUEST	Request,
IN size_t		Length
)
{
NTSTATUS          status;
PDEVICE_CONTEXT	  pDeviceContext;
WDFDMATRANSACTION dmaTransaction;

pDeviceContext = GetDeviceContext(WdfIoQueueGetDevice(Queue));
dmaTransaction = pDeviceContext->DmaTransaction;

do {
//
// Initialize this new DmaTransaction.
//
//����Request�����������ΪDMA��������
status = WdfDmaTransactionInitializeUsingRequest(
dmaTransaction,
Request,
PCISample_EvtProgramDma,
WdfDmaDirectionReadFromDevice);

if (!NT_SUCCESS(status)) {
DbgPrint("WdfDmaTransactionInitializeUsingRequest failed: %!STATUS!\n", status);
break;
}

//
// Execute this DmaTransaction.
//
status = WdfDmaTransactionExecute(dmaTransaction, WDF_NO_CONTEXT);

if (!NT_SUCCESS(status)) {
DbgPrint("WdfDmaTransactionExecute failed: %!STATUS!\n", status);
break;
}
status = STATUS_SUCCESS;
} while (0);

if (!NT_SUCCESS(status)) {
WdfDmaTransactionRelease(dmaTransaction);
WdfRequestComplete(Request, status);
}

return;
}
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
    //TraceEvents(TRACE_LEVEL_INFORMATION, 
      //          TRACE_QUEUE, 
        //        "!FUNC! Queue 0x%p, Request 0x%p OutputBufferLength %d InputBufferLength %d IoControlCode %d", 
          //      Queue, Request, (int) OutputBufferLength, (int) InputBufferLength, IoControlCode);
	WDFDEVICE device;
	PDEVICE_CONTEXT pDevContext;
	//size_t    InBufferSize = 0;
	//size_t    OutBufferSize = 0;

	NTSTATUS  status;
	
//	WDFMEMORY memory;
	PVOID	  inBuffer;
	PVOID     outBuffer;
//	PVOID     WriteAddress;
//	ULONG     pRAM;

	//PAGED_CODE();
	device = WdfIoQueueGetDevice(Queue);
	pDevContext = GetDeviceContext(device);

	//if (InputBufferLength > 0){
	//	//��ȡI/O��������뻺��ͳ���
	//	status = WdfRequestRetrieveInputBuffer(Request, InputBufferLength, &inBuffer, &InBufferSize);
	//	if (!NT_SUCCESS(status)){
	//		WdfRequestComplete(Request, status);
	//		return;
	//	}
	//}
	//else{
	//	inBuffer = NULL;
	//	InBufferSize = 0;
	//}
	//if (OutputBufferLength > 0){
	//	//��ȡI/O������������ͳ���
	//	status = WdfRequestRetrieveOutputBuffer(Request, OutputBufferLength, &outBuffer, &OutBufferSize);
	//	if (!NT_SUCCESS(status)){
	//		WdfRequestComplete(Request, status);
	//		return;
	//	}
	//}
	//else{
	//	outBuffer = NULL;
	//	OutBufferSize = 0;
	//}
	switch (IoControlCode) {
//����CTL_CODE����������Ӧ�Ĵ���
	case Spw_PCIe_IOCTL_IN_BUFFERED:
			status = WdfRequestRetrieveInputBuffer(
				Request,
				sizeof(ULONG),
				&inBuffer,
				NULL
				);
		//(ULONG *)WriteAddress = (ULONG*)pDevContext->MemBaseAddress + PCIE_WRITE_MEMORY_OFFSET;
			*(ULONG*)WDF_PTR_ADD_OFFSET(pDevContext->BAR0_VirtualAddress, PCIE_WRITE_MEMORY_OFFSET) = *(ULONG*)inBuffer;
			WdfRequestCompleteWithInformation(Request, status, sizeof(ULONG));
			if (!NT_SUCCESS(status)){
				goto Exit;
			}
		break;
		//
	//	// Get input memory.
	//	//
	//	status = WdfRequestRetrieveInputMemory(
	//		Request,
	//		&memory
	//		);
	//	if (!NT_SUCCESS(status)){
	//		goto Exit;
	//	}
	//	inBuffer = pDevContext->MemBaseAddress;
	//	inBuffer = (PULONG)inBuffer + PCIE_WRITE_MEMORY_OFFSET;
	//	InBufferSize = InputBufferLength;
	////	inBuffer += PCIE_WRITE_MEMORY_OFFSET;
	//	if (InBufferSize > MAXNLEN) InBufferSize = MAXNLEN;
	//	//��Ӧ�ó�������ݿ�����FPGA��PCIE_WRITE_MEMORY_BASE��
	//	status = WdfMemoryCopyToBuffer(memory, 0, inBuffer, InBufferSize);
	//	if (status != STATUS_PENDING)
	//		WdfRequestCompleteWithInformation(Request, status, InBufferSize);
	//	break;
	case Spw_PCIe_IOCTL_OUT_BUFFERED:
		status = WdfRequestRetrieveOutputBuffer(
			Request,
			sizeof(ULONG),
			&outBuffer,
			NULL
			);
		//*(ULONG*)outBuffer = pDevContext->MemBaseAddress;//read virtual address
		//*(ULONG*)outBuffer = pDevContext->PhysicalAddressRegister;//read virtual address
		//*(ULONG*)outBuffer = descriptor->u.Memory.Start;
		//*(ULONG*)outBuffer = *(ULONG*)pDevContext->MemBaseAddress + 1;//read data
		//--------------------------------------------------------------------------
		*(ULONG*)outBuffer = *(ULONG*)WDF_PTR_ADD_OFFSET(pDevContext->BAR0_VirtualAddress, PCIE_WRITE_MEMORY_OFFSET);
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
			//
			// Get output memory.
			//
			status = WdfRequestRetrieveOutputBuffer(
			Request,
			sizeof(ULONG),
			&outBuffer,
			NULL
			);
			//*(ULONG*)outBuffer = pDevContext->MemBaseAddress;//read virtual address
			*(ULONG*)outBuffer = pDevContext->PhysicalAddressRegister;//read virtual address
			//*(ULONG*)outBuffer = descriptor->u.Memory.Start;
			//*(ULONG*)outBuffer = *(ULONG*)pDevContext->MemBaseAddress + 1;//read data
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
//
//VOID
//Spw_PCIeEvtIoStop(
//    IN WDFQUEUE Queue,
//    IN WDFREQUEST Request,
//    IN ULONG ActionFlags
//)
//{
// //   TraceEvents(TRACE_LEVEL_INFORMATION, 
//   //             TRACE_QUEUE, 
//     //           "!FUNC! Queue 0x%p, Request 0x%p ActionFlags %d", 
//       //         Queue, Request, ActionFlags);
//
//    //
//    // In most cases, the EvtIoStop callback function completes, cancels, or postpones
//    // further processing of the I/O request.
//    //
//    // Typically, the driver uses the following rules:
//    //
//    // - If the driver owns the I/O request, it calls WdfRequestUnmarkCancelable
//    //   (if the request is cancelable) and either calls WdfRequestStopAcknowledge
//    //   with a Requeue value of TRUE, or it calls WdfRequestComplete with a
//    //   completion status value of STATUS_SUCCESS or STATUS_CANCELLED.
//    //
//    //   Before it can call these methods safely, the driver must make sure that
//    //   its implementation of EvtIoStop has exclusive access to the request.
//    //
//    //   In order to do that, the driver must synchronize access to the request
//    //   to prevent other threads from manipulating the request concurrently.
//    //   The synchronization method you choose will depend on your driver's design.
//    //
//    //   For example, if the request is held in a shared context, the EvtIoStop callback
//    //   might acquire an internal driver lock, take the request from the shared context,
//    //   and then release the lock. At this point, the EvtIoStop callback owns the request
//    //   and can safely complete or requeue the request.
//    //
//    // - If the driver has forwarded the I/O request to an I/O target, it either calls
//    //   WdfRequestCancelSentRequest to attempt to cancel the request, or it postpones
//    //   further processing of the request and calls WdfRequestStopAcknowledge with
//    //   a Requeue value of FALSE.
//    //
//    // A driver might choose to take no action in EvtIoStop for requests that are
//    // guaranteed to complete in a small amount of time.
//    //
//    // In this case, the framework waits until the specified request is complete
//    // before moving the device (or system) to a lower power state or removing the device.
//    // Potentially, this inaction can prevent a system from entering its hibernation state
//    // or another low system power state. In extreme cases, it can cause the system
//    // to crash with bugcheck code 9F.
//    //
//
//    return;
//}

/*++++++DMA & Interrupt
BOOLEAN
PCISample_EvtProgramDma(
IN  WDFDMATRANSACTION       Transaction,
IN  WDFDEVICE               Device,
IN  WDFCONTEXT              Context,
IN  WDF_DMA_DIRECTION       Direction,
IN  PSCATTER_GATHER_LIST    SgList
)
{
	NTSTATUS        status;
	PDEVICE_CONTEXT pDeviceContext;
	ULONG			address;
	ULONG			length;
	PUCHAR			pREG;

	pDeviceContext = GetDeviceContext(Device);
	pREG = pDeviceContext->MemBaseAddress;

	ASSERT(SgList->NumberOfElements == 1);
	ASSERT(SgList->Elements[0].Address.HighPart == 0);

	//
	// Only the first Scatter/Gather element is relevant for packet mode.
	// CY09449 only does 32-bit DMA transfer operations: only low part of
	// physical address is usable.
	//
	address = SgList->Elements[0].Address.LowPart;
	length = SgList->Elements[0].Length;

	if (length > MAXNLEN) length = MAXNLEN;

	// Acquire this device's InterruptSpinLock.
	WdfInterruptAcquireLock(pDeviceContext->Interrupt);

	//����H:DMA�ж�
	WRITE_REGISTER_ULONG((PULONG)(pREG + HINT), 0x2003FF);

	//���漸���������DMA�Ĵ����������ݣ�FromDeviceToMemory

	//L��ַ
	WRITE_REGISTER_USHORT((PUSHORT)(pREG + DMALBASE), RAM);
	//H��ַ
	WRITE_REGISTER_ULONG((PULONG)(pREG + DMAHBASE), address);

	//�ֽڳ��ȣ�0��ʾ����4�ֽڣ�4��ʾ����8�ֽ�
	WRITE_REGISTER_USHORT((PUSHORT)(pREG + DMASIZE), (USHORT)length - 1);

	//����DMA����
	WRITE_REGISTER_USHORT((PUSHORT)(pREG + DMACTL), 0x101);

	// Release our interrupt spinlock
	WdfInterruptReleaseLock(pDeviceContext->Interrupt);

	return TRUE;
}

BOOLEAN
PCISample_EvtInterruptIsr(
IN WDFINTERRUPT Interrupt,
IN ULONG        MessageID
)
{
	PDEVICE_CONTEXT pDeviceContext;
	PUCHAR			pREG;
	USHORT			status;

	pDeviceContext = GetDeviceContext(WdfInterruptGetDevice(Interrupt));
	pREG = pDeviceContext->MemBaseAddress;

	//��ȡ�ж�״̬�Ĵ���ֵ
	status = READ_REGISTER_USHORT((PUSHORT)(pREG + HINT));

	if ((status & 0x020) == 0x020)
	{	//�ж��Ƿ�ΪDMA��������ж�
		//���ǣ��������ֹ�ж�
		WRITE_REGISTER_ULONG((PULONG)(pREG + HINT), 0x3FF);

		//
		// Request the DPC to complete the transfer.
		//
		WdfInterruptQueueDpcForIsr(Interrupt);

		return TRUE;
	}
	else
		return FALSE;	// �����ɸ��豸�������жϣ�����FALSE
}

VOID
PCISample_EvtInterruptDpc(
IN WDFINTERRUPT Interrupt,
IN WDFOBJECT    Device
)
{
	NTSTATUS		  status;
	WDFDEVICE		  device;
	PDEVICE_CONTEXT	  pDeviceContext;
	WDFDMATRANSACTION dmaTransaction;
	BOOLEAN			  transactionComplete;
	WDFREQUEST		  request;
	size_t			  bytesTransferred;

	pDeviceContext = GetDeviceContext(Device);

	dmaTransaction = pDeviceContext->DmaTransaction;
	request = WdfDmaTransactionGetRequest(dmaTransaction);

	//
	// Indicate this DMA operation has completed:
	// This may drive the transfer on the next packet if
	// there is still data to be transfered in the request.
	//
	transactionComplete =
		WdfDmaTransactionDmaCompleted(dmaTransaction, &status);

	if (transactionComplete) {
		//DMA�����ֽ���
		bytesTransferred = WdfDmaTransactionGetBytesTransferred(dmaTransaction);
		//��ֹ����
		WdfDmaTransactionRelease(dmaTransaction);

		WdfRequestCompleteWithInformation(request, status, bytesTransferred);

		DbgPrint("Completing Read request in the DpcForIsr 0x%x\n", bytesTransferred);
	}

	return;
}

-----*/