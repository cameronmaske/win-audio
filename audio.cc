#include <nan.h>
#include <stdio.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <math.h>
#include <node.h>
#include <functiondiscoverykeys_devpkey.h>



using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Integer;
using v8::Object;
using v8::Value;
using namespace std;

Local<Value> encode(const std::wstring& str) {
    return Nan::Encode(
        str.c_str(),
        str.length() * sizeof(wchar_t),
        Nan::Encoding::UCS2
    );
}

IMMDevice* getDeviceById(LPCWSTR id) {  
  IMMDeviceEnumerator *enumerator = NULL;
  IMMDevice *device = NULL;
  HRESULT hr;

  // Initializes the COM library on the current thread. 
  CoInitialize(NULL);

  // Creates a single uninitalized object by passing in a CLSID (Class ID). 
  hr = CoCreateInstance(
    __uuidof(MMDeviceEnumerator), // Create the MMDevice Enumerator object
    NULL, 
    CLSCTX_INPROC_SERVER,  // Execution context, a.k.a that the object run in the same process as the application.
    __uuidof(IMMDeviceEnumerator), // Interface identifier, e.g. IMMDeviceEnumerator (matches pointer below).
    (LPVOID *) &enumerator // Pointer to recieve the interface.
  );
  
  enumerator->GetDevice(
    id,
    &device
  );
  // Clean up
  enumerator->Release();
  // Closes the COM library on the current thread.
  CoUninitialize();
  return device;
}

IAudioEndpointVolume* getVolume(int mic){
  HRESULT hr;
  IMMDeviceEnumerator *enumerator = NULL;
  IAudioEndpointVolume *volume = NULL;
  IMMDevice *defaultDevice = NULL;
  CoInitialize(NULL);
  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *) &enumerator);
  hr = enumerator->GetDefaultAudioEndpoint(mic ? eCapture : eRender, eConsole, &defaultDevice);
  if (hr != 0) {
    return volume;
  }
  hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *) &volume);
  enumerator->Release();
  defaultDevice->Release();
  CoUninitialize();
  return volume;
}

void get(const FunctionCallbackInfo<Value>& args) {
  float volume = 0;
  int mic = (int) args[0]->IntegerValue();
  IAudioEndpointVolume *temp_volume = NULL;
  temp_volume = getVolume(mic);
  if (temp_volume == NULL) {
    args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), -999));
    return;
  }
  temp_volume->GetMasterVolumeLevelScalar(&volume);
  int value = (int) round(volume*100);
  args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), value));
}

void isMute(const FunctionCallbackInfo<Value>& args) {
  int mute = 0;
  int mic = (int) args[0]->IntegerValue();
  IAudioEndpointVolume *temp_volume = NULL;
  temp_volume = getVolume(mic);

  if (temp_volume == NULL) {
    args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), -999));
    return;
  }

  temp_volume->GetMute(&mute);
  args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), mute));
  
}

void mute(const FunctionCallbackInfo<Value>& args) {
  int mic = (int) args[0]->IntegerValue();
  int mute = (int) args[1]->IntegerValue();

  IAudioEndpointVolume *temp_volume = NULL;
  temp_volume = getVolume(mic);

  if (temp_volume == NULL) {
    return;
  }

  temp_volume->SetMute(mute, NULL);

}

void getVolumeByDevice(const FunctionCallbackInfo<Value>& args) {
  LPCWSTR deviceId = (LPCWSTR) * v8::String::Value(args[0]->ToString());
  IMMDevice *device = getDeviceById(deviceId);
  IAudioEndpointVolume *volumeEndpoint = NULL;
  device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *) &volumeEndpoint);
  if (volumeEndpoint == NULL) {
    args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), -999));
    return;
  }
  float volume = 0;
  volumeEndpoint->GetMasterVolumeLevelScalar(&volume);
  int value = (int) round(volume*100);
  args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), value));
}

void setVolumeByDevice(const FunctionCallbackInfo<Value>& args) {
  float newVolume = (float) args[1]->IntegerValue()/100.0f;

  LPCWSTR deviceId = (LPCWSTR) * v8::String::Value(args[0]->ToString());
  IMMDevice *device = getDeviceById(deviceId);
  IAudioEndpointVolume *volumeEndpoint = NULL;
  device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *) &volumeEndpoint);
  if (volumeEndpoint == NULL) {
    return;
  }
  volumeEndpoint->SetMasterVolumeLevelScalar(newVolume, NULL);
}

void set(const FunctionCallbackInfo<Value>& args) {
  float newVolume = (float) args[0]->IntegerValue()/100.0f;
  int mic = (int) args[1]->IntegerValue();

  IAudioEndpointVolume *temp_volume = NULL;
  temp_volume = getVolume(mic);

  if (temp_volume == NULL) {
    return;
  }

  temp_volume->SetMasterVolumeLevelScalar(newVolume, NULL);

}

void listDevices(const FunctionCallbackInfo<Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  
  IMMDeviceEnumerator *enumerator = NULL;
  IMMDevice *defaultDevice = NULL;
  HRESULT hr;

  // Initializes the COM library on the current thread. 
  CoInitialize(NULL);

  // Creates a single uninitalized object by passing in a CLSID (Class ID). 
  hr = CoCreateInstance(
    __uuidof(MMDeviceEnumerator), // Create the MMDevice Enumerator object
    NULL, 
    CLSCTX_INPROC_SERVER,  // Execution context, a.k.a that the object run in the same process as the application.
    __uuidof(IMMDeviceEnumerator), // Interface identifier, e.g. IMMDeviceEnumerator (matches pointer below).
    (LPVOID *) &enumerator // Pointer to recieve the interface.
  );
  
  IMMDeviceCollection *collection;

  // TODO: HR
  enumerator->EnumAudioEndpoints(
    eCapture,
    DEVICE_STATE_ACTIVE,
    &collection
  );

  UINT count;
  collection->GetCount(&count);
  v8::Local<v8::Array> devices = v8::Array::New(isolate);

  for (int i = 0; i < count; i++) {
      IMMDevice *device;
      collection->Item(i, &device);
      LPWSTR id;
      device->GetId(&id);
      v8::Local<v8::Object> seralizedDevice = v8::Object::New(isolate);
      seralizedDevice->Set(
        v8::String::NewFromUtf8(isolate, "id"),
        encode(std::wstring(id))
      );

      IPropertyStore *props;
      device->OpenPropertyStore(STGM_READ, &props);

      PROPVARIANT name;
      PropVariantInit(&name);
      props->GetValue(PKEY_Device_FriendlyName, &name);

      seralizedDevice->Set(
        v8::String::NewFromUtf8(isolate, "name"),
        encode(std::wstring(name.pwszVal))
      );

      devices->Set(i, seralizedDevice);
  }

  // Clean up
  enumerator->Release();
  collection->Release();
  // Closes the COM library on the current thread.
  CoUninitialize();

  args.GetReturnValue().Set(devices);
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "get", get);
  NODE_SET_METHOD(exports, "set", set);
  NODE_SET_METHOD(exports, "mute", mute);
  NODE_SET_METHOD(exports, "isMute", isMute);
  NODE_SET_METHOD(exports, "listDevices", listDevices);
  NODE_SET_METHOD(exports, "getVolumeByDevice", getVolumeByDevice);
  NODE_SET_METHOD(exports, "setVolumeByDevice", setVolumeByDevice);
}

NODE_MODULE(addon, init)
