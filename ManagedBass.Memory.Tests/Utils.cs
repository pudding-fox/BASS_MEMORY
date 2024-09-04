using NUnit.Framework;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;

namespace ManagedBass.Memory.Tests
{
    public static class ProgressHandler
    {
        public static ConcurrentDictionary<string, IList<BassMemoryProgress>> Events = new ConcurrentDictionary<string, IList<BassMemoryProgress>>(StringComparer.OrdinalIgnoreCase);

        public static void Attach(Action<BassMemoryProgressHandler> source)
        {
            source(Progress);
            Events.Clear();
        }

        public static void Detach(Action<BassMemoryProgressHandler> source)
        {
            source(null);
            Events.Clear();
        }

        public static void Progress(ref BassMemoryProgress progress)
        {
            Events.GetOrAdd(progress.File, key => new List<BassMemoryProgress>()).Add(progress);
        }
    }

    public static class ChannelReader
    {
        const int BASS_STREAMPROC_END = -2147483648;

        public static int GetHashCode(int channelHandle)
        {
            var info = Bass.ChannelGetInfo(channelHandle);
            if (info.Flags.HasFlag(BassFlags.Float))
            {
                return GetFloatHashCode(channelHandle);
            }
            else
            {
                return GetShortHashCode(channelHandle);
            }
        }

        public static int GetShortHashCode(int channelHandle)
        {
            var eof = false;
            var buffer = new short[4096];
            var hashCode = default(int);
            while (Bass.ChannelIsActive(channelHandle) != PlaybackState.Stopped && !eof)
            {
                var length = Bass.ChannelGetData(channelHandle, buffer, buffer.Length);
                if (length < 0)
                {
                    TestContext.Out.WriteLine("Bass.ChannelGetData: {0}", Enum.GetName(typeof(Errors), Bass.LastError));
                    length = 0;
                }
                else if ((length & BASS_STREAMPROC_END) == BASS_STREAMPROC_END)
                {
                    length &= ~BASS_STREAMPROC_END;
                    eof = true;
                }
                for (var position = 0; position < length; position++)
                {
                    unchecked
                    {
                        hashCode += buffer[position].GetHashCode();
                    }
                }
            }
            return hashCode;
        }

        public static int GetFloatHashCode(int channelHandle)
        {
            var eof = false;
            var buffer = new float[4096];
            var hashCode = default(int);
            while (Bass.ChannelIsActive(channelHandle) != PlaybackState.Stopped && !eof)
            {
                var length = Bass.ChannelGetData(channelHandle, buffer, buffer.Length);
                if (length < 0)
                {
                    TestContext.Out.WriteLine("Bass.ChannelGetData: {0}", Enum.GetName(typeof(Errors), Bass.LastError));
                    length = 0;
                }
                else if ((length & BASS_STREAMPROC_END) == BASS_STREAMPROC_END)
                {
                    length &= ~BASS_STREAMPROC_END;
                    eof = true;
                }
                for (var position = 0; position < length; position++)
                {
                    unchecked
                    {
                        hashCode += buffer[position].GetHashCode();
                    }
                }
            }
            return hashCode;
        }
    }
}
