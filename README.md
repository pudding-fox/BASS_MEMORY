# BASS_MEMORY
An in memory playback plugin for BASS with .NET bindings.

bass.dll is required for native projects.
ManagedBass is required for .NET projects.

A simple example;

```c#
BassMemory.Load()
Bass.Init();

var sourceChannel = BassMemory.CreateStream(fileName);
Bass.ChannelPlay(sourceChannel);

while (Bass.ChannelIsActive(sourceChannel) == PlaybackState.Playing)
{
	global::System.Threading.Thread.Sleep(1000);
}

Bass.StreamFree(sourceChannel);
BassMemory.Unload();
Bass.Free();
```

A memory stream can also be created from an existing channel, this will buffer the data uncompressed (as PCM):

```c#
var sourceChannel = Bass.CreateStream(fileName);
var memoryChannel = BassMemory.CreateStream(sourceChannel);
```

Uncompressed channels have a size limit of ~4GB due to the internal WAVE format being 32 bit.

DSD files can also be handled with the `BassMemory.Dsd` type, all functions are duplicated there.

Buffering progress can be monitored with the `BassMemory.Progress` callback:

```c#
//Set this flag to cancel stream creation.
var cancel = false;

BassMemory.Progress((ref BassMemoryProgress progress) =>
{
	var percent = ((float)progress.Position / progress.Length) * 100;

	//Update the UI.
	...

	progress.Cancel = cancel;
});

var sourceChannel = BassMemory.CreateStream(fileName);
```