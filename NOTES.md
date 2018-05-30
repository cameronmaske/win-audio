From IMMDeviceEnumerator call EnumAudioEndpoints (method) and get IMMDeviceCollection.
Using IMMDeviceCollection, call GetCount (method) to get total number of devicse.
Then using Item(method) to get IMMDevice.

To get the volume, on the IMMDevice, you call Activate (method) with IAudioEndpointVolume.

On IMMDevice, to get the id, call GetId (method)
To get the name, call GetValue (method) with PKEY_Device_FriendlyName. The result returned is a PROPVARIANT. 
Extract the pswzVal using wstring (see: https://github.com/ukatama/node-native-sound-player/blob/master/wrapper/windows.cc#L95)


  // hr is the HRESULT. Used to represent error/warning conditions. 



An interface is one of the more overloaded and confusing terms in development.

It is actually a concept of abstraction and encapsulation. For a given "box", it declares the "inputs" and "outputs" of that box. In the world of software, that usually means the operations that can be invoked on the box (along with arguments) and in some cases the return types of these operations.