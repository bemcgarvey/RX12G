#include "bootloaderusblink.h"
#include <Windows.h>
#include <SetupAPI.h>
extern "C"
{
#include <hidsdi.h>
}
#include <Dbt.h>

BootLoaderUSBLink::BootLoaderUSBLink()
    : handle(INVALID_HANDLE_VALUE) {}

BootLoaderUSBLink::~BootLoaderUSBLink() { closeHandles(); }

void BootLoaderUSBLink::Open(uint16_t pid, uint16_t vid) {
  HDEVINFO deviceInfo = INVALID_HANDLE_VALUE;
  GUID guid;
  SP_DEVICE_INTERFACE_DATA deviceInfoData;
  DWORD i = 0;

  devicePath = "";
  HidD_GetHidGuid(&guid);
  deviceInfo = SetupDiGetClassDevs(&guid, NULL, NULL,
                                   (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
  if (deviceInfo == INVALID_HANDLE_VALUE) {
    return;
  }
  deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
  while (
      SetupDiEnumDeviceInterfaces(deviceInfo, 0, &guid, i, &deviceInfoData)) {
    DWORD requiredSize;
    SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInfoData, NULL, 0,
                                    &requiredSize, NULL);
    PSP_DEVICE_INTERFACE_DETAIL_DATA functionClassDeviceData;
    functionClassDeviceData =
        (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
    functionClassDeviceData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    if (SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInfoData,
                                        functionClassDeviceData, requiredSize,
                                        &requiredSize, NULL)) {
      HANDLE hidDevice;
      hidDevice = CreateFile(functionClassDeviceData->DevicePath,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                             OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
      if (hidDevice != INVALID_HANDLE_VALUE) {
        HIDD_ATTRIBUTES attributes;
        attributes.Size = sizeof(HIDD_ATTRIBUTES);
        HidD_GetAttributes(hidDevice, &attributes);
        if (pid == attributes.ProductID && vid == attributes.VendorID) {
          handle = hidDevice;
          devicePath = QString::fromWCharArray(
                           functionClassDeviceData->DevicePath).toLower();
          free(functionClassDeviceData);
          break;
        } else {
          CloseHandle(hidDevice);
        }
      }
      free(functionClassDeviceData);
    }
    ++i;
  }
  if (deviceInfo != INVALID_HANDLE_VALUE) {
    SetupDiDestroyDeviceInfoList(deviceInfo);
  }
}

bool BootLoaderUSBLink::WriteDevice(uint8_t *buffer, int len, int wait_ms) {
  DWORD actualLen;
  int status;
  HANDLE hEventObject;
  OVERLAPPED HIDOverlapped;
  uint8_t report[65];

  if (handle == INVALID_HANDLE_VALUE) {
    return false;
  }
  hEventObject = CreateEvent((LPSECURITY_ATTRIBUTES)NULL, FALSE, TRUE, L"");
  HIDOverlapped.hEvent = hEventObject;

  do {
    HIDOverlapped.Offset = 0;
    HIDOverlapped.OffsetHigh = 0;
    report[0] = 0;
    memcpy(&report[1], buffer, 64);
    status = WriteFile(handle, report, 65, &actualLen, &HIDOverlapped);
    (void) status;
    status = WaitForSingleObject(hEventObject, wait_ms);
    switch (status) {
      case WAIT_OBJECT_0:
        // Success;
        break;
      case WAIT_TIMEOUT:
        // Timeout error;
        // Cancel the read operation.
        CancelIo(handle);
        return false;
      default:
        // Undefined error;
        // Cancel the read operation.
        CancelIo(handle);
        return false;
    }
    len -= 64;
    buffer += 64;
  } while (len > 0);
  return true;
}

bool BootLoaderUSBLink::ReadDevice(uint8_t *buffer, int wait_ms) {
  DWORD len;
  int status;
  HANDLE hEventObject;
  OVERLAPPED HIDOverlapped;
  uint8_t report[65];

  if (handle == INVALID_HANDLE_VALUE) {
    return false;
  }
  hEventObject = CreateEvent((LPSECURITY_ATTRIBUTES)NULL, FALSE, TRUE, L"");
  HIDOverlapped.hEvent = hEventObject;
  HIDOverlapped.Offset = 0;
  HIDOverlapped.OffsetHigh = 0;
  report[0] = 0;
  ReadFile(handle, report, 65, &len, &HIDOverlapped);
  status = WaitForSingleObject(hEventObject, wait_ms);
  switch (status) {
    case WAIT_OBJECT_0:
      // Success;
      // Use the report data;
      memcpy(buffer, &report[1], 64);
      return true;
    case WAIT_TIMEOUT:
      // Timeout error;
      // Cancel the read operation.
      CancelIo(handle);
      break;
    default:
      // Undefined error;
      // Cancel the read operation.
      CancelIo(handle);
      break;
  }
  return false;
}

bool BootLoaderUSBLink::Connected(void) {
  return handle != INVALID_HANDLE_VALUE;
}

void BootLoaderUSBLink::Close(void) { closeHandles(); }

void BootLoaderUSBLink::closeHandles(void) {
  if (handle != INVALID_HANDLE_VALUE) {
    CloseHandle(handle);
    handle = INVALID_HANDLE_VALUE;
  }
}

QString BootLoaderUSBLink::getDevicePath() const { return devicePath; }
