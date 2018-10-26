#pragma once
#define _WINSOCKAPI_
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <stdio.h>
#include <iostream>
#include <nan.h>

IAudioEndpointVolume* getVolumeCOM() {

  // Flag used in Windows to represent error conditions, and warning conditions.
  HRESULT hr;

  // Initializes the COM library on the current thread and identifies the concurrency model as single-thread apartment (STA).
  CoInitialize(NULL);

  // The IMMDeviceEnumerator interface provides methods for enumerating multimedia device resources. Basically the interface of the requested object.
  IMMDeviceEnumerator *deviceEnumerator = NULL;

  // Creates a single uninitialized object of the class associated with a specified CLSID
  hr = CoCreateInstance(
     __uuidof(MMDeviceEnumerator),  // Requested COM device enumerator id
    NULL,                           // If NULL, indicates that the object is not being created as part of an aggregate.
    CLSCTX_INPROC_SERVER,           // Context in which the code that manages the newly created object will run (Same process).
    __uuidof(IMMDeviceEnumerator),  // A reference to the identifier of the interface to be used to communicate with the object.
    (LPVOID *)&deviceEnumerator     // Address of pointer variable that receives the interface pointer requested in riid.
  );

  if (hr != S_OK) {
    return NULL;
  }

  // Device interface pointer where we will dig the audio device endpoint
  IMMDevice *defaultDevice = NULL;

  hr = deviceEnumerator->GetDefaultAudioEndpoint(
    eRender,          // Audio rendering stream. Audio data flows from the application to the audio endpoint device, which renders the stream. eCapture would be the opposite
    eConsole,         // The role that the system has assigned to an audio endpoint device. eConsole for games, system notification sounds, and voice commands
    &defaultDevice    // Pointer to default audio enpoint device
  );

  deviceEnumerator->Release(); // Release the handle to the device enumerator, we already got the handle for default device
  deviceEnumerator = NULL;     // Clear the pointer

  IAudioEndpointVolume *endpointVolume = NULL;

  hr = defaultDevice->Activate(       // Creates a COM object with the specified interface.
    __uuidof(IAudioEndpointVolume),   // Reference to a GUID that identifies the interface that the caller requests be activated
    CLSCTX_INPROC_SERVER,             // Context in which the code that manages the newly created object will run (Same process).
    NULL,                             // Set NULL to activate the IAudioEndpointVolume endpoint https://msdn.microsoft.com/en-us/library/ms679029.aspx
    (LPVOID *)&endpointVolume         //  Pointer to a pointer variable into which the method writes the address of the interface specified by parameter iid. Through this method, the caller obtains a counted reference to the interface.
  );

  defaultDevice->Release(); // We got the handle to the volume interface so the device handle is no more needed
  defaultDevice = NULL;     // Clear the pointer

  // Closes the COM library on the current thread, unloads all DLLs loaded by the thread, frees any other resources that the thread maintains, and forces all RPC connections on the thread to close.
  CoUninitialize();

  return endpointVolume;
}

void checkErrors(HRESULT hr, std::string action) {
  if (hr != S_OK) {
    std::cout << "Error when " << action << ": " << hr << std::endl;
  }

}

float setVolume(float vol) {
  float newVolume = vol;
  IAudioEndpointVolume *endpointVolume = getVolumeCOM();

  if (newVolume > 1.0) {
    newVolume = 1.0;
  }

  if (newVolume < 0) {
    newVolume = 0;
  }

  checkErrors(
    endpointVolume->SetMasterVolumeLevelScalar(newVolume, NULL),
    "setting volume"
  );
  return newVolume;
}

float getVolume () {
  float currentVolume = 0;

  IAudioEndpointVolume *endpointVolume = getVolumeCOM();
  checkErrors(
    endpointVolume->GetMasterVolumeLevelScalar(&currentVolume),
    "getting volume"
  );

  return currentVolume;
}

bool isMuted() {
  BOOL muted = false;

  IAudioEndpointVolume *endpointVolume = getVolumeCOM();
  checkErrors(endpointVolume->GetMute(&muted), "getting muted state");

  return muted;
}

bool setMute(BOOL mute) {
  if (isMuted() && mute) {
    return mute;
  }

  IAudioEndpointVolume *endpointVolume = getVolumeCOM();
  checkErrors(endpointVolume->SetMute(mute, NULL), "setting mute");

  return mute;
}

NAN_METHOD(GetVolume) {
  info.GetReturnValue().Set(Nan::New(getVolume()));
}

NAN_METHOD(SetVolume) {
  float newVolume = (float)info[0]->NumberValue();
  info.GetReturnValue().Set(Nan::New(setVolume(newVolume)));
}

NAN_METHOD(IsMuted) {
  info.GetReturnValue().Set(Nan::New(isMuted()));
}

NAN_METHOD(SetMute) {
  bool muted = info[0]->BooleanValue();
  info.GetReturnValue().Set(Nan::New(setMute(muted)));
}

NAN_MODULE_INIT(init) {
  target->Set(  Nan::New("getVolume").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(GetVolume)->GetFunction());
  target->Set(  Nan::New("setVolume").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(SetVolume)->GetFunction());
  target->Set(  Nan::New("isMuted").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(IsMuted)->GetFunction());
  target->Set(  Nan::New("setMute").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(SetMute)->GetFunction());
}

NODE_MODULE(addon, init)